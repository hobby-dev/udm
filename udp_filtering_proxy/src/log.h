#pragma once

void log_init(int);

void log_warn(const char *, ...);

#define LLOG_WARN(x, ...) log_warn("%s: " x, __FUNCTION__, ##__VA_ARGS__)

void log_warnx(const char *, ...);

#define LLOG_WARNX(x, ...) log_warnx("%s: " x,  __FUNCTION__, ##__VA_ARGS__)

void log_info(const char *, ...);

#define LLOG_INFO(x, ...) log_info("%s: " x, __FUNCTION__, ##__VA_ARGS__)

void log_debug(const char *, ...);

#define LLOG_DEBUG(x, ...) log_debug("%s: " x, __FUNCTION__, ##__VA_ARGS__)

void fatal(const char *);