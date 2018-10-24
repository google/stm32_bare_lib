#include "debug_log.h"

void DebugLog(char* s) {
  asm("mov r0, #0x04\n"  // SYS_WRITE0
      "mov r1, %[str]\n"
      "bkpt #0xAB\n"
      :
      : [str] "r"(s)
      : "r0", "r1");
}

void DebugLogInt32(int32_t i) {
  char number_string[kFastToBufferSize];
  FastInt32ToBufferLeft(i, number_string);
  DebugLog(number_string);
}

void DebugLogUInt32(uint32_t i) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(i, number_string, 10);
  DebugLog(number_string);
}

void DebugLogHex(uint32_t i) {
  char number_string[kFastToBufferSize];
  FastUInt32ToBufferLeft(i, number_string, 16);
  DebugLog(number_string);
}

void DebugLogFloat(float i) {
  char number_string[kFastToBufferSize];
  FastFloatToBufferLeft(i, number_string);
  DebugLog(number_string);
}
