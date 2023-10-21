#include "havarg.h"
#include "common.h"
#include "error_methods.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO: Make variadic arguments ANSI C complient */
#define err(s, ...) (__ha_common_ErrLogger(s, ##__VA_ARGS__))
#define warn(s, ...) (__ha_common_WarnLogger(s, ##__VA_ARGS__))

#define puterr(s) (__ha_common_puterr(s))
#define printerr(fmt, ...) (__ha_common_printerr(fmt, ##__VA_ARGS__))

/* Change the logger function for logging errors */
void ha_ChangeErrorLogger(void (*errorlogger)(const char *, ...)) {
  __ha_common_ChangeGlobalErrLogger(errorlogger);
}
/* --------------------------------------------- */

/* Change the logger function for logging warnings */
void ha_ChangeWarningLogger(void (*warninglogger)(const char *, ...)) {
  __ha_common_ChangeGlobalWarnLogger(warninglogger);
}
/* ----------------------------------------------- */

/* Initialize default loggers */
void ha_InitDefaultLoggers(void) {
  __ha_common_ChangeGlobalErrLogger(__ha_common_DefaultErrorLogger);
  __ha_common_ChangeGlobalWarnLogger(__ha_common_DefaultWarningLogger);
}
/* -------------------------- */

/* Checks if a string is a valid number */
bool isvalidnumber(const char *in, struct ha_ParameterTemplate *ptemp) {
  char *endptr = NULL;
  strtol(in, &endptr, ptemp->base);
  if (endptr < in + strlen(in))
    return false;
  return true;
}
/* ------------------------------ */

/* Check for an allocation error */
bool isvalidalloc(void *param) {
  if (!param) {
    err("%s: %s\n", ha_errormsg[HA_AERROR_MEMORY_ALLOCATION], strerror(errno));
    return false;
  }

  return true;
}
/* ----------------------------- */

/* Set values for current argument */
enum ha_ArgumentError
populatecurrentargument(struct ha_Argument *arg, int ai, int ac,
                        const char **av,
                        const struct ha_ArgumentTemplate *atemp) {
  enum ha_ArgumentError error = HA_AERROR_SUCCESS;

  arg->text = av[ai];
  arg->index = atemp->id;
  arg->param = NULL;

  if (!atemp->param)
    goto main_return;

  arg->param = (struct ha_Parameter *)malloc(sizeof(struct ha_Parameter));

  printf("Argument: %s\n", av[ai]);

  const char *equal_pos = strstr(av[ai], "=");
  if (!equal_pos) {
    if (atemp->param->required) {
      __ha_err_print_missing_parameter_error(ac, av, ai,
                                             atemp->param->param_type);
      error = HA_AERROR_MISSING_ARGUMENT;

      goto main_return;
    }

    goto main_return;
  }

  arg->param->text = equal_pos + 1;

  printf("Found parameter: %s\n", equal_pos + 1);

  if (atemp->param->param_type == HA_PARAMTYPE_NUMBER) {
    if (!isvalidnumber(equal_pos + 1, atemp->param)) {
      __ha_err_print_invalid_parameter_error(ac, av, ai);

      error = HA_AERROR_INVALID_PARAMETER;
      goto main_return;
    }

    arg->param->number = strtol(equal_pos + 1, NULL, atemp->param->base);
  }

main_return:
  return error;
}
/* ------------------------------- */

/* Extracts the valid arguments */
struct ha_Arguments
getValidArguments(const struct ha_ArgumentTemplates *argtemplates, int ac,
                  const char **av) {

  struct ha_Arguments args = {0};
  args.error = HA_AERROR_SUCCESS;

  args.arguments = (struct ha_Argument *)calloc(ac, sizeof(struct ha_Argument));
  if (!isvalidalloc(args.arguments)) {
    args.error = HA_AERROR_MEMORY_ALLOCATION;
    goto main_return;
  }
  args.argument_num = 0;

  for (int arg = 1; arg < ac; arg++) {
    bool found_argument = false;
    for (int argname = 0; argname < argtemplates->argtemplate_num; argname++) {
      const struct ha_ArgumentTemplate curr_argtemp =
          argtemplates->argtemplates[argname];
      const unsigned long curr_arglen = strlen(av[arg]);
      const unsigned long sname_len = strlen(curr_argtemp.short_argname);
      const unsigned long lname_len = strlen(curr_argtemp.long_argname);
      /* argument output index */
      const int aoi = arg - 1;

      /* search for short name */
      if (!curr_argtemp.short_argname)
        goto long_name_comparison;

      /* We check for the short name first, since it's more efficient */
      if (curr_arglen < sname_len)
        goto long_name_comparison;

      /* Check if there is a parameter and discard longer arguments which do not
       * have one */
      if (curr_arglen > sname_len && av[arg][sname_len] != '=')
        goto long_name_comparison;

      /* Do a string compare */
      if (strncmp(av[arg], curr_argtemp.short_argname, sname_len))
        goto long_name_comparison;

      if (curr_arglen == sname_len) {
        /* If there's a required parameter this isn't the argument */
        if (curr_argtemp.param && curr_argtemp.param->required) {
          __ha_err_print_missing_parameter_error(
              ac, av, arg, curr_argtemp.param->param_type);
          args.error = HA_AERROR_INVALID_PARAMETER;
          break;
        }

        enum ha_ArgumentError poperr = populatecurrentargument(
            &args.arguments[aoi], arg, ac, av, &curr_argtemp);

        if (poperr != HA_AERROR_SUCCESS) {
          args.error = poperr;
          goto main_return;
        }

        args.argument_num++;
        found_argument = true;

        break;
      }

      enum ha_ArgumentError poperr = populatecurrentargument(
          &args.arguments[aoi], arg, ac, av, &curr_argtemp);

      if (poperr == HA_AERROR_SUCCESS) {
        args.error = poperr;
        break;
      }

      args.argument_num++;
      found_argument = true;

      break;
      /* --------------------- */

      /* Check for long names */
    long_name_comparison:
      if (!curr_argtemp.long_argname)
        continue;

      if (curr_arglen < lname_len)
        continue;

      if (curr_arglen > lname_len && av[arg][lname_len] != '=')
        continue;

      if (strncmp(av[arg], curr_argtemp.long_argname, lname_len))
        continue;

      if (curr_arglen == lname_len) {
        if (curr_argtemp.param && curr_argtemp.param->required) {
          __ha_err_print_missing_parameter_error(
              ac, av, arg, curr_argtemp.param->param_type);
          args.error = HA_AERROR_MISSING_PARAMETER;
          break;
        }

        enum ha_ArgumentError poperr = populatecurrentargument(
            &args.arguments[aoi], arg, ac, av, &curr_argtemp);

        if (poperr != HA_AERROR_SUCCESS) {
          args.error = poperr;
          break;
        }

        args.argument_num++;
        found_argument = true;

        break;
      }

      poperr = populatecurrentargument(&args.arguments[aoi], arg, ac, av,
                                       &curr_argtemp);

      if (poperr != HA_AERROR_SUCCESS) {
        args.error = poperr;
        break;
      }

      args.argument_num++;
      found_argument = true;

      break;
      /* -------------------- */
    }

    if (!found_argument) {
      if (args.error == HA_AERROR_SUCCESS) {
        __ha_err_print_unknown_argument_error(ac, av, arg);

        args.error = HA_AERROR_UNKNOWN_ARGUMENT;
      }

      /* The `arguments` buffer needs to be freed by the callee */
      goto main_return;
    }
  }

main_return:
  return args;
}
/* ---------------------------- */

/**
 * Gets arguments specified in the templates.
 * argtemplates: user defined templates to get from argv
 * ac: alias for argc
 * av: alias for argv
 *
 */
struct ha_Arguments
ha_GetArguments(const struct ha_ArgumentTemplates *argtemplates, const int ac,
                const char **av) {
  /* Check for intialized loggers */
  if (!__ha_common_ErrLogger || !__ha_common_WarnLogger) {
    puterr("The developer of this application forgot to initialize the loggers "
           "for havarg!");
    puterr("Continuing with default loggers!");
    ha_InitDefaultLoggers();
  }

  struct ha_Arguments args = {0};
  args.error = HA_AERROR_SUCCESS;

  int minimum_required_argument_number = 0;

  for (int i = 0; i < argtemplates->argtemplate_num; i++) {
    /**
     * We can just treat bools as numbers.
     * TODO: May need to be fixed in C23, because `bool` will become a keyword
     */
    minimum_required_argument_number += argtemplates->argtemplates[i].required;
  }

  /* argv[0] is not an argument */
  if (ac - 1 < minimum_required_argument_number) {
    __ha_err_print_missing_arguments_error(minimum_required_argument_number, ac,
                                           av);

    args.error = HA_AERROR_MISSING_ARGUMENT;
    goto main_return;
  }

  struct ha_Arguments valid_args = getValidArguments(argtemplates, ac, av);

  return valid_args;

main_return:
  return args;
}
/* ----------------------------------------------------- */

/**
 * Frees the argument struct.
 * args: `ha_Arguments` struct returned from `ha_GetArguments`
 *
 */
void ha_FreeArguments(const struct ha_Arguments *args) {
  if (!args->arguments)
    return;

  for (int i = 0; i < args->argument_num; i++) {
    printf("Argument #%i\n", i);
    if (!args->arguments[i].param)
      continue;

    free(args->arguments[i].param);
    args->arguments[i].param = NULL;
  }

  free(args->arguments);

  return;
}
/* ----------------------------------------------------------- */
