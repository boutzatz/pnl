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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pnl_basis.h"
#include "pnl_vector.h"
#include "pnl_matrix.h"
#include "pnl_mathtools.h"

#ifndef PNL_RANGE_CHECK_OFF
#define CHECK_NB_FUNC(coef, basis)                             \
if ( coef->size != basis->nb_func )                             \
{                                                               \
  PNL_ERROR("Wrong number of coefficients", "pnl_basis_eval"); \
}
#else
#define CHECK_NB_FUNC(coef, basis) 
#endif

/**
 * Returns the total degree of the polynomials represented by 
 * line i of T
 *
 * @param T a matrix of integers representing the decomposition of the mutli-d
 * polynomials
 * @param i the index of the element to be considered in the basis (i.e. the row
 * of T to consider)
 * @return  the total degree of the polynomials represented by 
 * line i of T
 */
static int count_degree (const PnlMatInt *T, int i)
{
  int j, deg;
  deg = 0;

  for ( j=0 ; j<T->n ; j++ ) { deg += PNL_MGET (T, i, j); }
  return deg;
}


/** 
 * Copies T_prev(T_previ, :) into T(Ti,:)
 * 
 * @param T an integer matrix with more columns than that T_prev
 * @param T_prev an integer matrix containing the last tensor computed
 * @param Ti the index of the line to consider in T
 * @param T_previ the index of the line to consider in T_prev
 * 
 */
static void copy_previous_basis (PnlMatInt *T, PnlMatInt *T_prev, int Ti, int T_previ)
{
  int j;
  for ( j=0 ; j<T_prev->n ; j++ )
    {
      PNL_MLET (T, Ti, j) = PNL_MGET (T_prev, T_previ, j);
    }
}

/** 
 * Computes the integer matrix representing the decomposition as a tensor
 * product of the elements of * the multi-b basis onto the 1-d basis
 * 
 * @param nb_variates the number of variates
 * @param nb_func the number of elements of the basis
 * 
 * @return a tensor
 */
static PnlMatInt* compute_tensor (int nb_func, int nb_variates)
{
  PnlMatInt *T;
  T = pnl_mat_int_create (nb_func, nb_variates);
  if (nb_variates == 1) 
    {
      int i;
      for ( i=0 ; i<nb_func ; i++ ) { PNL_MLET (T, i, 0) = i; }
      return T;
    }
  else
    {
      int current_degree, deg;
      int block, block_start;
      int i, j, k;
      PnlMatInt *T_prev; 
      current_degree = 0;
      T_prev = compute_tensor (nb_func, nb_variates-1); 

      i = 0;
      while (1) /* loop on global degree */
        {
          /* determining the last element of global degree <= current_degree */
          j = 0;
          while ( j<T_prev->m && count_degree (T_prev, j) < current_degree + 1) { j++; } 
          j--;
          for ( k=j, deg=current_degree ; k>=0 ; deg--)
            {
              block_start = k;
              if (deg > 0)
                {
                  /* Find the beginning of the block with global degree deg */
                  while ( count_degree (T_prev, block_start) == deg ) { block_start--; }
                  block_start++;
                }
              /* Loop in an incresing order over the block of global degree deg */ 
              for ( block=block_start ; block<=k ; block++ , i++ )
                {
                  if ( i==nb_func ) { pnl_mat_int_free (&T_prev); return T; }
                  copy_previous_basis (T, T_prev, i, block); 
                  PNL_MLET (T, i, nb_variates-1) = current_degree - deg;
                }
              /* Consider the previous block */
              k = block_start - 1;
            }
          current_degree++;
        }
    }
}

/** 
 * Computes the number of elements with total degree less or equal than degree
 * in the basis with (T->n + 1) variates
 * 
 * @param T the tensor matrix of the basis with n-1 variates
 * @param degree the maximum total degree requested
 * 
 * @return the number of elements with total degree less or equal than degree in
 * the basis with n variates
 */
static int compute_nb_elements (const PnlMatInt *T, int degree)
{
  int i; 
  int current_degree; /* Current total degree under investigation in the loop */
  int total_elements; /* Number of elements of total degree smaller than degree */
  int elements_in_degree; /* Number of element of degree exactly the one under investigation */
  i = 0;
  current_degree = 0;
  total_elements = 0;
  while ( i<T->m )
    {
      elements_in_degree = 0;
      /* search for the numbers of elements with exact degree "current_degree" */
      while ( i + elements_in_degree < T->m && count_degree (T, elements_in_degree + i) < current_degree + 1) 
        { 
          elements_in_degree++; 
        } 
      total_elements += elements_in_degree * (degree - current_degree + 1);
      /* jump ahead of the number of elements of the current degree */
      i += elements_in_degree;
      current_degree++;
    }
  return total_elements;
}

/** 
 * Computes the tensor matrix of the nb_variates variate basis with a total degree less or
 * equal than degree
 * 
 * @param nb_variates the number of variates of the basis.
 * @param degree the total degree
 * 
 * @return the tensor matrix of the nb_variates variate basis with a total degree less or
 * equal than degree
 */
static PnlMatInt* compute_tensor_from_degree (int degree, int nb_variates)
{
  PnlMatInt *T;
  if (nb_variates == 1) 
    {
      int i;
      T = pnl_mat_int_create (degree+1, nb_variates);
      for ( i=0 ; i<degree+1 ; i++ ) { PNL_MLET (T, i, 0) = i; }
      return T;
    }
  else
    {
      int nb_elements;
      int current_degree, deg;
      int block, block_start;
      int i, j, k;
      PnlMatInt *T_prev; 
      current_degree = 0;
      /* Compute the tensor with one variate less */
      T_prev = compute_tensor_from_degree (degree, nb_variates-1); 
      /* Compute the number of rows of T */
      nb_elements = compute_nb_elements (T_prev, degree);
      T = pnl_mat_int_create (nb_elements, nb_variates);
      i = 0;
      /* loop on global degree */
      for ( current_degree=0 ; current_degree <= degree ; current_degree++ )
        {
          /* Determine the last element of global degree <= current_degree */
          j = 0;
          while ( j<T_prev->m && count_degree (T_prev, j) < current_degree + 1 ) { j++; } 
          j--;
          for ( k=j, deg=current_degree ; k>=0 ; deg--)
            {
              block_start = k;
              if (deg > 0)
                {
                  /* Find the beginning of the block with global degree deg */
                  while ( count_degree (T_prev, block_start) == deg ) { block_start--; }
                  block_start++;
                }
              /* Loop in an incresing order over the block of global degree deg */ 
              for ( block=block_start ; block<=k ; block++ , i++ )
                {
                  copy_previous_basis (T, T_prev, i, block); 
                  PNL_MLET (T, i, nb_variates-1) = current_degree - deg;
                }
              /* Consider the previous block  */
              k = block_start - 1;
            }
        }
      pnl_mat_int_free (&T_prev);
      return T;
    }
}

/** 
 * First order derivative
 * 
 * @param b a basis
 * @param x the point at which to evaluate the first derivative
 * @param i the index of the basis element to differentiate
 * @param j the index of the variable w.r.t which we differentiate
 * 
 * @return (D(b_i)/Dj)(x)
 */
static double D_basis_i ( PnlBasis *b, double *x, int i, int j )
{
    int k;
    double aux = 1;
    for ( k=0 ; k < b->nb_variates ; k++ ) 
      {
        if ( k == j-1 ) 
          aux *= (b->Df) (x + k, PNL_MGET(b->T, i, k));
        else
          aux *= (b->f) (x + k, PNL_MGET(b->T, i, k)); 
      } 
    return aux;
}

/** 
 * Second order derivative
 * 
 * @param b a basis
 * @param x the point at which to evaluate the first derivative
 * @param i the index of the basis element to differentiate
 * @param j1 the index of the first variable w.r.t which we differentiate
 * @param j2 the index of the second variable w.r.t which we differentiate
 * 
 * @return (D(b_i)/(Dj1 Dj2))(x)
 */
static double D2_basis_i (PnlBasis *b, double *x, int i, int j1, int j2)
{
  int k;
  double aux = 1;
  if (j1 == j2)
    {
      for ( k = 0 ; k < b->nb_variates ; k++ )
        {
          if ( k == j1-1 )
            aux *= (b->D2f) (x + k, PNL_MGET(b->T, i, k));
          else
            aux *= (b->f) (x + k, PNL_MGET(b->T, i, k));
        }
    }
  else
    {
      for ( k = 0 ; k < b->nb_variates ; k++ )
        {
          if ( k == j1-1  || k == j2-1)
            aux *= (b->Df) (x + k, PNL_MGET(b->T, i, k));
          else
            aux *= (b->f) (x + k, PNL_MGET(b->T, i, k));
        }

    }
  return aux;
}


/**
 *  Canonical polynomials
 *  @param x the address of a real number
 *  @param ind the index of the polynom to be evaluated
 */
static double CanonicalD1(double *x, int ind)
{
  return pnl_pow_i (*x, ind);
}

/**
 *  First derivative of the Canonical polynomials
 *  @param x the address of a real number
 *  @param ind the index of the polynom whose first derivative is to be evaluated
 */
static double DCanonicalD1(double *x, int ind)
{
  if (ind == 0) return 0.;
  return ind * pnl_pow_i (*x, ind - 1);
}

/**
 *  Second derivative of the Canonical polynomials
 *  @param x the address of a real number
 *  @param ind the index of the polynom whose second derivative is to be evaluated
 */
static double D2CanonicalD1(double *x, int ind)
{
  if (ind <= 1) return 0.;
  return ind * (ind - 1) * pnl_pow_i (*x, ind - 2);
}

/**
 * The terminal recursive function to compute Hermite polynomials of any
 * order. This function is only used for order > 7
 *  @param x the address of a real number
 *  @param n the order of the polynom to be evaluated
 *  @param n0 7 on input
 *  @param f_n used to store the polynomial of order n. On input is P(7)
 *  @param f_n_1 used to store the polynomial of order n-1. On input is P(6)
 */
static double Hermite_rec (double *x, int n, int n0, double *f_n, double *f_n_1)
{
  if (n == 7)
    {
      return *f_n;
    }
  else
    {
      double save = *f_n;
      *f_n = (*x) * (*f_n) - n0 * (*f_n_1);
      *f_n_1 = save;
      return Hermite_rec (x, n-1, n0 + 1, f_n, f_n_1);
    }
}

/**
 *  Hermite polynomials
 *  @param x the address of a real number
 *  @param n the index of the polynom to be evaluated
 */
static double HermiteD1(double *x, int n)
{
  double val = *x;
  double val2;
  double f_n, f_n_1;
  switch (n)
    {
    case 0 : return 1;
    case 1 : return val;
    case 2 : return val*val-1.;
    case 3 : return (val*val-3.)*val;
    case 4 : val2 = val * val;
      return (val2-6.)*val2+3;
    case 5 : val2 = val * val;
      return ((val2-10)*val2+15.)*val;
    case 6 : val2 = val * val;
      return ((val2 - 15.)*val2 + 45.) * val2 - 15.;
    case 7: val2 = val * val;
      return (((val2 - 21.) * val2 + 105. ) * val2 - 105) * val;
    default:
      f_n = HermiteD1 (x, 7);
      f_n_1 = HermiteD1 (x, 6);
      return Hermite_rec (x, n, 7, &f_n, &f_n_1);
    }
}

/**
 *  First derivative of the Hermite polynomials
 *  @param x the address of a real number
 *  @param n the index of the polynom whose derivative is to be evaluated
 */
static double DHermiteD1(double *x, int n)
{
  if (n == 0) return 0.;
  else return n * HermiteD1 (x, n-1);
  
}

/**
 *  Second derivative of the Hermite polynomials
 *  @param x the address of a real number
 *  @param n the index of the polynom whose second derivative is to be evaluated
 */
static double D2HermiteD1(double *x, int n)
{
  if (n == 0 || n==1) return 0.;
  return n * (n-1) * HermiteD1 (x, n-2);
}

/**
 * The terminal recursive function to compute Tchebychev polynomials of any
 * order. This function is only used for order > 7
 *  @param x the address of a real number
 *  @param n the order of the polynom to be evaluated
 *  @param f_n used to store the polynomial of order n. On input is P(7)
 *  @param f_n_1 used to store the polynomial of order n-1. On input is P(6)
 */
static double Tchebychev_rec (double *x, int n, double *f_n, double *f_n_1)
{
  if (n == 7)
    {
      return *f_n;
    }
  else
    {
      double save = *f_n;
      *f_n = 2 * (*x) * (*f_n) - (*f_n_1);
      *f_n_1 = save;
      return Tchebychev_rec (x, n-1, f_n, f_n_1);
    }
}

/**
 *  Tchebytchev polynomials of any order
 *  @param x the address of a real number
 *  @param n the order of the polynom to be evaluated
 */
static double TchebychevD1(double *x, int n)
{
  double val = *x;
  double val2, val3, val4;
  double f_n, f_n_1;
  switch (n)
    {
    case 0 :
      return 1.;
    case 1 :
      return val;
    case 2 :
      return 2. * val * val - 1.;
    case 3 :
      return (4. * val * val - 3.) * val;
    case 4 :
      val2 = val * val;
      return 8. * val2 * val2 - 8. * val2 + 1.;
      break;
    case 5 :
      val2 = val * val; val3 = val2 * val;
      return 16. * val3 * val2 - 20. * val3 + 5.* val;
    case 6 :
      val2 = val * val; val4 = val2 * val2;
      return 32. * val4 * val2 - 48. * val4 + 18. * val2 - 1;
    case 7 :
      val2 = val * val; val3 = val2 * val; val4 = val2 * val2;
      return (64. * val4 - 112. * val2 + 56) * val3 - 7. * val;
    default :
      f_n = TchebychevD1 (x, 7);
      f_n_1 = TchebychevD1 (x, 6);
      return Tchebychev_rec (x, n, &f_n, &f_n_1);
    }
}

/**
 * The terminal recursive function to compute the first derivative of the
 * Tchebychev polynomials of any order. This function is only used for order >
 * 7
 *
 *  @param x the address of a real number
 *  @param n the order of the polynomial to be evaluated
 *  @param n0 7 on input
 *  @param f_n used to store the derivative of the polynomial of order n. On
 *  input is P'(7) 
 *  @param f_n_1 used to store the derivative of  the polynomial of order
 *  n-1. On input is P'(6) 
 */
static double DTchebychev_rec (double *x, int n, int n0, double *f_n, double *f_n_1)
{
  if (n == 7)
    {
      return *f_n;
    }
  else
    {
      double save = *f_n;
      *f_n = 2 * (*x) * (*f_n) - (*f_n_1) + 2 * TchebychevD1 (x, n0);
      *f_n_1 = save;
      return DTchebychev_rec (x, n-1, n0+1, f_n, f_n_1);
    }
}

/**
 *  First derivative of the Tchebytchev polynomials
 *  @param x the address of a real number
 *  @param n the index of the polynom whose first derivative is to be evaluated
 */
static double DTchebychevD1(double *x, int n)
{
  double val = *x;
  double val2, val3, val4;
  double f_n, f_n_1;
  switch (n)
    {
    case 0 :
      return 0.;
    case 1 :
      return 1.;
    case 2 :
      return 4. * val;
    case 3 :
      return (12. * val * val - 3.);
    case 4 :
      return (32. * val * val - 16.) * val;
    case 5 :
      val2 = val * val;
      return 80. * val2 * val2 - 60. * val2 + 5.;
    case 6 :
      val2 = val * val; val4 = val2 * val2;
      return (192. * val4 - 192. * val2 + 36.) * val;
    case 7 :
      val2 = val * val; val3 = val2 * val; val4 = val2 * val2;
      return (448. * val4 - 560. * val2 + 168) * val2 - 7.;
    default :
      f_n = DTchebychevD1 (x, 7);
      f_n_1 = DTchebychevD1 (x, 6);
      return DTchebychev_rec (x, n, 7, &f_n, &f_n_1);
    }
}

/**
 * The terminal recursive function to compute the second derivative of the
 * Tchebychev polynomials of any order. This function is only used for order >
 * 7
 *
 *  @param x the address of a real number
 *  @param n the order of the polynomial to be evaluated
 *  @param n0 7 on input
 *  @param f_n used to store the derivative of the polynomial of order n. On
 *  input is P''(7) 
 *  @param f_n_1 used to store the derivative of  the polynomial of order
 *  n-1. On input is P''(6) 
 */
static double D2Tchebychev_rec (double *x, int n, int n0, double *f_n, double *f_n_1)
{
  if (n == 7)
    {
      return *f_n;
    }
  else
    {
      double save = *f_n;
      *f_n = 2 * (*x) * (*f_n) - (*f_n_1) + 4 * DTchebychevD1 (x, n0);
      *f_n_1 = save;
      return D2Tchebychev_rec (x, n-1, n0+1, f_n, f_n_1);
    }
}

/**
 *  Second derivative of the Tchebytchev polynomials
 *  @param x the address of a real number
 *  @param n the index of the polynom whose second derivative is to be evaluated
 */
static double D2TchebychevD1(double *x, int n)
{
  double val = *x;
  double val2, val3, val4;
  double f_n, f_n_1;
  switch (n)
    {
    case 0 :
      return 0.;
    case 1 :
      return 0.;
    case 2 :
      return 4.;
    case 3 :
      return 24. * val;
    case 4 :
      return (96. * val * val - 16.);
    case 5 :
      val2 = val * val;
      return 320. * val2 * val - 120. * val;
    case 6 :
      val2 = val * val; val4 = val2 * val2;
      return (960. * val4 - 576. * val2 + 36.);
    case 7 :
      val2 = val * val; val3 = val2 * val; val4 = val2 * val2;
      return (2688. * val4 - 2240. * val2 + 336) * val;
    default :
      f_n = D2TchebychevD1 (x, 7);
      f_n_1 = D2TchebychevD1 (x, 6);
      return D2Tchebychev_rec (x, n, 7, &f_n, &f_n_1);
    }
}

/*
 * Interface for the PnlBasis object
 */

enum_member _reg_basis [] =
{
    { "Canonical", CANONICAL},
    { "Hermite", HERMITIAN},
    { "Tchebychev", TCHEBYCHEV},
    { NULL, NULLINT},
};

DEFINE_ENUM(PnlBases, _reg_basis);

/**
 * Returns a  PnlBasis
 *
 * @param index the index of the family to be used
 * @param T the tensor of the multi-dimensionnal basis. No copy of T is done, so
 * do not free T. It will be freed transparently by pnl_basis_free
 * @return a PnlBasis
 */
PnlBasis*  pnl_basis_create_from_tensor (int index, PnlMatInt *T)
{
  PnlBasis *b;
  enum_member *e;

  e = _reg_basis;

  while (e->label != NULL && e->key != index) { e++; }
  if (e->label == NULL )
    {
      printf ("No basis found : index exceeded\n"); abort();
    }

  b = malloc (sizeof (PnlBasis));
  b->nb_func = T->m;
  b->id = index;
  b->label = e->label;
  b->nb_variates = T->n;
  b->T = T;

  switch ( index )
    {
      case CANONICAL:
        b->f = CanonicalD1;
        b->Df = DCanonicalD1;
        b->D2f = D2CanonicalD1;
        break;
      case HERMITIAN:
        b->f = HermiteD1;
        b->Df = DHermiteD1;
        b->D2f = D2HermiteD1;
        break;
      case TCHEBYCHEV:
        b->f = TchebychevD1;
        b->Df = DTchebychevD1;
        b->D2f = D2TchebychevD1;
        break;
      default:
        PNL_ERROR ("unknow basis", "pnl_basis_create");
    }
  return b; 
}

/**
 * Returns a  PnlBasis
 *
 * @param index the index of the family to be used
 * @param nb_func the maximum number of functions which may be used
 * @param nb_variates the size of the space in which the basis functions are
 * defined
 * @return a PnlBasis
 */
PnlBasis*  pnl_basis_create (int index, int nb_func, int nb_variates)
{
  PnlMatInt *T;
  T = compute_tensor (nb_func, nb_variates);
  return pnl_basis_create_from_tensor (index, T);
}

/**
 * Returns a  PnlBasis
 *
 * @param index the index of the family to be used
 * @param degree the maximum total degree of the elements in the basis
 * @param nb_variates the size of the space in which the basis functions are
 * defined
 * @return a PnlBasis
 */
PnlBasis*  pnl_basis_create_from_degree (int index, int degree, int nb_variates)
{
  PnlMatInt *T;
  T = compute_tensor_from_degree (degree, nb_variates);
  return pnl_basis_create_from_tensor (index, T);
}


/*
 * This a deprecated function synonymous of pnl_basis_create.
 * It will removed in the future 
 */
PnlBasis*  pnl_basis_init (int index, int nb_func, int nb_variates)
{
  return pnl_basis_create (index, nb_func, nb_variates);
}

/** 
 * Frees a PnlBasis
 * 
 * @param basis
 */
void pnl_basis_free (PnlBasis **basis)
{
  if (*basis == NULL) return;
  pnl_mat_int_free ( &((*basis)->T) );
  free (*basis); basis = NULL;
}

/** 
 * Evaluates the i-th elements of the basis b at the point x
 * 
 * @param b a PnlBasis
 * @param x a C array containing the coordinates of the point at which to
 * evaluate the basis
 * @param i an integer describing the index of the element of the basis to
 * considier
 * 
 * @return f_i(x) where f is the i-th basis function
 */
double pnl_basis_i ( PnlBasis *b, double *x, int i )
{
  int k; 
  double aux = 1.;
  for ( k=0 ; k<b->nb_variates ; k++ )
    {
      aux *= (b->f)(x+k, PNL_MGET (b->T, i, k));
    }
  return aux;
}

/**
 * Evaluates a linear combination of basis functions at x
 *
 * @param coef a vector typically computed by pnl_basis_fit_ls
 * @param x the coordinates of the point at which to evaluate the function
 * @param basis a PnlBasis
 *
 * @return sum (coef .* f(x))
 */
double pnl_basis_eval (PnlBasis *basis, PnlVect *coef, double *x)
{
  int i;
  double y;

  CHECK_NB_FUNC (coef, basis);
  y = 0;
  for (i=0; i<coef->size; i++)
    {
      y += pnl_vect_get (coef, i) * pnl_basis_i (basis, x, i);
    }
  return y;
}

/**
 * Evaluates the first derivative with respect to x[i] of a linear combination
 * of basis functions at x
 *
 * @param coef a vector typically computed by pnl_basis_fit_ls
 * @param x the coordinates of the point at which to evaluate the function
 * @param basis a PnlBasis
 * @param i the index with respect to which the derivative is computed
 *
 * @return sum (coef .* D_i f(x))
 */
double pnl_basis_eval_D (PnlBasis *basis, PnlVect *coef, double *x, int i)
{
  int k;
  double y;

  CHECK_NB_FUNC (coef, basis);
  y = 0;
  for (k=0; k<coef->size; k++)
    {
      y += pnl_vect_get (coef, k) * D_basis_i (basis, x, k, i);
    }
  return y;
}

/**
 * Evaluates the second derivative with respect to x[i] and x[j] of a linear
 * combination of basis functions at x
 *
 * @param coef a vector typically computed by pnl_basis_fit_ls
 * @param x the coordinates of the point at which to evaluate the function
 * @param basis a PnlBasis
 * @param i the index with respect to which the derivative is computed
 * @param j the index with respect to which the derivative is computed
 *
 * @return sum (coef .* D2_{i,j} f(x))
 */
double pnl_basis_eval_D2 (PnlBasis *basis, PnlVect *coef, double *x, int i, int j)
{
  int k;
  double y;

  CHECK_NB_FUNC (coef, basis);
  y = 0;
  for (k=0; k<coef->size; k++)
    {
      y += pnl_vect_get (coef, k) * D2_basis_i (basis, x, k, i, j);
    }
  return y;
}

/**
 * Finds the best approximation of the function defined by f(x(i,:)) = y(i)
 *
 * @param basis a PnlBasis
 * @param x the matrix of points at which we know the value of the function. One line
 * of the matrix is the vector of the coordinates of one point
 * @param y the values of the function f at the points defined by x
 * @param coef contains on exit the coefficients of the regression
 *
 * @return OK or FAIL
 */
int pnl_basis_fit_ls (PnlBasis *basis, PnlVect *coef, PnlMat *x, PnlVect *y)
{
  int N, i, k;
  double b_k;
  PnlMat *A;
  PnlVect *phi_k;

  N = y->size;
  pnl_vect_resize (coef, basis->nb_func);
  pnl_vect_set_double (coef, 0.);
  phi_k = pnl_vect_create_from_double (basis->nb_func, 0.);
  A = pnl_mat_create_from_double (basis->nb_func, basis->nb_func, 0.);

  /* construct A and b*/
  for ( i=0 ; i<N ; i++ )
    {
      for ( k=0 ; k<basis->nb_func ; k++ )
        {
          double tmp = pnl_basis_i (basis, &(PNL_MGET(x, i, 0)), k);
          b_k =  pnl_vect_get(coef, k);
          b_k += tmp * pnl_vect_get (y, i);
          pnl_vect_set (coef, k, b_k);
          pnl_vect_set (phi_k, k, tmp);
        }
      /* A += phi_k' * phi_k */
      pnl_mat_dger(1., phi_k, phi_k, A);
    }

  /* Solve A x = b, with A >0 symmetric */
  /* pnl_mat_chol (A);
     pnl_mat_chol_syslin_inplace (A, coef); */
  /* Because A often comes from simulation, A is not >0. So we use a QR
     approach */
#ifdef HAVE_LAPACK
  pnl_mat_ls (A, coef);
#else
  pnl_mat_syslin_inplace (A, coef);
#endif
  
  pnl_vect_free (&phi_k);
  pnl_mat_free (&A);

  return OK;
}