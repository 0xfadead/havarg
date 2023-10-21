#ifndef __COMMON_H__
#define __COMMON_H__

/* Global loggers (shared between files to avoid dependency loops) */
extern void (*__ha_common_ErrLogger)(const char *fmt, ...);
extern void (*__ha_common_WarnLogger)(const char *fmt, ...);
/* --------------------------------------------------------------- */

/* Output channels for errors and warnings */
#ifndef __HA_COMMON_WARNING_OUTPUT_CHANNEL
#define __HA_COMMON_WARNING_OUTPUT_CHANNEL stderr
#endif

#ifndef __HA_COMMON_ERROR_OUTPUT_CHANNEL
#define __HA_COMMON_ERROR_OUTPUT_CHANNEL stderr
#endif
/* ----------------------------------- */

/* Functions to change global loggers */
void __ha_common_ChangeGlobalErrLogger(void ErrLogger(const char *, ...));
void __ha_common_ChangeGlobalWarnLogger(void WarnLogger(const char *, ...));
/* ---------------------------------- */

/* Default global loggers */
void __ha_common_DefaultErrorLogger(const char *fmt, ...);
void __ha_common_DefaultWarningLogger(const char *fmt, ...);
/* ---------------------- */

/* Quality of life functions */
void __ha_common_puterr(const char *s);
void __ha_common_printerr(const char *fmt, ...);
/* ------------------------- */

/* Internal function for padding with space (who would've guessed) */
const char *fmt_padspace(unsigned long number);
/* --------------------------------------------------------------- */

/* Global error enum */
enum ha_ArgumentError {
  HA_AERROR_SUCCESS,

  HA_AERROR_MISSING_ARGUMENT,
  HA_AERROR_EXCESS_ARGUMENT,
  HA_AERROR_UNKNOWN_ARGUMENT,

  HA_AERROR_MISSING_PARAMETER,
  HA_AERROR_EXCESS_PARAMETER,
  HA_AERROR_INVALID_PARAMETER,

  HA_AERROR_NUMBER_CONVERSION_ERROR,

  HA_AERROR_MEMORY_ALLOCATION,

  HA_AERROR_UNKNOWN,

  HA_AERROR_NUMBER,
};
/* ----------------- */

/* Global error message array */

/**
 * Old description:
 *   Error message array for `ha_ArgumentError`
 *   Warning: These error messages are meant for internal use
 *   and are therefore very short.
 *
 */

extern const char ha_errormsg[HA_AERROR_NUMBER][128];
/* --------------------- */

#endif /* __COMMON_H__ */
