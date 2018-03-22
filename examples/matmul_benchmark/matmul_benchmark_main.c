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

static inline void ReferenceEightBitIntGemm(
    int transpose_a, int transpose_b, int transpose_c, int m, int n, int k,
    const uint8_t* a, int32_t a_offset, int lda, const uint8_t* b,
    int32_t b_offset, int ldb, uint8_t* c, int32_t c_offset, int32_t c_mult_int,
    int32_t c_shift, int ldc) {
  int a_i_stride;
  int a_l_stride;
  if (transpose_a) {
    a_i_stride = 1;
    a_l_stride = lda;
  } else {
    a_i_stride = lda;
    a_l_stride = 1;
  }
  int b_j_stride;
  int b_l_stride;
  if (transpose_b) {
    b_j_stride = ldb;
    b_l_stride = 1;
  } else {
    b_j_stride = 1;
    b_l_stride = ldb;
  }
  int c_i_stride;
  int c_j_stride;
  if (transpose_c) {
    c_i_stride = 1;
    c_j_stride = ldc;
  } else {
    c_i_stride = ldc;
    c_j_stride = 1;
  }
  int i, j, l;

  const int32_t kRoundingTerm = (c_shift < 1) ? 0 : (1 << (c_shift - 1));

  for (j = 0; j < n; j++) {
    for (i = 0; i < m; i++) {
      int32_t total = 0;
      for (l = 0; l < k; l++) {
        const int a_index = i * a_i_stride + l * a_l_stride;
        const uint8_t a_as_byte = a[a_index];
        const int32_t a_as_int = (int32_t)(a_as_byte) + a_offset;
        const int b_index = j * b_j_stride + l * b_l_stride;
        const uint8_t b_as_byte = b[b_index];
        const int32_t b_as_int = (int32_t)(b_as_byte) + b_offset;
        const int32_t mult_as_int = a_as_int * b_as_int;
        total += mult_as_int;
      }
      int32_t output =
          (((total + c_offset) * c_mult_int) + kRoundingTerm) >> c_shift;
      if (output > 255) {
        output = 255;
      }
      if (output < 0) {
        output = 0;
      }
      const int c_index = i * c_i_stride + j * c_j_stride;
      c[c_index] = (uint8_t)(output);
    }
  }
}

static inline void BenchmarkSmallReferenceGemm() {
  // A matrix is:
  // |  1 |  2 |  3 |
  // |  4 |  5 |  6 |
  const int a_rows = 2;
  const int a_cols = 3;
  const uint8_t a_data[] = {
      1, 2, 3, 4, 5, 6,
  };

  // B matrix is:
  // |  7 |  8 |  9 | 10 |
  // | 11 | 12 | 13 | 14 |
  // | 15 | 16 | 17 | 18 |
  const int b_rows = 3;
  const int b_cols = 4;
  const uint8_t b_data[] = {
      7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  };

  // Here are the results we expect, from hand calculations:
  // (1 * 7) + (2 * 11) + (3 * 15) = 74
  // (1 * 8) + (2 * 12) + (3 * 16) = 80
  // (1 * 9) + (2 * 13) + (3 * 17) = 86
  // (1 * 10) + (2 * 14) + (3 * 18) = 92
  // (4 * 7) + (5 * 11) + (6 * 15) = 173
  // (4 * 8) + (5 * 12) + (6 * 16) = 188
  // (4 * 9) + (5 * 13) + (6 * 17) = 203
  // (4 * 10) + (5 * 14) + (6 * 18) = 218
  const int c_rows = 2;
  const int c_cols = 4;
  const uint8_t expected_c_data[] = {
      74, 80, 86, 92, 173, 188, 203, 218,
  };

  const int repetitions = 1000;
  uint8_t c_data[8];
  volatile uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    ReferenceEightBitIntGemm(0, 0, 0, a_rows, b_cols, a_cols, a_data, 0, a_cols,
                             b_data, 0, b_cols, c_data, 0, 1, 0, c_cols);
  }
  volatile uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_gemm = (duration * 1000) / repetitions;
  const int32_t op_count = a_rows * b_cols * a_cols * 2;
  const int32_t ops_per_second = (op_count * 1000000) / microseconds_per_gemm;
  char adc_log[ADC_LOG_LENGTH];
  StrCpy(adc_log, ADC_LOG_LENGTH, "Small ReferenceGemm took: ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_gemm);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\n ");
  DebugLog(adc_log);

  int did_all_match = 1;
  for (int i = 0; i < (c_rows * c_cols); ++i) {
    if (expected_c_data[i] != c_data[i]) {
      char adc_log[ADC_LOG_LENGTH];
      StrCpy(adc_log, ADC_LOG_LENGTH, "Error: c_data[");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, i);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "](");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, c_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, ") != ");
      StrCatInt32(adc_log, ADC_LOG_LENGTH, expected_c_data[i]);
      StrCatStr(adc_log, ADC_LOG_LENGTH, "\r\n");
      DebugLog(adc_log);
    }
  }
}

char adc_log[ADC_LOG_LENGTH];

static inline void BenchmarkReferenceGemm(int m, int n, int k) {
  const int a_rows = m;
  const int a_cols = k;
  const int a_elements = a_rows * a_cols;
  uint8_t a_data[a_elements];
  for (int i = 0; i < a_elements; ++i) {
    a_data[i] = (i % 256);
  }

  const int b_rows = k;
  const int b_cols = n;
  const int b_elements = b_rows * b_cols;
  uint8_t b_data[b_elements];
  for (int i = 0; i < b_elements; ++i) {
    b_data[i] = (i % 256);
  }

  const int c_rows = m;
  const int c_cols = n;
  uint8_t c_data[c_rows * c_cols * 10];

  const int repetitions = 1000;
  const uint16_t start_time = TimerGetCounter(TIMERID_TIM1);
  for (int i = 0; i < repetitions; ++i) {
    ReferenceEightBitIntGemm(0, 0, 0, a_rows, b_cols, a_cols, a_data, 0, a_cols,
                             b_data, 0, b_cols, c_data, 0, 1, 0, c_cols);
  }
  const uint16_t duration = TimerGetCounter(TIMERID_TIM1) - start_time;
  const int32_t microseconds_per_gemm = (duration * 1000) / repetitions;
  const int32_t op_count = a_rows * b_cols * a_cols * 2;
  const int32_t ops_per_second =
      ((op_count * 1000) / microseconds_per_gemm) * 1000;
  StrCpy(adc_log, ADC_LOG_LENGTH, "ReferenceGemm(");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, m);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, n);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ", ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, k);
  StrCatStr(adc_log, ADC_LOG_LENGTH, ") took: ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, microseconds_per_gemm);
  StrCatStr(adc_log, ADC_LOG_LENGTH, "us (");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, op_count);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops, ");
  StrCatInt32(adc_log, ADC_LOG_LENGTH, ops_per_second);
  StrCatStr(adc_log, ADC_LOG_LENGTH, " ops/s)\r\n ");
  DebugLog(adc_log);
}

void main(void) {
  // Start up the clock system.
  RccInitForAdc();

  TimerInit(TIMERID_TIM1);

  BenchmarkSmallReferenceGemm();

  BenchmarkReferenceGemm(5, 5, 5);
  BenchmarkReferenceGemm(10, 10, 10);
  BenchmarkReferenceGemm(15, 15, 15);

  BenchmarkReferenceGemm(25, 25, 5);
  BenchmarkReferenceGemm(5, 25, 25);
  BenchmarkReferenceGemm(25, 5, 25);

  BenchmarkReferenceGemm(25, 25, 20);
}
