#pragma once

void log_init(int);

void log_warn(const char *, ...);

void log_warnx(const char *, ...);

void log_info(const char *, ...);

void log_debug(const char *, ...);

void fatal(const char *);

