#ifndef __HAVARG_H__
#define __HAVARG_H__

#ifdef HA_ERROR_LOGGER_OUTPUT
#define __HA_COMMON_ERROR_OUTPUT_CHANNEL HA_ERROR_LOGGER_OUTPUT
#endif
#ifdef HA_WARNING_LOGGER_OUTPUT
#define __HA_COMMON_WARNING_OUTPUT_CHANNEL HA_WARNING_LOGGER_OUTPUT
#endif
#include "common.h"
#include <stdbool.h>

/* type of parameter for an argument */
enum ha_ParamType {
  HA_PARAMTYPE_STRING,
  HA_PARAMTYPE_LIST,
  HA_PARAMTYPE_NUMBER,
  HA_PARAMTYPE_NONE,
};
/* --------------------------------- */

/* a singe parameter template */
struct ha_ParameterTemplate {
  /* Is the parameter required? */
  bool required;

  /**
   * !! Ignore if the param_type isn't HA_PARAMTYPE_LIST !!
   * The minimum size of the list
   *
   */
  int minimum_list_size;

  /**
   * !! Ignore if the param_type isn't HA_PARAMTYPE_LIST !!
   * The maximum size of the list
   *
   */
  int maximum_list_size;

  /**
   * !! Ignore if the param_type isn't HA_PARAMTYPE_NUMBER !!
   * The base of the expected number
   *
   */
  int base;

  /* The type of the parameter */
  enum ha_ParamType param_type;
};
/* -------------------------- */

/* A single argument templates */
struct ha_ArgumentTemplate {
  /* A unique ID to specify the argument later */
  unsigned int id;

  /* Is the argument required? */
  bool required;

  /**
   * The long and short version of the argument name
   * If there is no long or short version, please set it to NULL.
   */
  const char *long_argname;
  const char *short_argname;

  struct ha_ParameterTemplate *param;
};
/* ---------------------------- */

/* a list of argument templates */
struct ha_ArgumentTemplates {
  int argtemplate_num;
  struct ha_ArgumentTemplate *argtemplates;
};
/* ---------------------------- */

/* a single parameter */
struct ha_Parameter {
  enum ha_ParamType parameter_type;

  const char *text;

  /* Won't be set if the type isn't HA_PARAMTYPE_LIST */
  unsigned long element_num;
  const char **list_elements;

  /* Upper- and lower bound for HA_PARAMTYPE_NUMBER */
  const long upper_bound;
  const long lower_bound;
  /* Will be zero of the type isn't HA_PARAMTYPE_NUMBER */
  long number;
};
/* ------------ */

/* a single argument */
struct ha_Argument {
  int index;
  const char *text;

  struct ha_Parameter *param;
};
/* ----------------- */

/* a list of arguments */
struct ha_Arguments {
  int argument_num;
  struct ha_Argument *arguments;

  enum ha_ArgumentError error;
};
/* ------------------- */

/* Initialize default loggers */
void ha_InitDefaultLoggers(void);
/* -------------------------- */

/**
 * Gets arguments from the templates.
 * argtemplates: user defined templates to get from argv
 * ac: alias for argc
 * av: alias for argv
 *
 */
struct ha_Arguments
ha_GetArguments(const struct ha_ArgumentTemplates *argtemplates, const int ac,
                const char **av);

/* Free the argument struct */
void ha_FreeArguments(const struct ha_Arguments *args);
/* ------------------------ */

/* Change the logger function for logging errors */
void ha_ChangeErrorLogger(void (*errorlogger)(const char *, ...));
/* --------------------------------------------- */

/* Change the logger function for logging warnings */
void ha_ChangeWarningLogger(void (*warninglogger)(const char *, ...));
/* ----------------------------------------------- */

#endif
