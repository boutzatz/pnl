#ifndef _PNL_BASIS_H
#define _PNL_BASIS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "pnl/pnl_types.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_object.h"

/**
 * \defgroup reg_basis Polynomial Bases
 */

/*@{*/

/* basis indices */
enum {CANONICAL, HERMITIAN, TCHEBYCHEV }; 

typedef struct _PnlBasis PnlBasis;

struct _PnlBasis
{
  /** 
   * Must be the first element in order for the object mechanism to work
   * properly. This allows any PnlBasis pointer to be cast to a PnlObject
   */
  PnlObject object; 
  int         id; /*<! basis type */
  const char *label; /*!< string to label the basis */
  int         nb_variates;  /*!< number of variates */
  int         nb_func; /*!< number of elements in the basis */
  PnlMatInt  *T; /*!< Tensor matrix */
  double    (*f)(double    *x, int i); /*!< Computes the i-th element of the one
                                            dimensional basis */
  double    (*Df)(double   *x, int i); /*!< Computes the first derivative of i-th element 
                                            of the one dimensional basis */
  double    (*D2f)(double  *x, int i); /*!< Computes the second derivative of the i-th element 
                                            of the one dimensional basis */
};

extern enum_members PnlBases ;
extern PnlBasis* pnl_basis_new ();
extern PnlBasis* pnl_basis_create ( int index, int nb_func, int space_dim);
extern PnlBasis* pnl_basis_create_from_degree ( int index, int degree, int space_dim);
extern void  pnl_basis_set_from_tensor (PnlBasis *b, int index, PnlMatInt *T);
extern PnlBasis* pnl_basis_create_from_tensor ( int index, PnlMatInt *T);
extern PnlBasis* pnl_basis_init ( int index, int nb_func, int space_dim);
extern void pnl_basis_free (PnlBasis **basis);
extern void pnl_basis_print (const PnlBasis *B);
extern int pnl_basis_fit_ls (PnlBasis *f, PnlVect *coef, PnlMat *x, PnlVect *y);
extern double pnl_basis_i ( PnlBasis *b, double *x, int i );
extern double pnl_basis_eval (PnlBasis *basis, PnlVect *coef, double *x);
extern double pnl_basis_eval_D (PnlBasis *basis, PnlVect *coef, double *x, int i);
extern double pnl_basis_eval_D2 (PnlBasis *basis, PnlVect *coef, double *x, int i, int j);

/*@}*/
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _PNL_BASIS_H */