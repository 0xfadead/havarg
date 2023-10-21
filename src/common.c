#include "common.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void (*__ha_common_ErrLogger)(const char *fmt, ...);
void (*__ha_common_WarnLogger)(const char *fmt, ...);

void __ha_common_ChangeGlobalErrLogger(void ErrLogger(const char *, ...)) {
  __ha_common_ErrLogger = ErrLogger;

  return;
}

void __ha_common_ChangeGlobalWarnLogger(void WarnLogger(const char *, ...)) {
  __ha_common_WarnLogger = WarnLogger;

  return;
}

void __ha_common_DefaultErrorLogger(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  fputs("ERR:  ", stderr);
  vfprintf(__HA_COMMON_ERROR_OUTPUT_CHANNEL, fmt, args);

  va_end(args);
}

void __ha_common_DefaultWarningLogger(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  fputs("WARN: ", stderr);
  vfprintf(__HA_COMMON_WARNING_OUTPUT_CHANNEL, fmt, args);

  va_end(args);
}

void __ha_common_puterr(const char *s) {
  fputs(s, __HA_COMMON_ERROR_OUTPUT_CHANNEL);
  fputs("\n", __HA_COMMON_ERROR_OUTPUT_CHANNEL);
}

void __ha_common_printerr(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);

  vfprintf(__HA_COMMON_ERROR_OUTPUT_CHANNEL, fmt, args);

  va_end(args);
}

static char __fmt_padspace_buffer[1024];
static unsigned long __fmt_last_number = 0;
const char *fmt_padspace(unsigned long number) {
  if (number > 1023)
    number = 1023;
  memset(__fmt_padspace_buffer, ' ', number);

  if (__fmt_last_number > number) {
    __fmt_padspace_buffer[number] = 0;
  }
  __fmt_last_number = number;

  return __fmt_padspace_buffer;
}

const char ha_errormsg[HA_AERROR_NUMBER][128] = {
    /* HA_AERROR_SUCCESS */
    "Success",

    /* HA_AERROR_MISSING_ARGUMENT */
    "Missing argument",
    /* HA_AERROR_EXCESS_ARGUMENT */
    "Excess argument",
    /* HA_AERROR_UNKNOWN_ARGUMENT */
    "Unknown argument",

    /* HA_AERROR_MISSING_PARAMETER */
    "Missing parameter",
    /* HA_AERROR_EXCESS_PARAMETER */
    "Excess parameter",
    /* HA_AERROR_INVALID_PARAMETER */
    "Invalid parameter",

    /* HA_AERROR_NUMBER_CONVERSION */
    "Invalid number",

    /* HA_AERROR_MEMORY_ALLOCATION */
    "Memory allocation error",

    /* HA_AERROR_UNKNOWN */
    "Unknown error",
};
