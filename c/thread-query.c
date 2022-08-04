/*
 *
 * Copyright (c) 2016 - 2022
 *   Stony Brook University
 * Copyright (c) 2015 - 2018
 *   Los Alamos National Security, LLC.
 * Copyright (c) 2011 - 2015
 *   University of Houston System and UT-Battelle, LLC.
 * Copyright (c) 2009 - 2015
 *   Silicon Graphics International Corp.  SHMEM is copyrighted
 *   by Silicon Graphics International Corp. (SGI) The OpenSHMEM API
 *   (shmem) is released by Open Source Software Solutions, Inc., under an
 *   agreement with Silicon Graphics International Corp. (SGI).
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * o Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * o Neither the name of the University of Houston System,
 *   UT-Battelle, LLC. nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Allow query of thread support.  Defaults to MULTIPLE, override on
 * command-line (case-insensitive, minimal-match).  Tells you the
 * level actually supported.  Only needs 1 PE.
 */

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include <shmem.h>

#define GLUE(_thr) { SHMEM_THREAD_##_thr,  #_thr }

static const int terminator = SHMEM_THREAD_SINGLE - 1;

static struct thread_encdec {
    int level;
    const char *name;
} threads_table[] = {
    GLUE(SINGLE),
    GLUE(FUNNELED),
    GLUE(SERIALIZED),
    GLUE(MULTIPLE),
    { terminator, NULL }
};

static const char *
decode(int tl)
{
    struct thread_encdec *tp = threads_table;

    while (tp->level != terminator) {
        if (tp->level == tl) {
            return tp->name;
            /* NOT REACHED */
        }
        ++tp;
    }
    return "unknown";
}

static int
encode(const char *tn)
{
    const int len = strlen(tn);
    struct thread_encdec *tp = threads_table;

    while (tp->level != terminator) {
        if (strncasecmp(tp->name, tn, len) == 0) {
            return tp->level;
            /* NOT REACHED */
        }
        ++tp;
    }
    return terminator;
}

int
main(int argc, char *argv[])
{
    struct utsname u;
    char *reqname;
    int req, prv;

    uname(&u);

    if (argc > 1) {
        reqname = argv[1];
    }
    else {
        reqname = "MULTIPLE";
    }

    req = encode(reqname);

    shmem_init_thread(req, &prv);

    printf("%s: %d: request %s (%d), got %s (%d)\n",
           u.nodename,
           shmem_my_pe(),
           decode(req), req,
           decode(prv), prv);

    shmem_finalize();

    return 0;
}
