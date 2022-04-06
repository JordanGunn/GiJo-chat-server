//
// Created by jordan on 2022-04-05.
//

#ifndef CPT_APPLICATION_FRAMEWORK_H
#define CPT_APPLICATION_FRAMEWORK_H


#include <dc_application/command_line.h>
#include <dc_application/config.h>
#include <dc_application/defaults.h>
#include <dc_application/environment.h>
#include <dc_application/options.h>
#include <dc_posix/dc_stdlib.h>
#include <dc_posix/dc_string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


struct application_settings
{
    struct dc_opt_settings opts;
    struct dc_setting_string * port;
    struct dc_setting_string * host;
    struct dc_setting_string * login;
};


/**
 *  DC application framework interface function.
 */
struct dc_application_settings *create_settings(const struct dc_posix_env * env, struct dc_error * err);


/**
 *  DC application framework interface function.
 */
int destroy_settings(const struct dc_posix_env * env, struct dc_error * err,
                     struct dc_application_settings ** psettings);


/**
 *  DC application framework interface function.
 */
int run(const struct dc_posix_env * env, struct dc_error *err, struct dc_application_settings * settings);


/**
 *  DC application framework interface function.
 */
void error_reporter(const struct dc_error * err);


/**
 *  DC application framework interface function.
 */
void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                    const char *function_name, size_t line_number);


/**
 *  DC application framework interface function.
 */
int run_client_cli(int argc, char *argv[]);




#endif //CPT_APPLICATION_FRAMEWORK_H
