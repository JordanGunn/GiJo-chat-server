//
// Created by jordan on 2022-04-05.
//

#include "application_framework.h"

int run_client_cli(int argc, char *argv[])
{
    dc_error_reporter reporter;
    dc_posix_tracer tracer;
    struct dc_posix_env env;
    struct dc_error err;
    struct dc_application_info *info;
    int ret_val;

    reporter = error_reporter;
    tracer = NULL;
    dc_error_init(&err, reporter);
    dc_posix_env_init(&env, tracer);
    info = dc_application_info_create(&env, &err, "Settings Application");
    ret_val = dc_application_run(&env, &err, info, create_settings, destroy_settings, run, dc_default_create_lifecycle, dc_default_destroy_lifecycle, NULL, argc, argv);
    dc_application_info_destroy(&env, &info);
    dc_error_reset(&err);

    return ret_val;
}


struct dc_application_settings * create_settings(const struct dc_posix_env *env, struct dc_error *err)
{
    struct application_settings *settings;

    DC_TRACE(env);
    settings = dc_malloc(env, err, sizeof(struct application_settings));

    if(settings == NULL)
    {
        return NULL;
    }

    settings->opts.parent.config_path = dc_setting_path_create(env, err);

    settings->host           = dc_setting_string_create(env, err);
    settings->port           = dc_setting_string_create(env, err);
    settings->login          = dc_setting_string_create(env, err);

    struct options opts[] = {
            {(struct dc_setting *)settings->opts.parent.config_path,
                    dc_options_set_path,
                    "config",
                    required_argument,
                    'c',
                    "CONFIG",
                    dc_string_from_string,
                    NULL,
                    dc_string_from_config,
                    NULL},
            {(struct dc_setting *)settings->login,
                    dc_options_set_string,
                    "LOGIN",
                    required_argument,
                    'L',
                    "LOGIN",
                    dc_string_from_string,
                    "LOGIN",
                    dc_string_from_config,
                    strdup("anon")},
            {(struct dc_setting *)settings->host,
                    dc_options_set_string,
                    "host",
                    required_argument,
                    'h',
                    "HOST",
                    dc_string_from_string,
                    "host",
                    dc_string_from_config,
                    "127.0.0.1"},
            {(struct dc_setting *)settings->port,
                    dc_options_set_string,
                    "port",
                    required_argument,
                    'p',
                    "PORT",
                    dc_string_from_string,
                    "port",
                    dc_string_from_config,
                    "8080"},
    };

    // note the trick here - we use calloc and add 1 to ensure the last line is all 0/NULL
    settings->opts.opts_count = (sizeof(opts) / sizeof(struct options)) + 1;
    settings->opts.opts_size = sizeof(struct options);
    settings->opts.opts = dc_calloc(env, err, settings->opts.opts_count, settings->opts.opts_size);
    dc_memcpy(env, settings->opts.opts, opts, sizeof(opts));
    settings->opts.flags = "L:h:p:";
    settings->opts.env_prefix = "CPT_CHAT_CLIENT_";

    return (struct dc_application_settings *)settings;
}


int destroy_settings(const struct dc_posix_env *env, __attribute__((unused)) struct dc_error *err,
                     struct dc_application_settings **psettings)
{
    struct application_settings *app_settings;

    DC_TRACE(env);
    app_settings = (struct application_settings *)*psettings;

    dc_setting_string_destroy(env, &app_settings->host );
    dc_setting_string_destroy(env, &app_settings->port );
    dc_setting_string_destroy(env, &app_settings->login);

    dc_free(env, app_settings->opts.opts, app_settings->opts.opts_count);
    dc_free(env, *psettings, sizeof(struct application_settings));

    if(env->null_free)
    {
        *psettings = NULL;
    }

    return 0;
}

void error_reporter(const struct dc_error *err)
{
    fprintf(stderr, "ERROR: %s : %s : @ %zu : %d\n", err->file_name, err->function_name, err->line_number, 0);
    fprintf(stderr, "ERROR: %s\n", err->message);
}

