/*
 *
 * Copyright (c) 2011 - 2015
 *   University of Houston System and UT-Battelle, LLC.
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
 * o Neither the name of the University of Houston System, UT-Battelle, LLC
 *    nor the names of its contributors may be used to
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

/* A simple DAXPY like sample kernel with computation and communication.
 * Adopted/ported from source url: http://parallel-for.sourceforge.net/shmem-proc-cpu-scalar.html
 */
#include <shmem.h>
#include <stdio.h>
#include <sys/time.h>

// global shmem_accesible
double maxtime;
double t, tv[2];

double
gettime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000000 + tv.tv_usec);
}

double
dt(double *tv1, double *tv2)
{
    return (*tv1 - *tv2);
}

int
main(int argc, char *argv[])
{
    int n = 16;
    int i;
    static float pWork[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
    static long pSync[SHMEM_REDUCE_SYNC_SIZE];
    static long pSync1[SHMEM_REDUCE_SYNC_SIZE];
    static double dpWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
    static float el, es;
    int my_pe, n_pes;

    for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i += 1) {
        pSync[i] = SHMEM_SYNC_VALUE;
        pSync1[i] = SHMEM_SYNC_VALUE;
    }

    tv[0] = gettime();

    shmem_init();
    n_pes = shmem_n_pes();
    my_pe = shmem_my_pe();
    int nn = (n - 1) / n_pes;
    int n_local0 = 1 + my_pe * nn;
    int n_local1 = 1 + (my_pe + 1) * nn;
    // allocate only local part + ghost zone of the arrays x,y
    float *x, *y;
    x = (float *) shmem_malloc((n_local1 - n_local0 + 2) * sizeof(float));
    y = (float *) shmem_malloc((n_local1 - n_local0 + 2) * sizeof(float));
    x -= (n_local0 - 1);
    y -= (n_local0 - 1);
    shmem_barrier_all();

    // ... // fill x, y
    for (i = n_local0; i < n_local1; ++i) {
        x[i] = 5;
        y[i] = 5;
    }

    // fill ghost zone
    if (my_pe > 0)
        shmem_float_put(&y[n_local1], &y[n_local0], 1, my_pe - 1);
    if (my_pe < n_pes - 1)
        shmem_float_put(&y[n_local0 - 1], &y[n_local1 - 1], 1, my_pe + 1);
    shmem_barrier_all();

    // do computation
    float e = 0;
    for (i = n_local0; i < n_local1; ++i) {
        x[i] += (y[i + 1] + y[i - 1]) * .5;
        e += y[i] * y[i];
    }

    el = e;
    shmem_float_sum_to_all(&es, &el, 1, 0, 0, n_pes, pWork, pSync);

    e = es;

    // ... // output x, e
#if DEBUG
    for (i = n_local0; i < n_local1; ++i) {
        printf("x%d[%d]=%f \n", my_pe, i, x[i]);
    }
    printf("\n");
#endif

    x += (n_local0 - 1);        // x=x,x=x+3
    y += (n_local0 - 1);        // y=y,y=y+3
    shmem_free(x);
    shmem_free(y);

    tv[1] = gettime();
    t = dt(&tv[1], &tv[0]);
    shmem_double_max_to_all(&maxtime, &t, 1, 0, 0, n_pes, dpWrk, pSync1);

    if (my_pe == 1) {
        printf("Execution time in seconds=%f\n", maxtime / 1000000.0);
        printf("Execution time in micro seconds=%f\n", maxtime / 1000.0);
    }

    shmem_finalize();

    return 0;
}
