/* Copyright 2018 Google Inc. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// Times running matrix multiplications.

#include "adc.h"
#include "debug_log.h"

#define ADC_LOG_LENGTH (256)

enum Padding {
  VALID = 1,  // No padding.
  SAME = 2,   // Input and output layers have the same size.
};

void ReferenceConv(const uint8_t* input_data,
		  int input_batches, int input_height, int input_width,
		  int input_depth, int input_offset, const uint8_t* filter_data,
		  int filter_height, int filter_width, int filter_count,
		  int filter_offset, int stride, enum Padding padding,
		  uint8_t* output_data, int output_height, int output_width,
		  int output_shift, int output_offset, int output_mult) {
  // Set up some constants we need for the output down-shifting and
  // saturation.
  const int32_t highest = (1 << 30) - 1;
  const int32_t lowest = -(1 << 30);

  // When we're converting the 32 bit accumulator to a lower bit depth, we
  // need to add on 0.5 in fixed-point terms to make the operation round half
  // up towards positive infinity, rather than a floor.
  // We also need to watch out for the case when there's no down shift,
  // because a left shift by a negative number gives undefined results.
  const int32_t rounding = (output_shift < 1) ? 0 : (1 << (output_shift - 1));

  // The two different padding modes we support can be a bit confusing. SAME
  // means we're trying to produce an output image that's the same size as the
  // input. It's complicated by stride, which shrinks the output image by a
  // a factor, but it means we end up sampling from outside the borders of the
  // input. These out-of-bounds values are read as zeroes. VALID means only
  // produce output values where the filters can read all their values from
  // within the input image. It effectively removes the margins of the output
  // image compared to the one produced by SAME. Stride complicates this
  // definition though, because it can result in the right and bottom filter
  // patches sampling from outside the borders if it's greater than 1.
  // Most of the logic for sorting this all out is done before this function,
  // when we calculate the output size, but the positioning of the origin of
  // the filters is different between the two modes, since SAME positions the
  // first filter off the edge of the input.
  int filter_left_offset;
  int filter_top_offset;
  if (padding == VALID) {
    filter_left_offset =
      ((output_width - 1) * stride + filter_width - input_width + 1) / 2;
    filter_top_offset =
      ((output_height - 1) * stride + filter_height - input_height + 1) / 2;
  } else {
    filter_left_offset =
      ((output_width - 1) * stride + filter_width - input_width) / 2;
    filter_top_offset =
      ((output_height - 1) * stride + filter_height - input_height) / 2;
  }

  // If we've got multiple images in our input, work through each of them.
  for (int batch = 0; batch < input_batches; ++batch) {
    // Walk through all the output image values, sliding the filter to
    // different
    // positions in the input.
    for (int out_y = 0; out_y < output_height; ++out_y) {
      for (int out_x = 0; out_x < output_width; ++out_x) {
	// Each filter kernel produces one output channel.
	for (int out_channel = 0; out_channel < filter_count; ++out_channel) {
	  // We're going to calculate a single output value, which means we
	  // need to multiply a three dimensional kernel of weights against
	  // the current location within the input image.
	  /*
	   *-------------------------------...
              |\ ^
              | \in_depth
              |  \ v
              |   *-------------------------------...
              |   |            ^
              |   |       in_y_origin
              |   |            v   \
              |   |<in_x_origin>*---*^
              |   |            \|   |filter_height
              .   |             *---*v
              .   |             <--->
                  .         filter_width
                  .
	  */
	  const int in_x_origin = (out_x * stride) - filter_left_offset;
	  const int in_y_origin = (out_y * stride) - filter_top_offset;
	  int32_t total = 0;
	  for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
	    for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
	      for (int in_channel = 0; in_channel < input_depth;
		   ++in_channel) {
		const int in_x = in_x_origin + filter_x;
		const int in_y = in_y_origin + filter_y;
		int32_t input_value;
		// If the location is outside the bounds of the input image,
		// use zero as a default value.
		if ((in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
		    (in_y < input_height)) {
		  const uint8_t input_source_value =
		    input_data[(batch * input_height * input_width *
				input_depth) +
			       (in_y * input_width * input_depth) +
			       (in_x * input_depth) + in_channel];
		  // We're promoting the T1 type to a higher bit depth here as
		  // we do the subtraction.
		  input_value =
		    (int32_t)(input_source_value) - input_offset;
		} else {
		  input_value = 0;
		}
		const uint8_t filter_source_value =
		  filter_data[(filter_y * filter_width * input_depth *
			       filter_count) +
			      (filter_x * input_depth * filter_count) +
			      (in_channel * filter_count) + out_channel];
		// Another promotion to 32 bit, as above.
		const int32_t filter_value =
		  (int32_t)(filter_source_value) - filter_offset;
		total += (input_value * filter_value);
	      }
	    }
	  }
	  // Here we're applying scale factors to compress the 32 bit
	  // accumulated total to a potentially lower bit depth.
	  const int32_t output =
	    ((((total + output_offset) * output_mult) + rounding) >>
	     output_shift);
	  output_data[(batch * output_height * output_width * filter_count) +
		      (out_y * output_width * filter_count) +
		      (out_x * filter_count) + out_channel] = output;
	}
      }
    }
  }
}

static inline void BenchmarkSmallReferenceConv() {
  const int image_depth = 1;
  const int image_width = 4;
  const int image_height = 3;
  const int image_batch_count = 1;

  // The image matrix is:
  // |  1 |  2 |  3 |  4 |
  // |  5 |  6 |  7 |  8 |
  // |  9 | 10 | 11 | 12 |
  const int32_t image_offset = 0;
  const uint8_t image_data[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  };

  // The filter matrix is:
  // | 1 | 4 | 7 |
  // | 2 | 5 | 8 |
  // | 3 | 6 | 9 |
  const int filter_size = 3;
  const int filter_count = 1;
  const int32_t filter_offset = 0;
  const int stride = 1;
  const uint8_t filter_data[] = {
    1, 4, 7, 2, 5, 8, 3, 6, 9,
  };

  // We're sliding the 3x3 filter across the 3x4 image, with accesses outside
  // the input set to zero because we're using the 'SAME' padding mode.
  // The calculations behind the expected output are:
  // (1*0)+(4*0)+(7*0)+(2*0)+(5*1)+(8*2)+(3*0)+(6*5)+(9*6)=105
  // (1*0)+(4*0)+(7*0)+(2*1)+(5*2)+(8*3)+(3*5)+(6*6)+(9*7)=150
  // (1*0)+(4*0)+(7*0)+(2*2)+(5*3)+(8*4)+(3*6)+(6*7)+(9*8)=183
  // (1*0)+(4*0)+(7*0)+(2*3)+(5*4)+(8*0)+(3*7)+(6*8)+(9*0)=95
  // (1*0)+(4*1)+(7*2)+(2*0)+(5*5)+(8*6)+(3*0)+(6*9)+(9*10)=235
  // (1*1)+(4*2)+(7*3)+(2*5)+(5*6)+(8*7)+(3*9)+(6*10)+(9*11)=312
  // (1*2)+(4*3)+(7*4)+(2*6)+(5*7)+(8*8)+(3*10)+(6*11)+(9*12)=357
  // (1*3)+(4*4)+(7*0)+(2*7)+(5*8)+(8*0)+(3*11)+(6*12)+(9*0)=178
  // (1*0)+(4*5)+(7*6)+(2*0)+(5*9)+(8*10)+(3*0)+(6*0)+(9*0)=187
  // (1*5)+(4*6)+(7*7)+(2*9)+(5*10)+(8*11)+(3*0)+(6*0)+(9*0)=234
  // (1*6)+(4*7)+(7*8)+(2*10)+(5*11)+(8*12)+(3*0)+(6*0)+(9*0)=261
  // (1*7)+(4*11)+(7*0)+(2*8)+(5*12)+(8*0)+(3*0)+(6*0)+(9*0)=121
  // This means we should end up with this matrix:
  // |  105  |  150  |  183  |   95  |
  // |  235  |  312  |  357  |  178  |
  // |  187  |  234  |  261  |  121  |
  const int expected_width = image_width;
  const int expected_height = image_height * filter_count;
  const uint8_t expected_data[] =
    {105, 150, 183, 95, 235, 312, 357, 178, 187, 234, 261, 121};

  const int expected_elements = expected_height * expected_width;
  const int output_shift = 0;
  const int output_offset = 0;
  const int output_mult = 1;
  uint8_t output_data[expected_elements];
  const int repetitions = 1000;
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    ReferenceConv(image_data,
		  image_batch_count, image_height, image_width,
		  image_depth, image_offset, filter_data,
		  filter_size, filter_size, filter_count,
		  filter_offset, stride, SAME,
		  output_data, expected_height, expected_width,
		  output_shift, output_offset, output_mult);
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_conv = (duration * 1000) / repetitions;
  const int32_t op_count =
	    expected_elements * image_depth * filter_size * filter_size * 2;
  const int32_t ops_per_second = (op_count * 1000000) / microseconds_per_conv; 
  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "Small ReferenceConv took: ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_conv, 10);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count, 10);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second, 10);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n ");
  DebugLog(adc_log);

  int did_all_match = 1;
  for (int i = 0; i < expected_elements; ++i) {
    if (expected_data[i] != output_data[i]) {
      char adc_log[ADC_LOG_LENGTH];
      StrCpy(adc_log, ADC_LOG_LENGTH, "Error: output_data[");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, i, 10);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "](");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, output_data[i], 10);
      StrCatStr(adc_log, ADC_LOG_LENGTH, ") != ");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, expected_data[i], 10);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "\r\n");
      DebugLog(adc_log);
    }
  }
}

char adc_log[ADC_LOG_LENGTH];

void OnReset(void) {
  // Start up the clock system.
  RccInitForAdc();

  TimerInit(TIMERID_TIM1);

  BenchmarkSmallReferenceConv();
}
