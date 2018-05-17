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

void ReferenceConv(const uint8_t* input_data, int input_batches,
                   int input_height, int input_width, int input_depth,
                   int input_offset, const uint8_t* filter_data,
                   int filter_height, int filter_width, int filter_count,
                   int filter_offset, int stride, enum Padding padding,
                   uint8_t* output_data, int output_height, int output_width,
                   int output_shift, int output_offset, int output_mult) {
  // Set up some constants we need for the output down-shifting and
  // saturation.
  const int32_t highest = (1 << 8) - 1;
  const int32_t lowest = 0;

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
              for (int in_channel = 0; in_channel < input_depth; ++in_channel) {
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
                  input_value = (int32_t)(input_source_value)-input_offset;
                } else {
                  input_value = 0;
                }
                const int filter_index =
                    (filter_y * filter_width * input_depth * filter_count) +
                    (filter_x * input_depth * filter_count) +
                    (in_channel * filter_count) + out_channel;
                const uint8_t filter_source_value = filter_data[filter_index];
                // Another promotion to 32 bit, as above.
                const int32_t filter_value =
                    (int32_t)(filter_source_value)-filter_offset;
                total += (input_value * filter_value);
              }
            }
          }
          // Here we're applying scale factors to compress the 32 bit
          // accumulated total to a potentially lower bit depth.
          const int32_t output =
              ((((total + output_offset) * output_mult) + rounding) >>
               output_shift);
          const int32_t top_clamped_output =
              (output > highest) ? highest : output;
          const int32_t clamped_output =
              (top_clamped_output < lowest) ? lowest : top_clamped_output;
          const int output_index =
              (batch * output_height * output_width * filter_count) +
              (out_y * output_width * filter_count) + (out_x * filter_count) +
              out_channel;
          output_data[output_index] = clamped_output;
        }
      }
    }
  }
}

void SymmetricalConv(const int8_t* input_data, int input_batches,
		     int input_height, int input_width, int input_depth,
		     const int8_t* filter_data,
		     int filter_height, int filter_width, int filter_count,
		     int stride, enum Padding padding,
		     uint8_t* output_data, int output_height, int output_width,
		     int output_shift, int output_offset, int output_mult) {
  // Set up some constants we need for the output down-shifting and
  // saturation.
  const int32_t highest = (1 << 8) - 1;
  const int32_t lowest = 0;

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
              for (int in_channel = 0; in_channel < input_depth; ++in_channel) {
                const int in_x = in_x_origin + filter_x;
                const int in_y = in_y_origin + filter_y;
                int8_t input_value;
                // If the location is outside the bounds of the input image,
                // use zero as a default value.
                if ((in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                    (in_y < input_height)) {
                  input_value =
                      input_data[(batch * input_height * input_width *
                                  input_depth) +
                                 (in_y * input_width * input_depth) +
                                 (in_x * input_depth) + in_channel];
                } else {
                  input_value = 0;
                }
                const int filter_index =
                    (filter_y * filter_width * input_depth * filter_count) +
                    (filter_x * input_depth * filter_count) +
                    (in_channel * filter_count) + out_channel;
                const int8_t filter_value = filter_data[filter_index];
                total += (input_value * filter_value);
              }
            }
          }
          // Here we're applying scale factors to compress the 32 bit
          // accumulated total to a potentially lower bit depth.
          const int32_t output =
              ((((total + output_offset) * output_mult) + rounding) >>
               output_shift);
          const int32_t top_clamped_output =
              (output > highest) ? highest : output;
          const int32_t clamped_output =
              (top_clamped_output < lowest) ? lowest : top_clamped_output;
          const int output_index =
              (batch * output_height * output_width * filter_count) +
              (out_y * output_width * filter_count) + (out_x * filter_count) +
              out_channel;
          output_data[output_index] = clamped_output;
        }
      }
    }
  }
}

void FastSymmetricalConv(const int8_t* input_data, int input_batches,
			 int input_height, int input_width, int input_depth,
			 const int8_t* filter_data,
			 int filter_height, int filter_width, int filter_count,
			 int stride, enum Padding padding,
			 uint8_t* output_data, int output_height, int output_width,
			 int output_shift, int output_offset, int output_mult) {
  const int32_t highest = (1 << 8) - 1;
  const int32_t lowest = 0;

  const int32_t rounding = (output_shift < 1) ? 0 : (1 << (output_shift - 1));

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

  for (int batch = 0; batch < input_batches; ++batch) {
    const int8_t* input_batch = input_data + (batch * input_height * input_width *
					      input_depth);
    for (int out_y = 0; out_y < output_height; ++out_y) {
      const int in_y_origin = (out_y * stride) - filter_top_offset;
      int filter_start_y;
      if (in_y_origin < 0) {
	filter_start_y = -in_y_origin;
      } else {
	filter_start_y = 0;
      }
      int filter_end_y;
      const int overlap_y = (in_y_origin + filter_height) - input_height;
      if (overlap_y > 0) {
	filter_end_y = filter_height - overlap_y;
      } else {
	filter_end_y = filter_height;
      }
      for (int out_x = 0; out_x < output_width; ++out_x) {
	const int in_x_origin = (out_x * stride) - filter_left_offset;
	int filter_start_x;
	if (in_x_origin < 0) {
	  filter_start_x = -in_x_origin;
	} else {
	  filter_start_x = 0;
	}
	int filter_end_x;
	const int overlap_x = (in_x_origin + filter_width) - input_width;
	if (overlap_x > 0) {
	  filter_end_x = filter_width - overlap_x;
	} else {
	  filter_end_x = filter_width;
	}
        for (int out_channel = 0; out_channel < filter_count; ++out_channel) {
          int32_t total = 0;
	  const int8_t* filter_channel = filter_data + out_channel;
          for (int filter_y = filter_start_y; filter_y < filter_end_y; ++filter_y) {
	    const int in_y = in_y_origin + filter_y;
	    const int8_t* filter_row = filter_channel +
	      (filter_y * filter_width * input_depth * filter_count);
	    asm volatile(
			 "ldr r0, %[p_in_y]\n\t"
			 "ldr r1, %[p_input_width]\n\t"
			 "mul r6, r0, r1\n\t"
			 "ldr r0, %[p_input_batch]\n\t"
			 "mla r6, r6, %[input_depth], r0\n\t"
			 "mov r5, %[filter_start_x]\n\t"
			 "filter_x_loop%=:"
			 "mul r4, r5, %[input_depth]\n\t"
			 "mla r4, r4, %[filter_count], %[filter_row]\n\t"
			 "add r2, %[in_x_origin], r5\n\t"
			 "mla r2, r2, %[input_depth], r6\n\t"
			 "add r3, r2, %[input_depth]\n\t"
			 "input_pixel_loop%=:\n\t"
			 "ldrsb r1, [r4]\n\t"
			 "ldrsb r0, [r2], #1\n\t"
			 "add r4, %[filter_count]\n\t"
			 "mla %[total], r0, r1, %[total]\n\t"
			 "cmp r2, r3\n\t"
			 "blt input_pixel_loop%=\n\t"
			 "add r5, #1\n\t"
			 "cmp r5, %[filter_end_x]\n\t"
			 "blt filter_x_loop%=\n\t"
			 : [total] "+r" (total)
			 : [filter_count] "r" (filter_count),
			   [input_depth] "r" (input_depth),
			   [filter_start_x] "r" (filter_start_x),
			   [filter_end_x] "r" (filter_end_x),
			   [filter_row] "r" (filter_row),
			   [in_x_origin] "r" (in_x_origin),
			   [p_in_y] "m" (in_y),
			   [p_input_width] "m" (input_width),
			   [p_input_batch] "m" (input_batch)
			 : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "memory");
	    /* const int8_t* input_row = input_batch + (in_y * input_width * input_depth); */
            /* for (int filter_x = filter_start_x; filter_x < filter_end_x; ++filter_x) { */
	      /* const int in_x = in_x_origin + filter_x; */
	      /* const int8_t* filter_pixel = filter_row + */
	      /* 	(filter_x * input_depth * filter_count); */
	      /* const int8_t* input_pixel = input_row + (in_x * input_depth); */
	      /* const int8_t* input_end = input_pixel + input_depth; */
              /* while (input_pixel < input_end) { */
	      /* const int8_t input_value = *input_pixel; */
	      /* input_pixel += 1; */
	      /* const int8_t filter_value = *filter_pixel; */
	      /* filter_pixel += filter_count; */
	      /* total += (input_value * filter_value); */
              /* } */
            /* } */
          }
          // Here we're applying scale factors to compress the 32 bit
          // accumulated total to a potentially lower bit depth.
          const int32_t output =
              ((((total + output_offset) * output_mult) + rounding) >>
               output_shift);
          const int32_t top_clamped_output =
              (output > highest) ? highest : output;
          const int32_t clamped_output =
              (top_clamped_output < lowest) ? lowest : top_clamped_output;
          const int output_index =
	                (batch * output_height * output_width * filter_count) +
              (out_y * output_width * filter_count) + (out_x * filter_count) +
              out_channel;
          output_data[output_index] = clamped_output;
        }
      }
    }
  }
}

void FastSymmetricalOneChannelConv(const int8_t* input_data, int input_batches_f,
				   int input_height_f, int input_width_f,
				   const int8_t* filter_data,
				   int filter_height_f, int filter_width_f, int filter_count_f,
				   int stride_f, enum Padding padding_f,
				   uint8_t* output_data, int output_height_f, int output_width_f,
				   int output_shift, int output_offset, int output_mult) {

  const int input_batches = 1;
  const int input_height = 25;/*50*/;
  const int input_width = 40;
  const int filter_height = 10;
  const int filter_width = 8;
  const int filter_count = 8;
  const int stride = 1;
  const enum Padding padding = SAME;
  const int output_height = input_height;
  const int output_width = input_width;

  const int input_depth = 1;
  const int32_t highest = (1 << 8) - 1;
  const int32_t lowest = 0;

  const int32_t rounding = (output_shift < 1) ? 0 : (1 << (output_shift - 1));

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

  for (int batch = 0; batch < input_batches; ++batch) {
    const int8_t* input_batch = input_data + (batch * input_height * input_width *
					      input_depth);
    for (int out_y = 0; out_y < output_height; ++out_y) {
      const int in_y_origin = (out_y * stride) - filter_top_offset;
      int filter_start_y;
      if (in_y_origin < 0) {
	filter_start_y = -in_y_origin;
      } else {
	filter_start_y = 0;
      }
      int filter_end_y;
      const int overlap_y = (in_y_origin + filter_height) - input_height;
      if (overlap_y > 0) {
	filter_end_y = filter_height - overlap_y;
      } else {
	filter_end_y = filter_height;
      }
      for (int out_x = 0; out_x < output_width; ++out_x) {
	const int in_x_origin = (out_x * stride) - filter_left_offset;
	int filter_start_x;
	if (in_x_origin < 0) {
	  filter_start_x = -in_x_origin;
	} else {
	  filter_start_x = 0;
	}
	int filter_end_x;
	const int overlap_x = (in_x_origin + filter_width) - input_width;
	if (overlap_x > 0) {
	  filter_end_x = filter_width - overlap_x;
	} else {
	  filter_end_x = filter_width;
	}
        for (int out_channel = 0; out_channel < filter_count; ++out_channel) {
          int32_t total = 0;
	  const int8_t* filter_channel = filter_data + out_channel;
          for (int filter_y = filter_start_y; filter_y < filter_end_y; ++filter_y) {
	    const int in_y = in_y_origin + filter_y;
	    const int8_t* filter_row = filter_channel +
	      (filter_y * filter_width * input_depth * filter_count);
	    const int8_t* input_row = input_batch + (in_y * input_width * input_depth);
	    const int in_start_x = in_x_origin + filter_start_x;
	    const int in_end_x = in_x_origin + filter_end_x;
	    const int8_t* filter_pixel = filter_row + (filter_start_x * input_depth * filter_count);
	    const int8_t* input_pixel = input_row + (in_start_x * input_depth);
	    const int8_t* const input_pixel_end = input_row + (in_end_x * input_depth);
	    while (input_pixel < (input_pixel_end - 7)) {
	      const uint32_t input_value_u32_0 = *(uint32_t*)(input_pixel);
	      const uint32_t input_value_u32_1 = *(uint32_t*)(input_pixel + 4);
	      const uint32_t filter_value_u32_0 = *(uint32_t*)(filter_pixel);
	      const uint32_t filter_value_u32_1 = *(uint32_t*)(filter_pixel + 4);
	      const int8_t input_value_0 = (int8_t)(input_value_u32_0 >> 0);
	      const int8_t filter_value_0 = (int8_t)(filter_value_u32_0 >> 0);
	      total += (input_value_0 * filter_value_0);
	      const int8_t input_value_1 = (int8_t)(input_value_u32_0 >> 8);
	      const int8_t filter_value_1 = (int8_t)(filter_value_u32_0 >> 8);
	      total += (input_value_1 * filter_value_1);
	      const int8_t input_value_2 = (int8_t)(input_value_u32_0 >> 16);
	      const int8_t filter_value_2 = (int8_t)(filter_value_u32_0 >>16);
	      total += (input_value_2 * filter_value_2);
	      const int8_t input_value_3 = (int8_t)(input_value_u32_0 >> 24);
	      const int8_t filter_value_3 = (int8_t)(filter_value_u32_0 >> 24);
	      total += (input_value_3 * filter_value_3);
	      const int8_t input_value_4 = (int8_t)(input_value_u32_1 >> 0);
	      const int8_t filter_value_4 = (int8_t)(filter_value_u32_1 >> 0);
	      total += (input_value_4 * filter_value_4);
	      const int8_t input_value_5 = (int8_t)(input_value_u32_1 >> 8);
	      const int8_t filter_value_5 = (int8_t)(filter_value_u32_1 >> 8);
	      total += (input_value_5 * filter_value_5);
	      const int8_t input_value_6 = (int8_t)(input_value_u32_1 >> 16);
	      const int8_t filter_value_6 = (int8_t)(filter_value_u32_1 >>16);
	      total += (input_value_6 * filter_value_6);
	      const int8_t input_value_7 = (int8_t)(input_value_u32_1 >> 24);
	      const int8_t filter_value_7 = (int8_t)(filter_value_u32_1 >> 24);
	      total += (input_value_7 * filter_value_7);
	      input_pixel += input_depth * 8;
	      filter_pixel += input_depth * 8; //(input_depth * filter_count);
	    }
	    while (input_pixel < input_pixel_end) {
	      const int8_t input_value = *input_pixel;
	      const int8_t filter_value = *filter_pixel;
	      total += (input_value * filter_value);
	      input_pixel += input_depth;
	      filter_pixel += input_depth; //(input_depth * filter_count);
	    }
          }
          // Here we're applying scale factors to compress the 32 bit
          // accumulated total to a potentially lower bit depth.
          const int32_t output =
	    ((((total + output_offset) * output_mult) + rounding) >>
	     output_shift);
          const int32_t top_clamped_output =
	    (output > highest) ? highest : output;
          const int32_t clamped_output =
	    (top_clamped_output < lowest) ? lowest : top_clamped_output;
          const int output_index =
	    (batch * output_height * output_width * filter_count) +
	    (out_y * output_width * filter_count) + (out_x * filter_count) +
	    out_channel;
          output_data[output_index] = clamped_output;
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
  const uint8_t expected_data[] = {105, 150,         183,         95,
                                   235, 255 /*312*/, 255 /*357*/, 178,
                                   187, 234,         255 /*261*/, 121};

  const int expected_elements = expected_height * expected_width;
  const int output_shift = 0;
  const int output_offset = 0;
  const int output_mult = 1;
  uint8_t output_data[expected_elements];
  const int repetitions = 1000;
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    ReferenceConv(image_data, image_batch_count, image_height, image_width,
                  image_depth, image_offset, filter_data, filter_size,
                  filter_size, filter_count, filter_offset, stride, SAME,
                  output_data, expected_height, expected_width, output_shift,
                  output_offset, output_mult);
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_conv = (duration * 1000) / repetitions;
  const int32_t op_count =
      expected_elements * image_depth * filter_size * filter_size * 2;
  const int32_t ops_per_second = (op_count * 1000000) / microseconds_per_conv;
  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "Small ReferenceConv took: ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_conv);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n ");
  DebugLog(adc_log);

  int did_all_match = 1;
  for (int i = 0; i < expected_elements; ++i) {
    if (expected_data[i] != output_data[i]) {
      char adc_log[ADC_LOG_LENGTH];
      StrCpy(adc_log, ADC_LOG_LENGTH, "Error: output_data[");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, i);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "](");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, output_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, ") != ");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, expected_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "\r\n");
      DebugLog(adc_log);
    }
  }
}

static inline void BenchmarkSmallSymmetricalConv() {
  const int image_depth = 1;
  const int image_width = 4;
  const int image_height = 3;
  const int image_batch_count = 1;

  // The image matrix is:
  // |  1 |  2 |  3 |  4 |
  // |  5 |  6 |  7 |  8 |
  // |  9 | 10 | 11 | 12 |
  const int8_t image_data[] = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  };

  // The filter matrix is:
  // | 1 | 4 | 7 |
  // | 2 | 5 | 8 |
  // | 3 | 6 | 9 |
  const int filter_height = 3;
  const int filter_width = 3;
  const int filter_count = 1;
  const int stride = 1;
  const int8_t filter_data[] = {
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
  const uint8_t expected_data[] = {105, 150,         183,         95,
                                   235, 255 /*312*/, 255 /*357*/, 178,
                                   187, 234,         255 /*261*/, 121};

  const int expected_elements = expected_height * expected_width;
  const int output_shift = 0;
  const int output_offset = 0;
  const int output_mult = 1;
  uint8_t output_data[expected_elements];
  const int repetitions = 1000;
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    FastSymmetricalConv(image_data, image_batch_count, image_height, image_width,
			image_depth, filter_data, filter_height,
			filter_width, filter_count, stride, SAME,
			output_data, expected_height, expected_width, output_shift,
			output_offset, output_mult);
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_conv = (duration * 1000) / repetitions;
  const int32_t op_count =
      expected_elements * image_depth * filter_height * filter_width * 2;
  const int32_t ops_per_second = (op_count * 1000000) / microseconds_per_conv;
  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "Small SymmetricalConv took: ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_conv);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n ");
  DebugLog(adc_log);

  int did_all_match = 1;
  for (int i = 0; i < expected_elements; ++i) {
    if (expected_data[i] != output_data[i]) {
      char adc_log[ADC_LOG_LENGTH];
      StrCpy(adc_log, ADC_LOG_LENGTH, "Error: output_data[");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, i);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "](");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, output_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, ") != ");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, expected_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "\r\n");
      DebugLog(adc_log);
    }
  }
}

static void BenchmarkReferenceConv(int image_batch_count, int image_height,
                                   int image_width, int image_depth,
                                   int filter_height, int filter_width,
                                   int filter_count) {
  const int32_t image_offset = 128;
  const int image_elements =
      image_batch_count * image_height * image_width * image_depth;
  uint8_t image_data[image_elements];
  for (int i = 0; i < image_elements; ++i) {
    image_data[i] = (i % 256);
  }

  const int32_t filter_offset = 128;
  const int stride = 1;
  const int filter_elements =
      filter_count * filter_height * filter_width * image_depth;
  uint8_t filter_data[filter_elements];
  for (int i = 0; i < filter_elements; ++i) {
    filter_data[i] = (i % 256);
  }

  const int expected_width = image_width;
  const int expected_height = image_height;
  const int expected_elements =
      image_batch_count * expected_height * expected_width * filter_count;
  const int output_shift = 0;
  const int output_offset = 128;
  const int output_mult = 1;
  uint8_t output_data[expected_elements];
  const int repetitions = 10;
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    ReferenceConv(image_data, image_batch_count, image_height, image_width,
                  image_depth, image_offset, filter_data, filter_height,
                  filter_width, filter_count, filter_offset, stride, SAME,
                  output_data, expected_height, expected_width, output_shift,
                  output_offset, output_mult);
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_conv = (duration * 1000) / repetitions;
  const int32_t op_count =
      expected_elements * image_depth * filter_height * filter_width * 2;
  const int32_t ops_per_second =
      ((op_count * 1000) / microseconds_per_conv) * 1000;

  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "ReferenceConv(");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_batch_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_height);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_width);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_depth);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_height);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_width);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ") took ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_conv);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n");
  DebugLog(adc_log);

  DebugLog("Done\n");
}

static void BenchmarkSymmetricalConv(int image_batch_count, int image_height,
				     int image_width, int image_depth,
				     int filter_height, int filter_width,
				     int filter_count) {
  const int image_elements =
      image_batch_count * image_height * image_width * image_depth;
  int8_t image_data[image_elements];
  for (int i = 0; i < image_elements; ++i) {
    image_data[i] = (i % 256) - 128;
  }

  const int stride = 1;
  const int filter_elements =
      filter_count * filter_height * filter_width * image_depth;
  int8_t filter_data[filter_elements];
  for (int i = 0; i < filter_elements; ++i) {
    filter_data[i] = (i % 256) - 128;
  }
  
  const int expected_width = image_width;
  const int expected_height = image_height;
  const int expected_elements =
      image_batch_count * expected_height * expected_width * filter_count;
  const int output_shift = 0;
  const int output_offset = 128;
  const int output_mult = 1;
  uint8_t output_data[expected_elements];
  const int repetitions = 10;
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  if (image_depth == 1 && 0) {
    for (int i = 0; i < repetitions; ++i) {
      FastSymmetricalOneChannelConv(image_data, image_batch_count, image_height,
				    image_width, filter_data, filter_height,
				    filter_width, filter_count, stride, SAME,
				    output_data, expected_height, expected_width,
				    output_shift, output_offset, output_mult);
    }
  } else {
    for (int i = 0; i < repetitions; ++i) {
      FastSymmetricalConv(image_data, image_batch_count, image_height, image_width,
			  image_depth, filter_data, filter_height,
			  filter_width, filter_count, stride, SAME,
			  output_data, expected_height, expected_width, output_shift,
			  output_offset, output_mult);
    }
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_conv = (duration * 1000) / repetitions;
  const int32_t op_count =
      expected_elements * image_depth * filter_height * filter_width * 2;
  const int32_t ops_per_second =
      ((op_count * 1000) / microseconds_per_conv) * 1000;

  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "SymmetricalConv(");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_batch_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_height);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_width);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, image_depth);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_height);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_width);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, filter_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ") took ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_conv);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n");
  DebugLog(adc_log);

  uint8_t expected_data[expected_elements];
  SymmetricalConv(image_data, image_batch_count, image_height,
		  image_width, image_depth, filter_data, filter_height,
		  filter_width, filter_count, stride, SAME,
		  expected_data, expected_height, expected_width,
		  output_shift, output_offset, output_mult);
  
  int did_all_match = 1;
  for (int i = 0; i < expected_elements; ++i) {
    if (expected_data[i] != output_data[i]) {
      char adc_log[ADC_LOG_LENGTH];
      StrCpy(adc_log, ADC_LOG_LENGTH, "Error: output_data[");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, i);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "](");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, output_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, ") != ");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, expected_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "\r\n");
      DebugLog(adc_log);
    }
  }
}

void main(void) {
  // Start up the clock system.
  RccInitForAdc();

  TimerInit(TIMERID_TIM1);

  int32_t total = 0;
  int8_t input_value = 10;
  int8_t filter_value = 10;
  asm volatile("mla %[total], %[input_value], %[filter_value], %[total]\n\t"
	       : [total]"+r" (total)
	       : [input_value] "r" (input_value),
		 [filter_value] "r" (filter_value));
  LOG_INT32(total);
  
  BenchmarkSmallReferenceConv();
  BenchmarkSmallSymmetricalConv();
  BenchmarkReferenceConv(1, 5, 5, 2, 3, 3, 4);
  BenchmarkReferenceConv(1, 10, 10, 2, 3, 3, 4);
  BenchmarkReferenceConv(1, 10, 10, 10, 3, 3, 4);
  BenchmarkSymmetricalConv(1, 5, 5, 2, 3, 3, 4);
  BenchmarkSymmetricalConv(1, 10, 10, 2, 3, 3, 4);
  while (1) {
    BenchmarkSymmetricalConv(1, 40, 25/*50*/, 1, 10, 8, 8);
  }
}
