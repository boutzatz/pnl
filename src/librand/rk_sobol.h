/* Random kit 1.4 */

/*
 * Copyright (c) 2004-2005, Jean-Sebastien Roy (js@jeannot.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* @(#) $Jeannot: rk_sobol.h,v 1.3 2005/12/28 23:20:10 js Exp $ */
/*
 * Function which are exported by this header are prefixed by pnl_ to avoid name
 * collisions 
 */

/*
 * Typical use:
 * 
 * int dimension = 2;
 * rk_sobol_state s;
 * rk_sobol_error rc;
 * double x[dimension], y[dimension];
 * 
 * /\* Init *\/
 * if (rc = pnl_rk_sobol_init(dimension, &s, NULL, NULL, NULL))
 * {
 *   fprintf(stderr, "%s\n", rk_sobol_strerror[rc]);
 *   abort();
 * }
 *   
 * /\* Draw uniform quasirandom doubles *\/
 * if (rc = pnl_rk_sobol_double(&s, x))
 * {
 *   fprintf(stderr, "%s\n", rk_sobol_strerror[rc]);
 *   abort();
 * }
 *   
 * /\* Draw gaussian quasirandom doubles *\/
 * if (rc = rk_sobol_gauss(&s, y))
 * {
 *   fprintf(stderr, "%s\n", rk_sobol_strerror[rc]);
 *   abort();
 * }
 * 
 * /\* Free allocated memory *\/
 * pnl_rk_sobol_free(&s);
 */


#ifndef _RK_SOBOL_
#define _RK_SOBOL_

#include "pnl/pnl_random.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  RK_SOBOL_OK      = 0, /* No error */
  RK_SOBOL_EINVAL  = 1, /* Invalid dimension (<= 0 or too large) */
  RK_SOBOL_EXHAUST = 2, /* Too many number generated */
  RK_SOBOL_ENOMEM  = 3, /* Not enough memory */
  RK_SOBOL_ERR_MAX = 4
} rk_sobol_error;

/* error strings */
extern char *rk_sobol_strerror[];

typedef struct
{
  int dimension;
  unsigned long *direction;
  unsigned long *numerator;
  unsigned long count;
  unsigned long gcount;
} rk_sobol_state;


/* Sobol directions initializations (zero terminated lists) */

/*
* Sobol/Levitan coefficients of the free direction integers as given
* by Bratley, P., Fox, B.L. (1988)
* Defined up to dimension 40.
*/
extern const unsigned long pnl_rk_sobol_SLdirections[];

/*
* Lemieux coefficients of the free direction integers as given
* in QuantLib by Christiane Lemieux, private communication, September 2004
* Defined up to dimension 360.
*/
extern const unsigned long pnl_rk_sobol_Ldirections[];

/*
* Peter J�ckel coefficients of the free direction integers as given
* in "Monte Carlo Methods in Finance", by Peter J�ckel, section 8.3
* Defined up to dimension 32.
*/
extern const unsigned long pnl_rk_sobol_Jdirections[];

/*
* Initialize a sobol quasirandom number generator. 
* 1 <= dimension <= the number of primitive polylonimals of degree < LONG_BIT
* If directions == NULL (or more directions than provided are required),
* the directions are picked at random using rs_dir.
* If rs_dir == NULL, it is initialized using pnl_rk_randomseed.
* polynomials is a zero terminated list of primitive polynomials to use if
* it is != NULL to speed up initialization for dimension > 1024.
*/
extern rk_sobol_error pnl_rk_sobol_init(int dimension, rk_sobol_state *s, 
          mt_state *rs_dir, const unsigned long *directions,
          const unsigned long *polynomials);

extern void pnl_rk_sobol_reinit(rk_sobol_state *s);
extern rk_sobol_error pnl_rk_sobol_copy(rk_sobol_state *copy, rk_sobol_state *orig);
extern void pnl_rk_sobol_free(rk_sobol_state *s);
extern rk_sobol_error pnl_rk_sobol_double(rk_sobol_state *s, double *x);

#ifdef __cplusplus
}
#endif

#endif /* _RK_SOBOL_ */
