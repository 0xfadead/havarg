#ifndef __ERR_METHODS_H__
#define __ERR_METHODS_H__

#include "havarg.h"
extern void (*ha_ErrorLogger)(const char *, ...);
extern void (*ha_WarningLogger)(const char *, ...);

void __ha_err_print_missing_arguments_error(unsigned int min_args, int ac,
                                            const char **av);

void __ha_err_print_unknown_argument_error(int ac, const char **av,
                                           int wrong_argument);

void __ha_err_print_missing_parameter_error(int ac, const char **av,
                                            int wrong_argument,
                                            enum ha_ParamType ptype);

void __ha_err_print_invalid_parameter_error(int ac, const char **av,
                                            int wrong_argument);

#endif
