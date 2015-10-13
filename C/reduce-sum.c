/*
 *
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
 * reduce by SUM() [1,2,3,4] across 4 PEs
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <shmem.h>

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

/*
 * reduce 1 element across the PEs
 */
static const int nred = 1;

/*
 * symmetric source and destination
 */
int src;
int dst;

int
main ()
{
    int i;
    long *pSync;
    int *pWrk;
    int pWrk_size;

    shmem_init ();

    pWrk_size = MAX (nred / 2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE);
    pWrk = (int *) shmem_malloc (pWrk_size * sizeof (*pWrk));
    assert (pWrk != NULL);

    pSync = (long *) shmem_malloc (SHMEM_REDUCE_SYNC_SIZE * sizeof (*pSync));
    assert (pSync != NULL);

    for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i += 1) {
        pSync[i] = SHMEM_SYNC_VALUE;
    }

    src = shmem_my_pe () + 1;
    shmem_barrier_all ();

    shmem_int_sum_to_all (&dst, &src, nred, 0, 0, 4, pWrk, pSync);

    printf ("%d/%d   dst =", shmem_my_pe (), shmem_n_pes ());
    printf (" %d", dst);
    printf ("\n");

    shmem_barrier_all ();
    shmem_free (pSync);
    shmem_free (pWrk);

    shmem_finalize ();

    return 0;
}
