/*
 * This code was originally written for matrices of doubles and has been
 * translated using a template system by
 *    David Pommier <pommier.david@gmail.com>             
 *    J�r�me Lelong <jerome.lelong@gmail.com>
 */


/************************************************************************/
/* Copyright J�r�me Lelong <jerome.lelong@gmail.com>                    */
/* Copyright C�line Labart                                              */
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
#include <ctype.h>
#include <stdarg.h>

#define PNL_MATRIX_PRIVATE
#include "pnl_mathtools.h"


/***************************
 *** PnlMat functions ***
 ***************************/
 
/**
 * creates a PnlMat
 * @param m number of rows
 * @param n number of columns
 * @return a TYPE(PnlMat) pointer
 */
TYPE(PnlMat) * FUNCTION(pnl_mat,create)(int m, int n)
{
  TYPE(PnlMat) *v;
  if ((v = malloc (sizeof (TYPE(PnlMat)))) == NULL)
    return NULL;
  v->m = m;
  v->n = n;
  v->mn = m * n;
  v->mem_size = v->mn;
  v->owner = 1;
  if (v->mn > 0)
    {
      if ((v->array = malloc (v->mn * sizeof(BASE))) == NULL)
        return NULL;
    }
  else
    v->array = (BASE*)NULL;  
  return v;  
}

/**
 * creates a TYPE(PnlMat)
 * @param m number of rows
 * @param n number of columns
 * @param x used to fill the matrix with
 * @return a TYPE(PnlMat) pointer
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,CONCAT2(create_from_,BASE))(int m, int n, BASE x)
{
  TYPE(PnlMat) *v;
  int i;
  BASE *ptr;
  
  if ((v=FUNCTION(pnl_mat,create)(m, n))==NULL)
    return NULL;

  ptr= FUNCTION(pnl_mat,lget) (v, 0, 0);
  for (i=0; i<v->mn; i++)
    {
      *ptr = x; ptr++;
    }
  return v;
}

/**
 * creates a TYPE(PnlMat)
 * @param m number of rows
 * @param n number of columns
 * @param x an array of BASE used to fill the TYPE(PnlMat). should be of length
 * <tt> mxn </tt>. No test can be done about it.
 * @return a TYPE(PnlMat) pointer
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,create_from_ptr)(int m, int n, const BASE* x)
{
  TYPE(PnlMat) *v;
  
  if ((v=FUNCTION(pnl_mat,create)(m, n))==NULL)
    return NULL;
  
  memcpy(v->array, x, v->mn*sizeof(BASE));
  return v;
}


/**
 * creates a new TYPE(PnlVect)pointer.
 *
 * @param m number of rows
 * @param n number of columns
 * @param ... is a list of values o type BASE used to fill the array. The list
 * must be of length size. The data are stored row-wise.
 * @return  a TYPE(PnlVect)pointer
 */
TYPE(PnlMat) * FUNCTION(pnl_mat,create_from_list)(int m, int n, ...)
{
  TYPE(PnlMat) * M;
  va_list ap;
  int i;

  if ((M=FUNCTION(pnl_mat,create)(m, n))==NULL)
    return NULL;
  va_start (ap, n);

  for (i=0; i<M->mn; i++)
    {
      BASE val ;
      val = va_arg (ap, BASE);
      M->array[i] = val;
    }
  va_end(ap);
  return M;
}

/**
 * creates a TYPE(PnlMat)
 * @param m number of rows
 * @param n number of columns
 * @param x an array of BASE used to fill the TYPE(PnlMat). should be of length
 * <tt> mxn </tt>. No test can be done about it.
 * @return a TYPE(PnlMat) pointer
 */
TYPE(PnlMat) FUNCTION(pnl_mat,create_wrap_array)(const BASE* x, int m, int n)
{
  TYPE(PnlMat) M;

  M.m = m;
  M.n = n;
  M.mn = m*n;
  M.mem_size = 0;
  M.owner = 0;
  M.array = (BASE *) x;
  return M;
}


/**
 * Reads a matrix from a file and creates the corresponding TYPE(PnlMat)
 * @param file  the file to be read
 * @ return a TYPE(PnlMat)
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,create_from_file )(const char * file)
{
  char car, prev = '\0', empty=1;
  TYPE(PnlMat) *M;
  int m, n, count, mn;
  ATOMIC *atomic_data;
  FILE *FIC = NULL;
  
  if ((FIC = fopen( file, "r")) == NULL )
    {
      PNL_ERROR("Cannot open file", "FUNCTION(pnl_mat,create_from_file)");
    }

  /* first pass to determine dimensions */
  m = 0; n = 1;
  while((car=fgetc(FIC))!='\n')
    {
      if (isdigit(car) || car == '-' || car == '.')
        {
          empty = 0;
          if (prev == ' ' || prev == '\t' ) ++n;
        }
      prev = car;
    }
  /*if (!empty && car =='\n' && isdigit(prev)) ++n; */
  if (!empty) ++m;
  empty = 1;
  while((car=fgetc(FIC))!= EOF)
    {
      if( car=='\n' )
        {
          if (!empty) { ++m; empty = 1;}
          else break;
        }
      else if (empty && isdigit(car)) empty=0;
    }
  if (m==0 || n==0)
    {
      PNL_ERROR ("No matrix found in input file",  "pnl_mat_create_from_file");
    }

  /* need special care when MULTIPLICITY > 1 */
  if ( MULTIPLICITY * ((double)m/MULTIPLICITY) != m)
    {
      PNL_ERROR ("Incorrect matrix format",  "pnl_mat_create_from_file");
    }
  mn = m*n;
  if ((M = FUNCTION(pnl_mat,create )(m/MULTIPLICITY,n))==NULL)
    {
      PNL_ERROR("Allocation error", "FUNCTION(pnl_mat,create_from_file)");
    }

  atomic_data = (ATOMIC *) FUNCTION(pnl_mat,lget )(M, 0, 0);

  /* second pass to read data */
  rewind( FIC );
  count = 0;
  while (fscanf(FIC,IN_FORMAT, atomic_data) > 0 && count < mn) { atomic_data++; count++;}
  fclose( FIC );
  return M;
}


/** 
 * frees a TYPE(PnlMat)
 *
 * @param v adress of a TYPE(PnlMat)*. v is set to NULL at exit.
 */
void FUNCTION(pnl_mat,free)(TYPE(PnlMat) **v)
{
  if (*v != NULL)
    {
      if ((*v)->array != NULL && (*v)->owner == 1) free((*v)->array);
      free(*v);
      *v=NULL;
    }
}

/**
 * copies a TYPE(PnlMat) 
 *
 * @param v : a constant TYPE(PnlMat) pointer
 * @return a TYPE(PnlMat) pointer initialised with v
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,copy)(const TYPE(PnlMat) *v)
{
  TYPE(PnlMat) *ret;

  if ((ret=FUNCTION(pnl_mat,create)(v->m, v->n))==NULL) return NULL;
  memcpy(ret->array, v->array, sizeof(BASE)*ret->mn);
  return ret;
}


/**
 * clones a TYPE(PnlMat)
 *
 * @param M : a constant TYPE(PnlMat) pointer
 * @param clone : needs to be already allocated. NOT TESTED
 */
void FUNCTION(pnl_mat,clone)(TYPE(PnlMat) *clone, const TYPE(PnlMat) *M)
{
#ifndef PNL_RANGE_CHECK_OFF
  if (clone->owner == 0 && clone->mn != M->mn)
    {
      PNL_ERROR ("owner == 0 and size mismatch", "pnl_mat_clone");
    }
#endif
  FUNCTION(pnl_mat,resize)(clone, M->m,M->n);
  memcpy(clone->array, M->array, sizeof(BASE)*M->mn);
}

/**
 * resizes a TYPE(PnlMat).
 *
 * If the new size is smaller than the current one, no memory is freed. If the
 * new size is larger than the current mem_size, a new pointer is
 * allocated. The old data are kept.
 *
 * @param M : a pointer to an already existing TYPE(PnlMat) 
 * @param m : new nb rows
 * @param n : new nb columns
 *
 * @return OK or FAIL. When returns OK, the matrix is changed. 
 */
int FUNCTION(pnl_mat,resize)(TYPE(PnlMat) *M, int m, int n)
{
  int mn;
  mn = m*n;
  if (M->owner == 0) return OK;
  if (mn < 0) return FAIL;
  if (mn == 0) /* free array */
    {
      M->m = M->n = M->mn = M->mem_size = 0;
      if (M->array != NULL) { free(M->array); M->array = NULL; }
      return OK;
    }

  if (M->mem_size >= mn) 
    {
      /* If the new size is smaller, we do not reduce the size of the
         allocated block. It may change, but il may allow to grow the matrix
         quicker */
      M->m=m; M->n=n; M->mn = mn;
      return OK;
    }

  /* Now, M->mem_size < mn */
  if (M->array != NULL) free (M->array);
  if ((M->array = malloc(mn * sizeof(BASE))) == NULL) return FAIL;
  M->m = m; M->n = n;
  M->mn = M->mem_size = mn;
  return OK;
}

/**
 * sets the value of self[i,j]=x
 *
 * @param self : a TYPE(PnlMat)
 * @param i : index of line
 * @param j : index of col 
 * @param x : self[i,j]=x
 */
void FUNCTION(pnl_mat,set)(TYPE(PnlMat) *self, int i, int j, BASE x)
{
#ifndef PNL_RANGE_CHECK_OFF
  if (i<0 || i>=self->m || j<0 || j>=self->n)
    {
      PNL_ERROR ("index out of range", "pnl_mat_set");
    }
#endif
  self->array[i*self->n+j]=x;
}

/**
 * gets the value of self[i,j]
 *
 * @param self : a TYPE(PnlMat)
 * @param i : index of line
 * @param j : index of col 
 * @return  self[i,j]
 */
BASE FUNCTION(pnl_mat,get)(const TYPE(PnlMat) *self, int i, int j)
{
  CheckIndexMat(self,i, j);
  return self->array[i*self->n+j];
}

/**
 * returns the address of self[i,j] for use as a lvalue.
 *
 * @param self : a TYPE(PnlMat)
 * @param i : index of line
 * @param j : index of col 
 * @return  &(self[i,j])
 */
BASE* FUNCTION(pnl_mat,lget)(TYPE(PnlMat) *self, int i, int j)
{
#ifndef PNL_RANGE_CHECK_OFF
  if (i<0 || i>=self->m || j<0 || j>=self->n)
    {
      PNL_ERROR ("index out of range", "pnl_mat_lget");
    }
#endif 
  return &(self->array[i*self->n+j]);
}

/**
 * in-place set matrix constant 
 *
 * @param lhs : left hand side matrix
 * @param x : scalar
 * @return  lhs = x
 */
void FUNCTION(pnl_mat, CONCAT2(set_,BASE)) (TYPE(PnlMat) *lhs, BASE x)
{
  int i;
  for (i=0; i<lhs->mn; i++) lhs->array[i] = x;
}

/**
 * in-place set matrix to identity
 *
 * @param lhs : left hand side matrix
 */
void FUNCTION(pnl_mat,set_id)(TYPE(PnlMat) *lhs)
{
  int i;
  BASE zero = ZERO, one = ONE;
  CheckIsSquare (lhs);
  for (i=0; i<lhs->mn; i++) lhs->array[i] = zero;
  for (i=0; i<lhs->m; i++)
    {
      PNL_MLET(lhs, i, i) = one;
    }
}

/**
 * in-place set matrix to identity
 *
 * @param M a matrix
 * @param x the value used to fill the diagonal
 * @param d the index of the diagonal (if d>0, we consider the d-th upper
 * diagonal, if d<0, we consider the (-d)-th lower diagonal)
 * 
 */
void FUNCTION(pnl_mat,set_diag)(TYPE(PnlMat) *M, BASE x, int d)
{
  int i;
  CheckIsSquare (M);
  for ( i=MAX(0, -d) ; i<M->m + MIN(0, -d) ; i++ )
    {
      PNL_MLET (M, i, i+d) = x;
    }
}


/**
 *  transposition of  matrices
 *
 * @param M : a TYPE(PnlMat) pointer.
 * @return the transpose of M
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,transpose)(const TYPE(PnlMat) *M)
{
  TYPE(PnlMat) *res;
  BASE *mptr;
  BASE *rptr;
  int i,j;
  res=FUNCTION(pnl_mat,create)(M->n,M->m);
  rptr=res->array;
  
  for (i=0;i<res->m;i++)
    {
      mptr=M->array+i;
      for (j=0;j<res->n;j++)
        {
          (*rptr)=(*mptr); rptr++;mptr+=M->n;
        }
    }
  return res;
}

/**
 * in-place transposition of square matrices
 *
 * @param M : a PnlMat pointer.
 */
void FUNCTION(pnl_mat,sq_transpose) (TYPE(PnlMat) *M)
{
  BASE a, *b;
  int i,j;
  CheckIsSquare(M);
  for (i=0;i<M->m;i++)
    {
      for (j=i+1;j<M->n;j++)
        {
          b = &(PNL_MGET (M, j, i));
          a = PNL_MGET (M, i, j);
          PNL_MLET (M, i, j) = *b;
          *b = a;
        }
    }  
}


/**
 * Prints a matrix to a file
 *
 * @param fic a file descriptor.
 * @param M a TYPE(PnlMat) pointer.
 */
void FUNCTION(pnl_mat,fprint )(FILE *fic, const TYPE(PnlMat) *M)
{
  int i, j;
  for (i=0;i<M->m;i++)
    {
      for (j=0;j<M->n;j++)
        {
          fprintf (fic,OUT_FORMAT " ",OUT_PUT_FORMAT(FUNCTION(pnl_mat,get )(M, i, j)));
        }
      fprintf (fic, "\n");
    }
}

/**
 * Prints a matrix to the standard output
 *
 * @param M a TYPE(PnlMat) pointer.
 */
void FUNCTION(pnl_mat,print )(const TYPE(PnlMat) *M) 
{ FUNCTION(pnl_mat,fprint)(stdout, M);}

/**
 * prints a TYPE(PnlMat) to a file in a format compatible with Nsp
 *
 * @param fic a file descriptor
 * @param M a(constant) TYPE(PnlMat)ptr.
 */
void FUNCTION(pnl_mat, fprint_nsp)(FILE *fic, const TYPE(PnlMat) * M)
{
  int i, j;
  fprintf(fic,"[ ");
  for (i=0; i<M->m; i++) 
    {
      for (j=0;j<M->n;j++)
        {
          fprintf(fic,OUT_FORMAT,OUT_PUT_FORMAT(FUNCTION(pnl_mat,get )(M, i, j)));
          if (j != M->n-1) fprintf (fic,", ");
        }
      if (i != M->m-1) fprintf(fic,"; \n  ");
    }
  fprintf(fic," ]; \n");
}

/**
 * prints a TYPE(PnlMat) in a format compatible with Nsp
 *
 * @param M a(constant) TYPE(PnlMat)ptr.
 */
void FUNCTION(pnl_mat, print_nsp)(const TYPE(PnlMat) * M)
{ FUNCTION(pnl_mat,fprint_nsp)(stdout, M);}


/**
 * Creates a square matrix by specifying its diagonal terms.
 *
 *@param V : a TYPE(PnlVect) 
 *@return a squared matrix whose diagonal is the vector V
 *and null elsewhere
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,create_diag)(const TYPE(PnlVect) *V)
{
  return FUNCTION(pnl_mat,create_diag_from_ptr )(V->array, V->size);
}

/**
 * @param x a double array 
 * @param d size of the square matrix to create
 * @return a squared matrix whose diagonal is the array x
 * and 0 anywhereelse
 */
TYPE(PnlMat)* FUNCTION(pnl_mat,create_diag_from_ptr)(const BASE x[], int d)
{
  TYPE(PnlMat)* lhs;
  int i;
  if((lhs=FUNCTION(pnl_mat,CONCAT2(create_from_,BASE))(d,d,ZERO))==NULL)
    return NULL;
  for(i=0;i<d;i++) { PNL_MLET(lhs, i, i) =  x[i]; }
  return lhs;
}

/**
 * replace the ith row of a matrix M by a vector V
 *
 * @param M : left hand side matrix (needs to be already allocated)
 * @param V : right hand side vector
 * @param i : integer corresponding to the row of M which
 * becomes V
 * @return matrix M which contains whose ith row is becoming V
 */
void FUNCTION(pnl_mat,set_row)(TYPE(PnlMat) *M, const TYPE(PnlVect) *V, int i)
{
#ifndef PNL_RANGE_CHECK_OFF
  if(i>M->m) {PNL_ERROR("index out of range", "pnl_mat_set_row");}
  if (M->n != V->size) {PNL_ERROR ("incompatible data", "pnl_mat_set_row");}
#endif
  memcpy (&(M->array[i*M->n]), V->array, V->size*sizeof(BASE)); 
}

/**
 * Swap two rows of a matrix
 *
 * @param M TYPE(PnlMat)
 * @param i first index
 * @param j second index
 */
void FUNCTION(pnl_mat,swap_rows )(TYPE(PnlMat) *M, int i, int j)
{
  int k, p;
  register BASE Mik, Mjk;
#ifndef PNL_RANGE_CHECK_OFF
  if (i >= M->m)
    {
      PNL_ERROR("first index is out of range", "FUNCTION(pnl_mat,swap_rows");
    }

  if (j >= M->m)
    {
      PNL_ERROR("second index is out of range", "FUNCTION(pnl_mat,swap_rows");
    }
#endif
  p = M->n % 3;
  for (k=0; k<p; k++)
    {
      Mik = FUNCTION(pnl_mat,get )(M, i, k);
      Mjk = FUNCTION(pnl_mat,get )(M,j, k);
      FUNCTION(pnl_mat,set )(M, i, k, Mjk);
      FUNCTION(pnl_mat,set )(M, j, k, Mik);
    }
  for (k=p; k<M->n; k+=3)
    {
      Mik = FUNCTION(pnl_mat,get )(M, i, k);
      Mjk = FUNCTION(pnl_mat,get )(M,j, k);
      FUNCTION(pnl_mat,set )(M, i, k, Mjk);
      FUNCTION(pnl_mat,set )(M, j, k, Mik);
  
      Mik = FUNCTION(pnl_mat,get )(M, i, k+1);
      Mjk = FUNCTION(pnl_mat,get )(M,j, k+1);
      FUNCTION(pnl_mat,set )(M, i, k+1, Mjk);
      FUNCTION(pnl_mat,set )(M, j, k+1, Mik);
  
      Mik = FUNCTION(pnl_mat,get )(M, i, k+2);
      Mjk = FUNCTION(pnl_mat,get )(M,j, k+2);
      FUNCTION(pnl_mat,set )(M, i, k+2, Mjk);
      FUNCTION(pnl_mat,set )(M, j, k+2, Mik);
    }
}

/**
 * replace the ith column of a matrix M by a vector V
 *
 * @param M : left hand side matrix (needs to be already allocated)
 * @param V : right hand side vector
 * @param j : integer corresponding to the column of M which
 * becomes V
 * @return matrix M which contains whose ith row is becoming V
 */
void FUNCTION(pnl_mat,set_col)(TYPE(PnlMat) *M, const TYPE(PnlVect) *V, int j)
{
  int i;
#ifndef PNL_RANGE_CHECK_OFF
  if(j>M->n) {printf("index out of range \n"); abort();}
#endif
  CheckMatVectIsCompatible(M,V);
  for (i=0; i<M->m; i++)
    {
      PNL_MLET(M, i, j) = PNL_GET(V, i);
    }
}


/**
 * Extract the ith row of M. Makes a copy.
 *
 * @param V a PnlVect which contains the row on exit
 * @param M a PnlMat
 * @param i the index of the row to be extracted
 */
void FUNCTION(pnl_mat,get_row)(TYPE(PnlVect) *V, const TYPE(PnlMat) *M, int i)
{
#ifndef PNL_RANGE_CHECK_OFF
  if(i>M->m) {PNL_ERROR("index out of range", "pnl_mat_get_row");}
#endif
  FUNCTION(pnl_vect,resize)(V, M->n);
  memcpy (V->array,&(M->array[i*M->n]),M->n*sizeof(BASE)); 
}

/**
 * Extract the ith row of M. Makes a copy.
 *
 * @param V a PnlVect which contains the row on exit
 * @param M a PnlMat
 * @param i the index of the row to be extracted
 */
void FUNCTION(pnl_mat,row_to_vect_inplace)(TYPE(PnlVect) *V, const TYPE(PnlMat) *M, int i)
{
  FUNCTION(pnl_mat,get_row)(V, M, i);
}

/**
 * Extract a row of a matrix and wrap it into a vector.
 * @param M a matrix
 * @param i the index of the row to be extracted
 * @return a vector (not a pointer) whose array pointer is the address of the
 * first element of the ith row of M. No copying is done.
 */
TYPE(PnlVect) FUNCTION(pnl_mat,wrap_row)(const TYPE(PnlMat) *M, int i)
{
  TYPE(PnlVect) V;
#ifndef PNL_RANGE_CHECK_OFF
  if (i > M->m) {PNL_ERROR ("index out of range", "pnl_mat_row_to_vect");}
#endif
  V.size = M->n;
  V.mem_size = 0;
  V.owner = 0;
  V.array = &(M->array[i*M->n]); 
  return V;
}

/**
 * Cast a mtrix into a PnlVect 
 * @param M a matrix
 * @return a vector (not a pointer) whose array pointer is the address of the
 * first element of the matrix M. No copying is done.
 */
TYPE(PnlVect) FUNCTION(pnl_mat,wrap_vect)(const TYPE(PnlMat) *M)
{
  TYPE(PnlVect) V;
  V.size = M->mn;
  V.mem_size = 0;
  V.owner = 0;
  V.array = M->array; 
  return V;
}

/**
 * Copies the ith column of M into V
 *
 * @param V a PnlVect resized within the function
 * @param M the matrix to be extracted
 * @param j the index of the column
 */
void FUNCTION(pnl_mat, get_col)(TYPE(PnlVect) *V, const TYPE(PnlMat) *M, int j)
{
  int i;
#ifndef PNL_RANGE_CHECK_OFF
  if (j >= M->n) {PNL_ERROR ("index out of range", "pnl_mat_get_col");}
#endif
  FUNCTION(pnl_vect, resize)(V,M->m);
  for (i=0; i<M->m; i++)
    {
      FUNCTION(pnl_vect,set) (V, i, PNL_MGET (M, i, j) );
    }
}

/**
 * in-place map function
 *
 * @param x a vector
 * @param f : the function to be applied term by term
 * @return  x = f(x)
 */
void FUNCTION(pnl_mat, map_inplace)(TYPE(PnlMat) *x, BASE(*f)(BASE))
{
  BASE  *xi;
  int i;
  xi = x->array;
  for (i=0; i<x->mn; i++)
    {
      *xi = f(*xi);
      xi++;
    }
}


/**
 * map matrix componentwise
 *
 * @param lhs : each component lhs(i) contains f(rhs(i))
 * @param rhs : right hand side vector
 * @param f   : BASE function 
 */
void FUNCTION(pnl_mat,map)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs, BASE(*f)(BASE))
{
  FUNCTION(pnl_mat,clone)(lhs, rhs);
  FUNCTION(pnl_mat,map_inplace)(lhs, f);
}


static BASE CONCAT2(__op_plus_,BASE)(BASE a, BASE b) { return PLUS(a,b); }
static BASE CONCAT2(__op_minus_,BASE)(BASE a, BASE b) { return MINUS(a,b); }
static BASE CONCAT2(__op_mult_,BASE)(BASE a, BASE b) { return MULT(a,b); }
static BASE CONCAT2(__op_div_,BASE)(BASE a, BASE b) { return DIV(a,b); }

/**
 * in-place matrix operator application
 *
 * @param lhs : left hand side vector
 * @param x : BASE arg
 * @param op : a binary operator, given as a function
 * @return  lhs = lhs op x
 */
static void FUNCTION(__pnl_mat,apply_op)(TYPE(PnlMat) *lhs, BASE x, BASE (*op)(BASE, BASE))
{
  int i;
  for (i=0; i<lhs->mn; i++)
    {
      lhs->array[i] = (*op) (lhs->array[i], x);
    }
}

/**
 * map matrix componentwise
 *
 * @param lhs : each component lhs(i) contains f(lhs(i),rhs(i))
 * @param rhs : right hand side vector
 * @param f   : real function 
 */
void FUNCTION(pnl_mat,map_mat)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs, BASE(*f)(BASE,BASE))
{
  int i;
  CheckMatMatch(lhs, rhs);
  for (i=0; i<lhs->mn; i++)
    {
      lhs->array[i] = (*f) (lhs->array[i], rhs->array[i]);
    }
}

/**
 * in-place matrix matrix addition
 *
 * @param lhs : left hand side matrix
 * @param rhs : rigth hand side matrix
 * @return  lhs = lhs+rhs
 */
void FUNCTION(pnl_mat,plus_mat)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs)
{
  FUNCTION(pnl_mat,map_mat)(lhs, rhs, CONCAT2(__op_plus_,BASE));
}

/**
 * in-place matrix matrix substraction
 *
 * @param lhs : left hand side matrix
 * @param rhs : rigth hand side matrix
 * @return  lhs = lhs+rhs
 */
void FUNCTION(pnl_mat,minus_mat)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs)
{
  FUNCTION(pnl_mat,map_mat)(lhs, rhs, CONCAT2(__op_minus_,BASE));
}
  
/**
 * in-place matrix scalar addition
 *
 * @param lhs : left hand side matrix
 * @param  x : scalar
 * @return  lhs = lhs+x
 */

void FUNCTION(pnl_mat,CONCAT2(plus_,BASE))(TYPE(PnlMat) *lhs, BASE x)
{
  FUNCTION(__pnl_mat,apply_op)(lhs, x, CONCAT2(__op_plus_,BASE));
}

/**
 * in-place matrix scalar substraction
 *
 * @param lhs : left hand side matrix
 * @param  x : scalar
 * @return  lhs = lhs-x
 */
void FUNCTION(pnl_mat,CONCAT2(minus_,BASE))(TYPE(PnlMat) *lhs, BASE x)
{
  FUNCTION(__pnl_mat,apply_op)(lhs, x, CONCAT2(__op_minus_,BASE));
}

/**
 * in-place matrix scalar multiplication
 *
 * @param lhs : left hand side matrix
 * @param x : scalar
 * @return  lhs = lhs*x
 */

void FUNCTION(pnl_mat,CONCAT2(mult_,BASE))(TYPE(PnlMat) *lhs, BASE x)
{
  FUNCTION(__pnl_mat,apply_op)(lhs, x, CONCAT2(__op_mult_,BASE));
}

/**
 * in-place matrix scalar division
 *
 * @param lhs : left hand side matrix
 * @param x : scalar
 * @return  lhs = lhs*x
 */
void FUNCTION(pnl_mat,CONCAT2(div_,BASE))(TYPE(PnlMat) *lhs, BASE x)
{
  FUNCTION(__pnl_mat,apply_op)(lhs, x, CONCAT2(__op_div_,BASE));
}

/**
 * in-place matrix matrix term by term product
 *
 * @param lhs : left hand side matrix
 * @param rhs : rigth hand side matrix
 * @return  lhs = lhs.*rhs
 */
void FUNCTION(pnl_mat,mult_mat_term)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs)
{
  FUNCTION(pnl_mat,map_mat)(lhs, rhs, CONCAT2(__op_mult_,BASE));
}

/**
 * in-place matrix matrix term by term division
 *
 * @param lhs : left hand side matrix
 * @param rhs : rigth hand side matrix
 * @return  lhs = lhs.*rhs
 */
void FUNCTION(pnl_mat,div_mat_term)(TYPE(PnlMat) *lhs, const TYPE(PnlMat) *rhs)
{
  FUNCTION(pnl_mat,map_mat)(lhs, rhs, CONCAT2(__op_div_,BASE));
}

/**
 * Computes y := alpha * op(A) * x + beta * y
 *     where op(X) = X or op(X) = X'
 *
 * @param A a  matrix
 * @param trans a char. If trans=='T', op(X) = X'. If trans=='N', op(X) = X.
 * @param x a vector such that op(A) * x is legal
 * @param alpha  a BASE constant
 * @param beta  a BASE constant. When b==0, the content of y is not used, instead y
 * is resized to store op(A) * x
 * @param y a vector containing alpha * op(A) * x + beta * y on exit.
 */
void FUNCTION(pnl_mat,dgemv) (char trans, BASE alpha, const TYPE(PnlMat) *A,
                    const TYPE(PnlVect) *x , BASE beta, TYPE(PnlVect) * y)
{
  BASE yi, temp;
  int i,j;
  
  if ( EQ(beta,ZERO) )
    {
      if (trans=='N' || trans=='n') FUNCTION(pnl_vect,resize) (y, A->m);
      else FUNCTION(pnl_vect,resize)(y, A->n);
      FUNCTION(pnl_vect,CONCAT2(set_,BASE)) (y, ZERO);
    }
  else if ( NEQ(beta, ONE) ) FUNCTION(pnl_vect,CONCAT2(mult_,BASE)) (y, beta);
  if ( EQ(alpha, ZERO) ) return;
  

  /* Form alpha * A * x + beta * y */
  if (trans=='N' || trans=='n')
    {
#ifndef PNL_RANGE_CHECK_OFF
      if (A->n != x->size || A->m != y->size)
        {
          PNL_ERROR ("size mismatch", "pnl_mat_dgemv");
        }
#endif
      for (i=0; i<y->size; i++)
        {
          temp = ZERO;
          for ( j=0 ; j<A->n ; j++)
            {
              temp = PLUS (temp, MULT(PNL_MGET (A, i, j), 
                                      FUNCTION(pnl_vect,get) (x, j) ) );  
            } 
          yi = FUNCTION(pnl_vect,get) (y, i);
          yi = PLUS( yi, MULT(alpha, temp)); 
          FUNCTION(pnl_vect,set) (y, i, yi);
        }
    }
  /* Form alpha * A' * x + beta * y */
  else   if (trans=='T' || trans=='t')
    {
#ifndef PNL_RANGE_CHECK_OFF
      if (A->m != x->size || A->n != y->size)
        {
          PNL_ERROR ("size mismatch", "pnl_mat_dgmev");
        }
#endif
      for (j=0; j<A->m; j++)
        {
          temp = MULT(alpha, FUNCTION(pnl_vect,get) (x, j));
          for (i=0; i<y->size; i++)
            {
              yi =  FUNCTION(pnl_vect,get) (y, i);
              yi = PLUS(yi, MULT(PNL_MGET (A, j, i), temp));
              FUNCTION(pnl_vect,set) (y, i, yi);
            }
        }
    }
  else
    {
      PNL_ERROR ("invalid value for trans", "pnl_mat_dgemv");
    }
}

/**
 *  Computes A * x
 *
 * @param A : matrix
 * @param x : vector
 * @return  y = A * x
 */
TYPE(PnlVect)* FUNCTION(pnl_mat,mult_vect)(const TYPE(PnlMat) *A, const TYPE(PnlVect) *x)
{
  TYPE(PnlVect) *y;
  y = FUNCTION(pnl_vect,create) (0);
  FUNCTION(pnl_mat,dgemv) ('N', ONE, A, x, ZERO, y);
  return y;
}

/**
 *  Computes A' * x
 *
 * @param A : matrix
 * @param x : vector
 * @return  y = A' * x
 */
TYPE(PnlVect)* FUNCTION(pnl_mat,mult_vect_transpose)(const TYPE(PnlMat) *A, const TYPE(PnlVect) *x)
{
  TYPE(PnlVect) *y;
  y = FUNCTION(pnl_vect,create) (A->n);
  FUNCTION(pnl_mat,dgemv) ('T', ONE, A, x, ZERO, y);
  return y;
}

/**
 *  Computes y = A * x
 *
 * @param A : matrix
 * @param x : vector
 * @param y : vector
 * @return  lhs=mat*rhs
 */
void FUNCTION(pnl_mat,mult_vect_inplace)(TYPE(PnlVect) *y, const TYPE(PnlMat) *A, const TYPE(PnlVect) *x)
{
  FUNCTION(pnl_mat,dgemv) ('N', ONE, A, x, ZERO, y);
}


/**
 *  Computes y = A' * x
 *
 * @param A : matrix
 * @param x : vector
 * @param y : vector
 * @return  y = A' * x
 */
void FUNCTION(pnl_mat,mult_vect_transpose_inplace)(TYPE(PnlVect) *y, const TYPE(PnlMat) *A, const TYPE(PnlVect) *x)
{
  FUNCTION(pnl_mat,dgemv) ('T', ONE, A, x, ZERO, y);
}

/**
 * Computes y := alpha * A * x + beta * y
 *
 * @param A a  matrix
 * @param x a vector such that A * x is legal
 * @param alpha  a BASE constant
 * @param beta  a BASE constant. When b==0, the content of y is not used, instead y
 * is resized to store A * x
 * @param y a vector containing alpha * A * x + beta * y on exit.
 */
void FUNCTION(pnl_mat,lAxpby)(BASE alpha, const TYPE(PnlMat) *A, const TYPE(PnlVect) *x, BASE beta, TYPE(PnlVect) * y)
{
  FUNCTION(pnl_mat,dgemv) ('N', alpha, A, x, beta, y);
}

/**
 * Computes Y := a * X + Y
 *
 * @param a BASE coefficient
 * @param X a matrix
 * @param Y a matrix whose size must be the same as the one of X. Contains the
 * result on exit
 */
void FUNCTION(pnl_mat,axpy) (BASE a, const TYPE(PnlMat) *X, TYPE(PnlMat) *Y)
{
  int i;
  CheckMatMatch(X, Y);
  if ( EQ(a, ZERO) ) return;
  if ( EQ(a, ONE) )
    {
      for ( i=0 ; i<Y->mn ; i++ )
        {
          Y->array[i] = PLUS(Y->array[i], X->array[i]);
        }
      return;
    }
  for ( i=0 ; i<Y->mn ; i++)
    {
        Y->array[i] = PLUS(Y->array[i], MULT(a, X->array[i]));
    }
}

/**
 * Computes A := alpha x' * y + A
 *
 * @param alpha a BASE number
 * @param x a vector
 * @param y a vector
 * @param A a matrix
 */
void FUNCTION(pnl_mat,dger) (BASE alpha, const TYPE(PnlVect) *x, const TYPE(PnlVect) *y, TYPE(PnlMat) *A)
{
  int i, j;
  BASE ai, alpha_xi;
#ifdef PNL_RANGE_CHECK_OFF
  if ((x->size != y->size) || (A->m != A->n) || (A->m != x->size))
    {
      PNL_ERROR ("size mismatch", "pnl_mat_dger");
    }
#endif
  if ( EQ(alpha,ZERO) ) return;

  for ( i=0 ; i<x->size ; i++)
    {
      alpha_xi = MULT(alpha, FUNCTION(pnl_vect,get) (x, i));
      for (j=0; j<x->size; j++)
        {
          ai = PNL_MGET (A, i, j);
          ai = PLUS(ai, MULT(alpha_xi, FUNCTION(pnl_vect,get) (x, j)));
          PNL_MLET(A, i, j) = ai;
        }
    }
}

/**
 *  matrix multiplication
 *
 * @param rhs1 : first right hand side matrix
 * @param rhs2 : second right hand side matrix
 * @return  rhs1*rhs2
 */

TYPE(PnlMat)* FUNCTION(pnl_mat,mult_mat)(const TYPE(PnlMat) *rhs1, const TYPE(PnlMat) *rhs2)
{
  TYPE(PnlMat) *lhs;
  lhs = FUNCTION(pnl_mat,create) (0,0);
  FUNCTION(pnl_mat,dgemm) ('N', 'N', ONE, rhs1, rhs2, ZERO, lhs);
  return lhs;
}

/**
 *  in-place matrix multiplication
 *
 * @param lhs : left hand side matrix
 * @param rhs1 : first right hand side matrix
 * @param rhs2 : second right hand side matrix
 * @return  lhs=rhs1*rhs2
 */

void FUNCTION(pnl_mat,mult_mat_inplace)(TYPE(PnlMat) *lhs, const  TYPE(PnlMat) *rhs1, const TYPE(PnlMat) *rhs2)
{
  FUNCTION(pnl_mat,dgemm) ('N', 'N', ONE, rhs1, rhs2, ZERO, lhs);
}




/**
 * sum matrix componentwise
 *
 * @param lhs : left hand side matrix
 * @return  sum=sum(lhs(i,j))
 */
BASE FUNCTION(pnl_mat,sum)(const TYPE(PnlMat) *lhs)
{
  BASE sum;
  int i;
  sum=ZERO;
  for(i=0;i<lhs->mn;i++) { sum = PLUS(sum, lhs->array[i]); }
  return sum;
}

/**
 * prod matrix componentwise
 *
 * @param lhs : left hand side matrix
 * @return  sum=sum(lhs(i,j))
 */
BASE FUNCTION(pnl_mat,prod)(const TYPE(PnlMat) *lhs)
{
  BASE p;
  int i;
  p=ONE;
  for(i=0;i<lhs->mn;i++) { p = MULT(p, lhs->array[i]); }
  return p;
}

/**
 * sum rows or columns matrix componentwise
 *
 * @param A : left hand side matrix
 * @param y a vector containing on exit y(j) = sum(A(i,j), i=1..m) if c='r'
 * and y(i) = sum(A(i,j), j=1..m) if c='c'
 * @param a :char equals 'r' (if we want to sum rows) we get
 * a row vector  or 'c' (if we want to sum columns) we get a
 * column vector
 */
void FUNCTION(pnl_mat,sum_vect)(TYPE(PnlVect) *y, const TYPE(PnlMat) *A, char a)
{
  
  BASE sum, yj;
  int i,j;
  if (a=='r')
    {
      /* the loops are done in a non natural order to make the most of
         data alignment while using matrix with row major storage */
      FUNCTION(pnl_vect,resize) (y, A->n);
      FUNCTION(pnl_vect,CONCAT2(set_, BASE)) (y, ZERO);
      for (i=0; i<A->m; i++)
        {
          for (j=0; j<A->n; j++)
            {
              yj = FUNCTION(pnl_vect,get) (y, j);
              yj = PLUS (yj, PNL_MGET (A, i, j));
              FUNCTION(pnl_vect,set) (y, j, yj);
            }
        }
    }
  else if (a=='c')
    {
      FUNCTION(pnl_vect,resize) (y, A->m);
      for (i=0; i<A->m; i++)
        {
          sum = ZERO;
          for (j=0; j<A->n; j++)
            {
              sum = PLUS (sum, PNL_MGET (A, i, j));
            }
          FUNCTION(pnl_vect,set) (y, i, sum);
        }
    }
  else
    {
      PNL_ERROR ("wrong char arg ('c' or 'r')", "pnl_mat_sum_vect");
    }
}


/**
 * prod rows or columns matrix componentwise
 *
 * @param A : left hand side matrix
 * @param y a vector containing on exit y(j) = prod(A(i,j), i=1..m) if c='r'
 * and y(i) = prod(A(i,j), j=1..m) if c='c'
 * @param a :char equals 'r' (if we want to prod rows) we get
 * a row vector  or 'c' (if we want to prod columns) we get a
 * column vector
 */
void FUNCTION(pnl_mat,prod_vect)(TYPE(PnlVect) *y, const TYPE(PnlMat) *A, char a)
{
  
  BASE prod, yj;
  int i,j;
  if (a=='r')
    {
      /* the loops are done in a non natural order to make the most of
         data alignment while using matrix with row major storage */
      FUNCTION(pnl_vect,resize) (y, A->n);
      FUNCTION(pnl_vect,CONCAT2(set_, BASE)) (y, ONE);
      for (i=0; i<A->m; i++)
        {
          for (j=0; j<A->n; j++)
            {
              yj = FUNCTION(pnl_vect,get) (y, j);
              yj = MULT (yj, PNL_MGET (A, i, j));
              FUNCTION(pnl_vect,set) (y, j, yj);
            }
        }
    }
  else if (a=='c')
    {
      FUNCTION(pnl_vect,resize) (y, A->m);
      for (i=0; i<A->m; i++)
        {
          prod = ONE;
          for (j=0; j<A->n; j++)
            {
              prod = MULT (prod, PNL_MGET (A, i, j));
            }
          FUNCTION(pnl_vect,set) (y, i, prod);
        }
    }
  else
    {
      PNL_ERROR ("wrong char arg ('c' or 'r')", "pnl_mat_prod_vect");
    }
}


/**
 * cumulative sum rows or columns matrix componentwise
 *
 * @param A : left hand side matrix
 * On exit A(i,j) = sum(A(k,j), k=1..i) if c='r'
 * and A(i,j) = sum(A(i,k), k=1..j) if c='c'
 * @param a :char equals 'r' (if we want to sum rows) we get
 * a row vector  or 'c' (if we want to sum columns) we get a
 * column vector
 */
void FUNCTION(pnl_mat,cumsum)(TYPE(PnlMat) *A, char a)
{
  
  BASE sum, Aij, Ai1j;
  int i, i1, j;
  if (a=='r')
    {
      /* the loops are done in a non natural order to make the most of
         data alignment while using matrix with row major storage */
      for (i=1, i1=0; i<A->m; i++, i1++)
        {
          for (j=0; j<A->n; j++)
            {
              Aij = PNL_MGET (A, i, j);
              Ai1j = PNL_MGET(A, i1, j);
              PNL_MLET(A, i, j) = PLUS(Ai1j, Aij);
            }
        }
    }
  else if (a=='c')
    {
      for (i=0; i<A->m; i++)
        {
          sum = PNL_MGET (A, i, 0);
          for (j=1; j<A->n; j++)
            {
              sum = PLUS(sum, PNL_MGET(A, i, j));
              PNL_MLET (A, i, j) =  sum;
            }
        }
    }
  else
    {
      PNL_ERROR ("wrong char arg ('c' or 'r')", "pnl_mat_cumsum");
    }
}

/**
 * cumulative prod rows or columns matrix componentwise
 *
 * @param A : left hand side matrix
 * On exit A(i,j) = prod(A(k,j), k=1..i) if c='r'
 * and A(i,j) = prod(A(i,k), k=1..j) if c='c'
 * @param a :char equals 'r' (if we want to prod rows) we get
 * a row vector  or 'c' (if we want to prod columns) we get a
 * column vector
 */
void FUNCTION(pnl_mat,cumprod)(TYPE(PnlMat) *A, char a)
{
  
  BASE prod, Aij, Ai1j;
  int i, i1, j;
  if (a=='r')
    {
      /* the loops are done in a non natural order to make the most of
         data alignment while using matrix with row major storage */
      for (i=1, i1=0; i<A->m; i++, i1++)
        {
          for (j=0; j<A->n; j++)
            {
              Aij = PNL_MGET (A, i, j);
              Ai1j = PNL_MGET(A, i1, j);
              PNL_MLET(A, i, j) = MULT(Ai1j, Aij);
            }
        }
    }
  else if (a=='c')
    {
      for (i=0; i<A->m; i++)
        {
          prod = PNL_MGET (A, i, 0);
          for (j=1; j<A->n; j++)
            {
              prod = MULT(prod, PNL_MGET(A, i, j));
              PNL_MLET (A, i, j) = prod;
            }
        }
    }
  else
    {
      PNL_ERROR ("wrong char arg ('c' or 'r')", "pnl_mat_cumprod");
    }
}


#if defined ORDERED
extern void FUNCTION(pnl_array, min_index)(const BASE *a, int n, int incr,
                                           BASE *min_out, int *imin_out);
extern void FUNCTION(pnl_array, max_index)(const BASE *a, int n, int incr,
                                           BASE *max_out, int *imax_out);
extern void FUNCTION(pnl_array, minmax_index)(const BASE *a, int n, int incr,
                                              BASE *min_out, BASE *max_out,
                                              int *imin_out, int *imax_out);

/**
 * returns the maxima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (out(i) = max(A(i,:)) or 'r' (out(i) = max(A(:,i))
 * @param out a vector containing on exit the maxima
 * @param index a vector of integers containing on exit the indices of the
 * maxima. if NULL, no index is computed. 
 */
void FUNCTION(pnl_mat, max_index)(const TYPE(PnlMat) *A, TYPE(PnlVect) *out, PnlVectInt *index, char d)
{
  int length, n, incr, i, lda, fake;
  switch (d)
    {
    case 'c' : n = A->m; length = A->n; lda = A->n;  incr = 1; break;
    case 'r' : length = A->m; n = A->n, lda = 1; incr = A->n; break;
    default : PNL_ERROR("unknow direction", "pnl_mat_max_index"); break;
    }
  if (index!=NULL) pnl_vect_int_resize (index, n);
  FUNCTION(pnl_vect, resize)(out, n);

  for (i=0; i<n; i++)
    {
      FUNCTION(pnl_array, max_index)(&(A->array[i*lda]), length, incr, &(out->array[i]), &fake);
      if (index!=NULL) index->array[i] = fake;
    }
}

/**
 * returns the minima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (out(i) = min(A(i,:)) or 'r' (out(i) = min(A(:,i))
 * @param out a vector containing on exit the minima
 * @param index a vector of integers containing on exit the indices of the
 * minima. if NULL, no index is computed. 
 */
void FUNCTION(pnl_mat, min_index)(const TYPE(PnlMat) *A, TYPE(PnlVect) *out, PnlVectInt *index, char d)
{
  int length, n, incr, i, fake, lda;
  switch (d)
    {
    case 'c' : n = A->m; length = A->n; lda = A->n; incr = 1; break;
    case 'r' : length = A->m; n = A->n; lda = 1; incr = A->n; break;
    default : PNL_ERROR("unknow direction", "pnl_mat_min_index"); break;
    }
  if (index!=NULL) pnl_vect_int_resize (index, n);
  FUNCTION(pnl_vect, resize)(out, n);

  for (i=0; i<n; i++)
    {
      FUNCTION(pnl_array, min_index)(&(A->array[i*lda]), length, incr, &(out->array[i]), &fake);
      if (index!=NULL) index->array[i] = fake;
    }
}

/**
 * returns the minima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (out(i) = min(A(i,:)) or 'r' (out(i) = min(A(:,i))
 * @param out_min a vector containing on exit the minima
 * @param out_max a vector containing on exit the maxima
 * @param index_min a vector of integers containing on exit the indices of the
 * minima. if NULL, no index is computed.
 * @param index_max a vector of integers containing on exit the indices of the
 * maxima. if NULL, no index is computed.
 */
void FUNCTION(pnl_mat, minmax_index)(const TYPE(PnlMat) *A, TYPE(PnlVect) *out_min, TYPE(PnlVect) *out_max, PnlVectInt *index_min, PnlVectInt *index_max, char d)
{
  int length, n, incr, i, lda, fake_min, fake_max;
  switch (d)
    {
    case 'c' : n = A->m; length = A->n; lda = A->n; incr = 1; break;
    case 'r' : length = A->m; n = A->n; lda = 1; incr = A->n; break; 
    default : PNL_ERROR("unknow direction", "pnl_mat_min_index"); break;
    }
  if (index_min != NULL) pnl_vect_int_resize (index_min, n);
  if (index_max != NULL) pnl_vect_int_resize (index_max, n);
  FUNCTION(pnl_vect, resize)(out_min, n);
  FUNCTION(pnl_vect, resize)(out_max, n);

  for (i=0; i<n; i++)
    {
      FUNCTION(pnl_array, minmax_index)(&(A->array[i*lda]), length, incr, &(out_min->array[i]), &(out_max->array[i]), &fake_min, &fake_max);
      if (index_min != NULL) index_min->array[i] = fake_min;
      if (index_max != NULL) index_max->array[i] = fake_max;
    }
}


/**
 * returns the maxima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (out(i) = max(A(i,:)) or 'r' (out(i) = max(A(:,i))
 * @param out a vector containing on exit the maxima
 */
void FUNCTION(pnl_mat,max)(const TYPE(PnlMat) *A, TYPE(PnlVect) *out, char d)
{
  FUNCTION (pnl_mat, max_index)(A, out, NULL, d);
}

/**
 * returns the minima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (out(i) = min(A(i,:)) or 'r' (out(i) = min(A(:,i))
 * @param out a vector containing on exit the minima
 */
void FUNCTION(pnl_mat,min)(const TYPE(PnlMat) *A, TYPE(PnlVect) *out, char d)
{
  FUNCTION (pnl_mat, min_index)(A, out, NULL, d);
}

/**
 * returns the minima and maxima of the components of a matrix row-wise or column-wise
 *
 * @param A a matrix
 * @param d can be 'c' (min_out(i) = min(A(i,:)) or 'r' (min_out(i) = min(A(:,i))
 * @param min_out a vector containing on exit the minima
 * @param max_out a vector containing on exit the maxima
 */
void FUNCTION(pnl_mat,minmax)(const TYPE(PnlMat) *A, TYPE(PnlVect) *min_out,
                              TYPE(PnlVect) *max_out, char d)
{
  FUNCTION (pnl_mat, minmax_index)(A, min_out, max_out, NULL, NULL, d);
}

static int FUNCTION(__pnl, cmp_i) ( const void *a, const void *b)
{
  if ( * (BASE *) a < * (BASE *) b) return -1;
  if ( * (BASE *) a == * (BASE *) b) return 0;
  return 1;
}

static int FUNCTION(__pnl, cmp_d) ( const void *a, const void *b)
{
  if ( * (BASE *) a > * (BASE *) b) return -1;
  if ( * (BASE *) a == * (BASE *) b) return 0;
  return 1;
}

static void FUNCTION(pnl_mat, qsort_aux)(TYPE(PnlMat) * A, PnlMatInt *t, int use_index, char dir, char order)
{
  int i, j, lda, loops, n, incr, *index = NULL;
  cmp_func f;

  lda = incr = loops = n = 0;   /* avoid warnings */
  
  if (use_index == TRUE) { pnl_mat_int_resize (t, A->m, A->n); index=t->array; }

  switch (order)
    {
    case 'i' : f = FUNCTION(__pnl, cmp_i); break;
    case 'd' : f = FUNCTION(__pnl, cmp_d); break;
    default : PNL_ERROR ("unknow order", "pnl_mat_qsort"); break;
    }
  
  switch (dir)
    {
    case 'r' : lda = A->n; incr = 1; n = A->m; loops = A->n;
      if (use_index == TRUE){
        for (i=0; i<t->m; i++)
          for (j=0; j<t->n; j++) { pnl_mat_int_set (t, i, j, i); }
      }
      break;
    case 'c' : lda = 1; incr = A->n; n = A->n; loops = A->m;
      if (use_index == TRUE){
        for (i=0; i<t->m; i++)
          for (j=0; j<t->n; j++) { pnl_mat_int_set (t, i, j, j); }
      }
      break;
    default: PNL_ERROR("unknown direction", "pnl_mat_qsort_aux"); break;
    }
  for (i=0; i< loops; i++)
    pnl_qsort ( A->array + i * incr, n, sizeof(BASE), lda, index + i * incr, lda, use_index, f);
}

/**
 * Quick sort function for matrices
 *
 * @param A a TYPE(PnlMat), contains the sorted matrix on exit
 * @param order can be 'i' or 'd' for increasing or decreasing order.
 * @param dir can be 'r' or 'c' 
 */
void FUNCTION(pnl_mat, qsort)(TYPE(PnlMat) * A, char dir, char order)
{
  FUNCTION(pnl_mat, qsort_aux)(A, NULL, FALSE, dir, order);
}

/**
 * Quick sort function for matrices with permutation index
 *
 * @param A a TYPE(PnlMat), contains the sorted matrix on exit
 * @param t a PnlMatInt, on exit contains the permutation used to sort A
 * @param order can be 'i' or 'd' for increasing or decreasing order.
 * @param dir can be 'r' or 'c' 
 */
void FUNCTION(pnl_mat, qsort_index)(TYPE(PnlMat) * A, PnlMatInt *t, char dir, char order)
{
  FUNCTION(pnl_mat, qsort_aux)(A, t, TRUE, dir, order);
}

#endif

/****************************
 *** PnlHMat functions ***
 ****************************/


TYPE(PnlHMat)* FUNCTION(pnl_hmat,create)(int ndim, const int *dims)
{
  TYPE(PnlHMat) *H;
  int i;
  int s=1;
  const int *ptr;
  ptr=dims;
  if((H=malloc(sizeof(TYPE(PnlHMat))))==NULL)
    return NULL;
  H->ndim=ndim;
  if (ndim>0)
    {
      if ((H->dims=malloc(sizeof(int)*ndim))==NULL)
        return NULL;
      memcpy(H->dims, dims, ndim*sizeof(int));
      for(i=0;i<ndim;i++)
        { s*=(*ptr); ptr++;}
      H->mn=s;
      if((H->array=malloc(H->mn*sizeof(BASE)))==NULL)
        return NULL;
    }
  else
    {
      H->dims = (int*)NULL;
      H->mn=0;
      H->array = (BASE*)NULL;
    }
  return H;  
}

TYPE(PnlHMat)* FUNCTION(pnl_hmat,CONCAT2(create_from_,BASE))(int ndim, const int *dims, BASE x)
{
  TYPE(PnlHMat) *H;
  int i=0;
  BASE *ptr;
  
  if ((H=FUNCTION(pnl_hmat,create)(ndim,dims))==NULL)
    return NULL;

  ptr=H->array;
  while(i<H->mn)
    {
      *ptr=x;
      ptr++; i++;
    }
  return H;
}


TYPE(PnlHMat)* FUNCTION(pnl_hmat,create_from_ptr)(int ndim, const int *dims, const BASE *x)
{
  TYPE(PnlHMat) *H;
  
  if ((H=FUNCTION(pnl_hmat,create)(ndim,dims))==NULL)
    return NULL;
  
  memcpy(H->array, x, H->mn*sizeof(BASE));
  return H;
}


void FUNCTION(pnl_hmat,free)(TYPE(PnlHMat) **H)
{
  if (*H != NULL)
    {
      free((*H)->array);
      free((*H)->dims);
      free(*H);
      *H=NULL;
    }
}


/**
 * copies a TYPE(PnlHMat) 
 *
 * @param H : a constant TYPE(PnlHMat) pointer
 * @return a TYPE(PnlHMat) pointer initialised with H
 */
TYPE(PnlHMat)* FUNCTION(pnl_hmat,copy)(const TYPE(PnlHMat) *H)
{
  TYPE(PnlHMat) *ret;
  if ((ret=FUNCTION(pnl_hmat,create)(H->ndim, H->dims))==NULL) return NULL;
  memcpy(ret->array, H->array, sizeof(BASE)*ret->mn);
  return ret;
}


/**
 * clones a TYPE(PnlHMat)
 *
 * @param H : a constant TYPE(PnlHMat) pointer
 * @param clone : needs to be already allocated. NOT TESTED
 */
void FUNCTION(pnl_hmat,clone)(TYPE(PnlHMat) *clone, const TYPE(PnlHMat) *H)
{
  FUNCTION(pnl_hmat,resize)(clone, H->ndim,H->dims);
  memcpy(clone->array, H->array, sizeof(BASE)*H->mn);
}

/**
 * resizes a TYPE(PnlHMat).
 *
 * If the new size is smaller than the current one, no
 * memory is free. If the new size is larger than the
 * current one, more space is allocated. Note that for the
 * sake of efficiency the old data are not copied.
 *
 * @param H : a pointer to an already existing TYPE(PnlHMat) (mn
 * must be initialised)
 * @param ndim : new nb dimensions
 * @param dims : new pointer to the dimensions array
 *
 * @return OK or FAIL. When returns OK, the hmatrix is changed. 
 */
int FUNCTION(pnl_hmat,resize)(TYPE(PnlHMat) *H, int ndim, const int *dims)
{
  int i;
  int s=1;
  const int *ptr;
  ptr=dims;
  for(i=0;i<ndim;i++) { s*=(*ptr); ptr++; }

  if (H->mn == s) /*nothing to do, just adjust ndim and dims*/
    {
      H->ndim=ndim;
      if(H->ndim> ndim) if ((H->dims=realloc(H->dims,sizeof(int)*ndim))==NULL) return FAIL;
      memcpy(H->dims, dims, ndim*sizeof(int));
      return OK;
    }
  if (s< 0) return FAIL;
  if (s==0) /* free array */
    {
      H->ndim =  H->mn = 0;
      H->dims=NULL;
      free(H->array); H->array = NULL;
      return OK;
    }

  H->ndim=ndim; H->mn=s;
  if ((H->dims=realloc(H->dims,sizeof(int)*ndim))==NULL) return FAIL;
  memcpy(H->dims, dims, ndim*sizeof(int));
  if (H->array==NULL)
    {
      if ((H->array = malloc(H->mn*sizeof(BASE)))==NULL)
        return FAIL;
    }else
    {
      if ((H->array = realloc(H->array,H->mn*sizeof(BASE)))==NULL)
        return FAIL;
    }
  return OK;
}


/**
 * sets the value of self[tab]=x
 *
 * @param self : a TYPE(PnlHMat)
 * @param tab : coordinates array 
 * @param x : self[tab]=x
 */
void FUNCTION(pnl_hmat,set)(TYPE(PnlHMat) *self, int *tab, BASE x)
{
  int i,j;
  int *ptr;
  int s=0;
  int p;
  int d=self->ndim;
  CheckIndexHMat(self,tab);
  
 
  for(i=0;i<d;i++)
    {
      ptr=self->dims;
      p=1.0; ptr+=i+1;
      for(j=i+1;j<d;j++)
        {
          p*=(*ptr); ptr++;
        }
      s+=(*tab)*p; tab++;
    }
  self->array[s]=x;
}

/**
 * gets the value of self[tab]
 *
 * @param self : a TYPE(PnlHMat)
 * @param tab : coordinates array; 
 * @return  self[tab]
 */
BASE FUNCTION(pnl_hmat,get)(const TYPE(PnlHMat) *self, int *tab)
{
  int i,j;
  int *ptr;
  int s=0;
  int p;
  int d=self->ndim;
  CheckIndexHMat(self,tab);
  
 
  for(i=0;i<d;i++)
    {
      ptr=self->dims;
      p=1.0; ptr+=i+1;
      for(j=i+1;j<d;j++)
        {
          p*=(*ptr); ptr++;
        }
      s+=(*tab)*p; tab++;
    }
  return self->array[s];
}

/**
 * returns the address of self[tab] for use as a lvalue.
 *
 * @param self : a TYPE(PnlHMat)
 * @param tab : coordinates array 
 * @return  &(self[i,j])
 */
BASE* FUNCTION(pnl_hmat,lget)(TYPE(PnlHMat) *self, int *tab)
{
  int i,j;
  int *ptr;
  int s=0;
  int p;
  int d=self->ndim;
  CheckIndexHMat(self,tab);
  
 
  for(i=0;i<d;i++)
    {
      ptr=self->dims;
      p=1.0; ptr+=i+1;
      for(j=i+1;j<d;j++)
        {
          p*=(*ptr); ptr++;
        }
      s+=(*tab)*p; tab++;
    }
  return &(self->array[s]);
}

/**
 * Print a hmatrix
 *
 * @param H : a TYPE(PnlHMat) pointer.
 */
void FUNCTION(pnl_hmat,print)(const TYPE(PnlHMat) *H)
{
  BASE *lptr=H->array;
  int *ptr=NULL;
  int i,j,s;
  int nd;
  int nd_1;
  int *index=NULL;
  int d=H->ndim;
  int l=d-3;
  s=0;
  ptr=H->dims+(d-1);/*last cell of H->dims */
  nd=*(ptr);
  nd_1=*(ptr-1);
  if ((index=malloc((d-2)*sizeof(int)))==NULL)
    abort();
  for (i=0; i<d-2; i++) index[i]=0;
  while(s<H->mn)
    { printf("cell=[ ");
      for(i=0;i<d-2;i++) printf("%d ",index[i]);
      printf("]\n");
      for (i=0;i<nd_1;i++)
        {
          for (j=0;j<nd;j++)
            {
              printf(OUT_FORMAT ,OUT_PUT_FORMAT(*lptr));
              lptr++;s++;
            }
          printf("\n");
        }
      if(l>=0)
        {
          if(index[l]<H->dims[l]-1)
            (index[l])++;
          else
            {
              while(l>=0 && index[l]==H->dims[l]-1)
                {index[l]=0; l--;}
              if (l==-1 && s != H->mn)
                abort();
              if (l>=0)
                {
                  (index[l])++; l=d-3;
                }
            }
        }
    }
  free(index);
}

/**
 * in-place hypermatrix hypermatrix addition
 *
 * @param lhs : left hand side hypermatrix
 * @param rhs : rigth hand side hypermatrix
 * @return  lhs = lhs+rhs
 */
void FUNCTION(pnl_hmat,plus_hmat)(TYPE(PnlHMat) *lhs, const TYPE(PnlHMat) *rhs)
{

  BASE *lptr=lhs->array, *rptr=rhs->array;
  int i=0;
  CheckHMatMatch(lhs, rhs);
  while (i<lhs->mn)
    {
      (*lptr)=PLUS((*lptr),(*rptr)); lptr++; rptr++;  i++;
    }
}

/**
 * in-place hyper matrix scalar multiplication
 *
 * @param lhs : left hand side hyper matrix
 * @param x : scalar
 * @return  lhs = lhs*x
 */
void FUNCTION(pnl_hmat,mult_BASE)(TYPE(PnlHMat) *lhs, BASE x)
{
  BASE *lptr=lhs->array;
  int i=0;
  while (i<lhs->mn)
    {
      (*lptr)=MULT((*lptr),x); lptr++; i++;
    }
}

