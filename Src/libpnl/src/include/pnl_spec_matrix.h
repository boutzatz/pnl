#ifndef SPECIAL_MATRIX_H
#define SPECIAL_MATRIX_H

#include "pnl_perm.h"
#include "pnl_matrix.h"
#include "cs.h"

#ifndef PNL_RANGE_CHECK_OFF
#define CheckIndexSparseMat(v,i,j){             \
    if (i>=v->m || j>=v->n || j<0  || i<0 )       \
      {perror("index out of range"); abort();\
      }}
#define CheckSparseMatVectIsCompatible(mat, vect){            \
    if((mat)->m != (vect)->size)                        \
      {perror("non compatible dimensions"); abort();}}
#else
#define CheckIndexSparseMat(v,i,j) {}                          
#define CheckSparseMatMatch(lhs, rhs) {}
#define CheckSparseMatVectIsCompatible(mat, vect){}
#endif /* PNL_RANGE_CHECK_OFF */

/* Morse Matrix, use only for construction 
 * 
 * m,n :  size of matrix 
 * D[i]-> points to the ith column
 * D[i]-> size : number of non nul elements in col i 
 * D[i]->Index : array of rows of the elements of lin i
 * D[i]->Value : array of real part of the elements of lin i
 * D[i]->C : array of imaginary part of the elements of lin i
 */

/* used to store a matlab compatible representation */
typedef struct _sprow  SpRow ;

struct _sprow {
  int size;
  /* size of a row */
  int Max_size;
  /* max size allocation of a row */
  int    *Index;
  /* pointer to an int array giving the columns or row i */
  double *Value;
  /* Pointer on values */
};

/**
 * \defgroup PnlMorseMat Morse Matrix structure for Premia
 */
/*@{*/


typedef struct PnlMorseMat{
  int m; /*!< nb rows */ 
  int n; /*!< nb columns */ 
  SpRow * array; 
  /*!< pointer in each row or col to store no nul coefficients */
  int RC; /*!> 0 if store row, 1 if store col*/ 
} PnlMorseMat;


extern PnlMorseMat * pnl_morse_mat_create(int m, int n,int Max_size_row,int RC);
extern PnlMorseMat * pnl_morse_mat_create_fromfull(PnlMat * FM,int RC);
extern void pnl_morse_mat_free(PnlMorseMat ** M);

extern double pnl_morse_mat_get(PnlMorseMat* M, int i, int j);
extern int pnl_morse_mat_set(PnlMorseMat* M, int i, int j,double Val);
extern double* pnl_morse_mat_lget(PnlMorseMat* M, int i, int j);
extern int pnl_morse_mat_freeze(PnlMorseMat* M);
extern void pnl_morse_mat_mult_vect_inplace(PnlVect *lhs, const PnlMorseMat *M, const PnlVect *rhs);
extern PnlVect* pnl_morse_mat_mult_vect(const PnlMorseMat *M, const PnlVect *vec);
extern void pnl_morse_mat_print (const PnlMorseMat *M);
extern PnlMat * pnl_morse_mat_full(PnlMorseMat * M);

/*@}*/

/**
 * \defgroup PnlSparseMat Sparse Matrix structure for Premia
 *
 * This struct is the cs struct of Csparse librarie
 * written by Timothy A.Davis on LGPL licence.
 *
 * For convenient, we have rename somes functions and struct.
 * we have also reduce numbers of function parameters for non
 * expert in sparse matrix.
 *
 * In following, we use only LU factorization for sparse system
 * Factorization can be stored for PDE-parabolic problem with same
 * operator at each time discretisation step.
 *
 */
/*@{*/

typedef cs PnlSparseMat;

extern PnlSparseMat *pnl_sparse_mat_create_fromfull(PnlMat * M);
extern PnlSparseMat *pnl_sparse_mat_create_frommorse(PnlMorseMat * M);
extern void pnl_sparse_mat_free(PnlSparseMat **M);
extern int pnl_sparse_mat_gaxpby(PnlVect *lhs, const PnlSparseMat *M, const PnlVect *rhs);
extern int pnl_sparse_mat_mult_vect_inplace(PnlVect *lhs, const PnlSparseMat *M, const PnlVect *rhs);
extern void pnl_sparse_mat_print(PnlSparseMat *A);

typedef struct PnlSparseFactorization
{
  css *S ;
  csn *N ;
}PnlSparseFactorization;

extern PnlSparseFactorization * pnl_sparse_factorization_lu_create (const PnlSparseMat*A, double tol);
extern void pnl_sparse_factorization_free(PnlSparseFactorization ** F);
extern void pnl_sparse_factorization_lu_syslin(const PnlSparseFactorization * N, PnlVect *x);

/*@}*/


typedef struct _spwaverow  SpWaveRow ;

struct _spwaverow {
  int level_size;
  SpRow * L_Value;
};

/**
 * \defgroup PnlMorseMat Morse Matrix structure for Premia
 */
/*@{*/

typedef struct PnlMorseWaveMat{
  int m; /*!< nb rows */ 
  int n; /*!< nb columns */ 
  SpWaveRow * array; 
  /*!< pointer in each row and each column level to store no nul coefficients */
} PnlMorseWaveMat;

/*@}*/


typedef struct PnlMorseLUFact
/* LU, factorization */
{
    PnlMorseMat *L ;	    /* L for LU and Cholesky, V for QR */
    PnlMorseMat *U ;	    /* U for LU, R for QR, not used for Cholesky */
    PnlPermutation *pinv;   /* partial pivoting for LU */
}PnlMorseLUFact;

extern PnlMorseLUFact *pnl_morsefact_empty_create(int m,int n,int max_size);
extern PnlMorseLUFact *pnl_morse_lu_fact_create(const PnlMorseMat *A,double tol);
extern void pnl_morse_lu_fact_solve_inplace(const PnlMorseLUFact *F, PnlVect *x);
extern void pnl_morsefact_free(PnlMorseLUFact ** Fatc);

#endif /* SPECIAL_MATRIX_H */


