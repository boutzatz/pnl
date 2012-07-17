
/************************************************************************/
/* Copyright J�r�me Lelong <jerome.lelong@gmail.com>                    */
/*                                                                      */
/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU Lesser General Public License as       */
/* published by the Free Software Foundation, either version 3 of the   */
/* License, or (at your option) any later version.                      */
/*                                                                      */
/* This program is distributed in the hope that it will be useful, but  */
/* WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    */
/* Lesser General Public License for more details.                      */
/*                                                                      */
/* You should have received a copy of the GNU Lesser General Public     */
/* License along with this program.  If not, see                        */
/* <http://www.gnu.org/licenses/>.                                      */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_mathtools.h"



/**
 * Matrix exponential B = exp( A)
 *
 * @param A a matrix
 * @param B contains exp(A) on return
 *
 * This algorithms has been written in C by J�r�me Lelong
 * following the Fortran implementation given in EXPOKIT
 * Roger B. Sidje Department of Mathematics, University of Queensland 
 * Brisbane, QLD-4072, Australia, (c) 1996-1999 All Rights Reserved
 * The work resulting from EXPOKIT has been published in ACM-Transactions 
 * on Mathematical Software, 24(1):130-156, 1998.
 *
 *  The bibtex record of the citation:
 *
 * \verbatim
 * ARTICLE{EXPOKIT,
 *        AUTHOR  = {Sidje, R. B.},
 *        TITLE   = {{\sc Expokit.} {S}oftware {P}ackage for
 *                 {C}omputing {M}atrix {E}xponentials},
 *        JOURNAL = {ACM Trans. Math. Softw.},
 *        VOLUME  = {24},
 *        NUMBER  = {1},
 *        PAGES   = {130-156}
 *        YEAR    = {1998}
 * }
 * \endverbatim
 */
int pnl_mat_exp (PnlMat *B, const PnlMat *A)
{
  int i,j,k,ih2,ip,iq,iused,ifree,iodd,icoef,ideg,iput,iget,lwork, ns;
  double *work;
  PnlMat Mwork, Mwork1, Mwork2;
  double hnorm,scale,scale2,cp,cq;
  
  CheckIsSquare (A);
  
  /* Mwork is used as a container for pnl_mat_xxx routines */
  pnl_mat_resize (B, A->m, A->n);
  
  icoef = 0;
  ideg = 6;
  ih2 = icoef + (ideg+1);
  ip  = ih2 + A->mn;
  iq  = ip + A->mn;
  ifree = iq + A->mn;
  lwork = 4*A->m*A->m + ideg + 1;
  if ( (work = malloc (lwork * sizeof(double))) == NULL ) abort();

  /*  scaling: seek ns such that ||t*H/2^ns|| < 1/2;  */
  /*  and set scale = t/2^ns ... */
  for (i=0; i<A->m; i++) work[i] = 0.;
  for (j=0; j<A->m; j++)
    {
      for (i=0; i<A->m; i++)
        work[i] += fabs( PNL_MGET(A,i,j) );      
    }

  hnorm = 0.;
  for (i=0; i<A->m; i++)
    {
      hnorm = MAX( hnorm, work[i] );
    }

  if (hnorm == 0.)
    /* matrix is full of zeros */
    {
      pnl_mat_set_id (B);
      free (work); work = NULL;
      return FAIL;
    }
  ns = MAX( 0, (int)(log(hnorm)/log(2.)) + 2 );
  scale = 1. / pnl_pow_i (2., ns);
  scale2 = scale*scale;

  /*  compute Pade coefficients ... */
  i = ideg+1;
  j = 2*ideg+1;
  work[icoef] = 1.0;
  for (k=1; k<ideg+1; k++)
    {
      work[icoef+k] = (work[icoef+k-1]*( i-k )) / ( k*(j-k) );
    }

  Mwork = pnl_mat_wrap_array (&(work[ih2]), A->m, A->n);
  pnl_mat_dgemm ('N', 'N', scale2, A, A, 0., &Mwork);  /* H2 = scale2*H*H */
  
  /* initialize p (numerator) and q (denominator) */
  cp = work[icoef+ideg-1];
  cq = work[icoef+ideg];
  for (j=0; j<A->m; j++)
    {
      for (i=0; i<A->m; i++)
        {
          work[ip + j*A->m + i] = 0.;
          work[iq + j*A->m + i] = 0.;
        }
      work[ip + j*(A->m+1)] = cp; /* sets the diagonal */
      work[iq + j*(A->m+1)] = cq; /* sets the diagonal */
    }

  /* Apply Horner rule */
  iodd = 1;
  k = ideg - 1;
  while (k > 0)
    {
      iused = iodd*iq + (1-iodd)*ip;
      Mwork = pnl_mat_wrap_array (&(work[ifree]), A->m, A->m);
      Mwork1 = pnl_mat_wrap_array (&(work[iused]), A->m, A->m);
      Mwork2 = pnl_mat_wrap_array (&(work[ih2]), A->m, A->m);
      pnl_mat_dgemm('N', 'N', 1., &Mwork1, &Mwork2, 0., &Mwork);
      
      for (j=0; j<A->m; j++)
        {
          /* add work[icoef+k-1]; to the diagonal */
          work[ifree+j*(A->m+1)] += work[icoef+k-1];
        }
      ip = (1-iodd)*ifree + iodd*ip;
      iq = iodd*ifree + (1-iodd)*iq;
      ifree = iused;
      iodd = 1-iodd;
      k--;
    }
 
  /* Obtain (+-)(I + 2 * (p/q))  */
  Mwork = pnl_mat_wrap_array (&(work[ifree]), A->m, A->m);
  Mwork1 = pnl_mat_wrap_array (&(work[ip]), A->m, A->m);
  pnl_mat_dgemm ('N', 'N', scale, &Mwork1, A, 0., &Mwork);
  ip = ifree;

  Mwork1 = pnl_mat_wrap_array (&(work[ip]), A->m, A->m);
  Mwork2 = pnl_mat_wrap_array (&(work[iq]), A->m, A->m);

  pnl_mat_axpy (-1., &Mwork1, &Mwork2 );
  pnl_mat_syslin_mat (&Mwork2, &Mwork1);

  pnl_mat_mult_double (&Mwork1, 2.);
  for (j=0; j<A->m; j++) work[ip+j*(A->m+1)] += 1.;
  iput = ip;

  if (ns == 0 && iodd == 1)
    {
      pnl_mat_mult_double (&Mwork1, -1.);
    }
  else
    {
      /* squaring : exp(t*H) = (exp(t*H))^(2^ns) */
      iodd = 1;
      for (k=0; k<ns; k++)
        {
          iget = iodd*ip + (1-iodd)*iq;
          iput = (1-iodd)*ip + iodd*iq;
          Mwork = pnl_mat_wrap_array (&(work[iget]), A->m, A->m);
          Mwork1 = pnl_mat_wrap_array (&(work[iput]), A->m, A->m);
          pnl_mat_dgemm ('N', 'N', 1., &Mwork, &Mwork, 0., &Mwork1);
          iodd = 1-iodd;
        }
    }

  /* the solution is located at work[iput] */
  memcpy (B->array, &(work[iput]), A->mn * sizeof(double));
  
  free (work); work = NULL;
  return OK;
}






