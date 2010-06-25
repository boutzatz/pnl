
#include "pnl_f2c.h"

/* Subroutine */ int clauu2_(char *uplo, integer *n, complex *a, integer *lda,
	 integer *info)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       September 30, 1994   


    Purpose   
    =======   

    CLAUU2 computes the product U * U' or L' * L, where the triangular   
    factor U or L is stored in the upper or lower triangular part of   
    the array A.   

    If UPLO = 'U' or 'u' then the upper triangle of the result is stored,   
    overwriting the factor U in A.   
    If UPLO = 'L' or 'l' then the lower triangle of the result is stored,   
    overwriting the factor L in A.   

    This is the unblocked form of the algorithm, calling Level 2 BLAS.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies whether the triangular factor stored in the array A   
            is upper or lower triangular:   
            = 'U':  Upper triangular   
            = 'L':  Lower triangular   

    N       (input) INTEGER   
            The order of the triangular factor U or L.  N >= 0.   

    A       (input/output) COMPLEX array, dimension (LDA,N)   
            On entry, the triangular factor U or L.   
            On exit, if UPLO = 'U', the upper triangle of A is   
            overwritten with the upper triangle of the product U * U';   
            if UPLO = 'L', the lower triangle of A is overwritten with   
            the lower triangle of the product L' * L.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,N).   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -k, the k-th argument had an illegal value   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static complex c_b1 = {1.f,0.f};
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;
    complex q__1;
    /* Local variables */
    static integer i__;
    extern /* Complex */ VOID cdotc_(complex *, integer *, complex *, integer 
	    *, complex *, integer *);
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int cgemv_(char *, integer *, integer *, complex *
	    , complex *, integer *, complex *, integer *, complex *, complex *
	    , integer *);
    static logical upper;
    extern /* Subroutine */ int clacgv_(integer *, complex *, integer *), 
	    csscal_(integer *, real *, complex *, integer *), xerbla_(char *, 
	    integer *);
    static real aii;
#define a_subscr(a_1,a_2) (a_2)*a_dim1 + a_1
#define a_ref(a_1,a_2) a[a_subscr(a_1,a_2)]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CLAUU2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (upper) {

/*        Compute the product U * U'. */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = a_subscr(i__, i__);
	    aii = a[i__2].r;
	    if (i__ < *n) {
		i__2 = a_subscr(i__, i__);
		i__3 = *n - i__;
		cdotc_(&q__1, &i__3, &a_ref(i__, i__ + 1), lda, &a_ref(i__, 
			i__ + 1), lda);
		r__1 = aii * aii + q__1.r;
		a[i__2].r = r__1, a[i__2].i = 0.f;
		i__2 = *n - i__;
		clacgv_(&i__2, &a_ref(i__, i__ + 1), lda);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		q__1.r = aii, q__1.i = 0.f;
		cgemv_("No transpose", &i__2, &i__3, &c_b1, &a_ref(1, i__ + 1)
			, lda, &a_ref(i__, i__ + 1), lda, &q__1, &a_ref(1, 
			i__), &c__1);
		i__2 = *n - i__;
		clacgv_(&i__2, &a_ref(i__, i__ + 1), lda);
	    } else {
		csscal_(&i__, &aii, &a_ref(1, i__), &c__1);
	    }
/* L10: */
	}

    } else {

/*        Compute the product L' * L. */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = a_subscr(i__, i__);
	    aii = a[i__2].r;
	    if (i__ < *n) {
		i__2 = a_subscr(i__, i__);
		i__3 = *n - i__;
		cdotc_(&q__1, &i__3, &a_ref(i__ + 1, i__), &c__1, &a_ref(i__ 
			+ 1, i__), &c__1);
		r__1 = aii * aii + q__1.r;
		a[i__2].r = r__1, a[i__2].i = 0.f;
		i__2 = i__ - 1;
		clacgv_(&i__2, &a_ref(i__, 1), lda);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		q__1.r = aii, q__1.i = 0.f;
		cgemv_("Conjugate transpose", &i__2, &i__3, &c_b1, &a_ref(i__ 
			+ 1, 1), lda, &a_ref(i__ + 1, i__), &c__1, &q__1, &
			a_ref(i__, 1), lda);
		i__2 = i__ - 1;
		clacgv_(&i__2, &a_ref(i__, 1), lda);
	    } else {
		csscal_(&i__, &aii, &a_ref(i__, 1), lda);
	    }
/* L20: */
	}
    }

    return 0;

/*     End of CLAUU2 */

} /* clauu2_ */

#undef a_ref
#undef a_subscr


