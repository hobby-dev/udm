/*	$OpenBSD: log.c,v 1.11 2007/12/07 17:17:00 reyk Exp $	*/

/*
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE 1

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "log.h"

int debug;

void vlog(const char *, va_list);

void
log_init(int n_debug) {
    debug = n_debug;

    tzset();
}


void
logit(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vlog(fmt, ap);
    va_end(ap);
}

void
vlog(const char *fmt, va_list ap) {
    char *nfmt;

    /* best effort in out of mem situations */
    if (asprintf(&nfmt, "%s\n", fmt) == -1) {
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
    } else {
        vfprintf(stderr, nfmt, ap);
        free(nfmt);
    }
    fflush(stderr);
}


void
log_warn(const char *emsg, ...) {
    char *nfmt;
    va_list ap;

    /* best effort to even work in out of memory situations */
    if (emsg == NULL)
        logit("%s", strerror(errno));
    else {
        va_start(ap, emsg);

        if (asprintf(&nfmt, "%s: %s", emsg, strerror(errno)) == -1) {
            /* we tried it... */
            vlog(emsg, ap);
            logit("%s", strerror(errno));
        } else {
            vlog(nfmt, ap);
            free(nfmt);
        }
        va_end(ap);
    }
}

void
log_warnx(const char *emsg, ...) {
    va_list ap;

    va_start(ap, emsg);
    vlog(emsg, ap);
    va_end(ap);
}

void
log_info(const char *emsg, ...) {
    va_list ap;

    va_start(ap, emsg);
    vlog(emsg, ap);
    va_end(ap);
}

void
log_debug(const char *emsg, ...) {
    va_list ap;

    if (debug > 0) {
        va_start(ap, emsg);
        vlog(emsg, ap);
        va_end(ap);
    }
}

void
fatal(const char *emsg) {
    if (emsg == NULL)
        logit("fatal: %s(%d)", strerror(errno), errno);
    else if (errno)
        logit("fatal: %s: %s(%d)",
              emsg, strerror(errno), errno);
    else
        logit("fatal: %s(%d)", emsg, errno);

    exit(1);
}

