//
// Created by jordan on 2022-03-19.
//

#ifndef CPT_CLI_DC_APP_H
#define CPT_CLI_DC_APP_H

#include "cpt_client.h"
#include "tcp_client.h"

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

struct dc_application_settings *create_settings(const struct dc_posix_env * env, struct dc_error * err);

int destroy_settings(const struct dc_posix_env * env, struct dc_error * err,
                     struct dc_application_settings ** psettings);

int run(const struct dc_posix_env * env, struct dc_error *err, struct dc_application_settings * settings);

void error_reporter(const struct dc_error * err);

void trace_reporter(const struct dc_posix_env *env, const char *file_name,
                    const char *function_name, size_t line_number);

int run_client_cli(int argc, char *argv[]);

#endif //CPT_CLI_DC_APP_H
