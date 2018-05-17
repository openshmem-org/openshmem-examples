/*
 *
 * Copyright (c) 2016 - 2018
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
 * expected output on every PE:
 *
 * On PE<pe-id>:
 * Major version = <..>(SUCCESS)
 * Minor Version = <..>(SUCCESS)
 * Name="<..>"(SUCCESS)
 *
 */

#include <stdio.h>
#include <shmem.h>
#include <string.h>

#define SUCCESS "SUCCESS"
#define FAIL "FAIL"
#define UNDEF "UNDEFINED"

int
main()
{
    char *major_status, *minor_status, *name_status, name[SHMEM_MAX_NAME_LEN];
    int major_ver, minor_ver;
    int me;

    shmem_init();
    me = shmem_my_pe();

    shmem_info_get_version(&major_ver, &minor_ver);
    shmem_info_get_name(name);

    if (major_ver == SHMEM_MAJOR_VERSION) {
        major_status = SUCCESS;
    }
    else {
        major_status = FAIL;
    }

    if (minor_ver == SHMEM_MINOR_VERSION) {
        minor_status = SUCCESS;
    }
    else {
        minor_status = FAIL;
    }

    if (strlen(name) < SHMEM_MAX_NAME_LEN) {
        name_status = SUCCESS;
    }
    else {
        name_status = UNDEF;
    }

    printf
        ("On PE%d:\n Major version = %d(%s)\n Minor Version = %d(%s)\n Name=\"%s\"(%s)\n",
         me, major_ver, major_status, minor_ver, minor_status, name,
         name_status);

    shmem_finalize();

    return 0;
}
