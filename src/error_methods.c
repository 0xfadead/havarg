#include "error_methods.h"
#include "havarg.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* TODO: Make variadic arguments ANSI C complient */
#define err(s, ...) (__ha_common_ErrLogger(s, ##__VA_ARGS__))
#define warn(s, ...) (__ha_common_WarnLogger(s, ##__VA_ARGS__))

#define puterr(s) (__ha_common_puterr(s))
#define printerr(fmt, ...) (__ha_common_printerr(fmt, ##__VA_ARGS__))

void (*ha_ErrorLogger)(const char *, ...) = NULL;
void (*ha_WarningLogger)(const char *, ...) = NULL;

void __ha_err_print_missing_arguments_error(unsigned int min_args, int ac,
                                            const char **av) {
  err("%s: Expected a minimum of %i arguments, but got %i!\n",
      ha_errormsg[HA_AERROR_MISSING_ARGUMENT], min_args,
      (ac - 1 < 0 ? 0 : ac - 1));

  assert(ac > 0 && "from havarg: Internal fault in function calling!");

  unsigned long combined_arg_length = 0;

  err("%s ", av[0]);
  combined_arg_length += strlen(av[0]) + 1;
  for (int i = 1; i < ac; i++) {
    printerr("%s ", av[i]);
    combined_arg_length += strlen(av[i]) + 1;
  }
  puterr("");

  err("%s^\n", fmt_padspace(combined_arg_length));

  unsigned long aerror_missing_arguments_strlen =
      strlen(ha_errormsg[HA_AERROR_MISSING_ARGUMENT]);
  if (combined_arg_length > aerror_missing_arguments_strlen >> 1)
    combined_arg_length -= aerror_missing_arguments_strlen >> 1;
  else
    combined_arg_length = 0;
  err("%s%s\n", fmt_padspace(combined_arg_length),
      ha_errormsg[HA_AERROR_MISSING_ARGUMENT]);
}

void __ha_err_print_unknown_argument_error(int ac, const char **av,
                                           int wrong_argument) {

  assert(ac > 0 && "from havarg: Internal fault in function calling!");
  assert(wrong_argument < ac &&
         "from havarg: Internal fault in function calling!");
  assert(wrong_argument >= 0 &&
         "from havarg: Internal fault in function calling!");

  err("Unknown argument `%s`!\n", av[wrong_argument]);

  unsigned long combined_arg_length = 0;
  err("%s ", av[0]);
  combined_arg_length = strlen(av[0]) + 1;
  for (int i = 1; i < wrong_argument; i++) {
    printerr("%s ", av[i]);
    combined_arg_length += strlen(av[i]) + 1;
  }
  printerr("%s ", av[wrong_argument]);
  for (int i = wrong_argument + 1; i < ac; i++)
    printerr("%s ", av[i]);
  puterr("");

  err("%s", fmt_padspace(combined_arg_length));

  unsigned long wrong_argument_strlen = strlen(av[wrong_argument]);
  for (unsigned long i = 0; i < wrong_argument_strlen; i++) {
    printerr("~");
  }
  puterr("");

  combined_arg_length += wrong_argument_strlen >> 1;
  unsigned long errmsg_strlen_div_2 =
      strlen(ha_errormsg[HA_AERROR_UNKNOWN_ARGUMENT]) >> 1;
  if (errmsg_strlen_div_2 < combined_arg_length)
    combined_arg_length -= errmsg_strlen_div_2;
  else
    combined_arg_length = 0;
  err("%s%s\n", fmt_padspace(combined_arg_length),
      ha_errormsg[HA_AERROR_UNKNOWN_ARGUMENT]);

  return;
}

void __ha_err_print_missing_parameter_error(int ac, const char **av,
                                            int wrong_argument,
                                            enum ha_ParamType ptype) {
  assert(ac > 0 && "from havarg: Internal fault in function calling!");
  assert(wrong_argument < ac &&
         "from havarg: Internal fault in function calling!");
  assert(wrong_argument >= 0 &&
         "from havarg: Internal fault in function calling!");

  err("Missing parameter for argument `%s`!\n", av[wrong_argument]);

  unsigned long combined_arg_length = strlen(av[0]) + 1;

  err("%s ", av[0]);

  for (int i = 1; i < wrong_argument; i++) {
    printerr("%s ", av[i]);
    combined_arg_length += strlen(av[i]) + 1;
  }

  char value_string[32];
  switch (ptype) {
  case HA_PARAMTYPE_STRING:
    strcpy(value_string, "=<string>");
    break;
  case HA_PARAMTYPE_LIST:
    strcpy(value_string, "=<list>");
    break;
  case HA_PARAMTYPE_NUMBER:
    strcpy(value_string, "=<number>");
    break;
  case HA_PARAMTYPE_NONE:
    return;
  }

  printerr("%s%s ", av[wrong_argument], value_string);
  combined_arg_length += strlen(av[wrong_argument]);

  for (int i = wrong_argument + 1; i < ac; i++) {
    printerr("%s ", av[i]);
  }
  puterr("");

  err("%s", fmt_padspace(combined_arg_length));

  const unsigned long value_string_len = strlen(value_string);

  for (unsigned long i = 0; i < value_string_len; i++)
    printerr("+");
  puterr("");

  combined_arg_length += value_string_len >> 1;
  const unsigned long error_msglen_div_2 =
      strlen(ha_errormsg[HA_AERROR_MISSING_PARAMETER]) >> 1;
  if (combined_arg_length > error_msglen_div_2)
    combined_arg_length -= error_msglen_div_2;
  else
    combined_arg_length = 0;
  err("%s%s\n", fmt_padspace(combined_arg_length),
      ha_errormsg[HA_AERROR_MISSING_PARAMETER]);

  return;
}

void __ha_err_print_invalid_parameter_error(int ac, const char **av,
                                            int wrong_argument) {
  err("");
  assert(ac > 0 && "from havarg: Internal fault in function calling!");
  assert(wrong_argument < ac &&
         "from havarg: Internal fault in function calling!");
  assert(wrong_argument >= 0 &&
         "from havarg: Internal fault in function calling!");

  unsigned long combined_arg_length = 0;

  err("%s ", av[0]);
  combined_arg_length += strlen(av[0]) + 1;
  for (int i = 1; i < wrong_argument; i++) {
    printerr("%s ", av[i]);
    combined_arg_length += strlen(av[i]) + 1;
  }
  err("%s ", av[wrong_argument]);
  for (int i = wrong_argument + 1; i < ac; i++)
    printerr("%s ", av[i]);
  puterr("");

  err("%s", fmt_padspace(combined_arg_length));
  unsigned long equal_pos = (unsigned long)strstr(av[wrong_argument], "=");

  unsigned long wrong_arglen = strlen(av[wrong_argument]);
  unsigned long errmsg_len_div_2 =
      strlen(ha_errormsg[HA_AERROR_INVALID_PARAMETER]) >> 1;

  if (!equal_pos) {
    assert(ac - 1 > wrong_argument &&
           "from havarg: Internal fault in function calling!");

    unsigned long param_strlen = strlen(av[wrong_arglen + 1]);

    for (unsigned long i = 0; i < param_strlen; i++)
      printerr("~");
    puterr("");

    param_strlen = param_strlen >> 1;
    combined_arg_length += param_strlen;
    if (combined_arg_length > errmsg_len_div_2)
      combined_arg_length -= errmsg_len_div_2;
    else
      combined_arg_length = 0;
    err("%s%s\n", fmt_padspace(combined_arg_length),
        av[HA_AERROR_INVALID_PARAMETER]);

    return;
  }

  equal_pos -= (unsigned long)av[wrong_argument];
  printerr("%s", fmt_padspace(equal_pos));

  wrong_arglen -= equal_pos;

  for (unsigned long i = 0; i < wrong_arglen; i++)
    printerr("~");
  puterr("");
}
