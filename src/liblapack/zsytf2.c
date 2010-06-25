
#include "pnl_f2c.h"

/* Subroutine */ int zsytf2_(char *uplo, integer *n, doublecomplex *a, 
	integer *lda, integer *ipiv, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    ZSYTF2 computes the factorization of a complex symmetric matrix A   
    using the Bunch-Kaufman diagonal pivoting method:   

       A = U*D*U'  or  A = L*D*L'   

    where U (or L) is a product of permutation and unit upper (lower)   
    triangular matrices, U' is the transpose of U, and D is symmetric and   
    block diagonal with 1-by-1 and 2-by-2 diagonal blocks.   

    This is the unblocked version of the algorithm, calling Level 2 BLAS.   

    Arguments   
    =========   

    UPLO    (input) CHARACTER*1   
            Specifies whether the upper or lower triangular part of the   
            symmetric matrix A is stored:   
            = 'U':  Upper triangular   
            = 'L':  Lower triangular   

    N       (input) INTEGER   
            The order of the matrix A.  N >= 0.   

    A       (input/output) COMPLEX*16 array, dimension (LDA,N)   
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading   
            n-by-n upper triangular part of A contains the upper   
            triangular part of the matrix A, and the strictly lower   
            triangular part of A is not referenced.  If UPLO = 'L', the   
            leading n-by-n lower triangular part of A contains the lower   
            triangular part of the matrix A, and the strictly upper   
            triangular part of A is not referenced.   

            On exit, the block diagonal matrix D and the multipliers used   
            to obtain the factor U or L (see below for further details).   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,N).   

    IPIV    (output) INTEGER array, dimension (N)   
            Details of the interchanges and the block structure of D.   
            If IPIV(k) > 0, then rows and columns k and IPIV(k) were   
            interchanged and D(k,k) is a 1-by-1 diagonal block.   
            If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and   
            columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k)   
            is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) =   
            IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were   
            interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block.   

    INFO    (output) INTEGER   
            = 0: successful exit   
            < 0: if INFO = -k, the k-th argument had an illegal value   
            > 0: if INFO = k, D(k,k) is exactly zero.  The factorization   
                 has been completed, but the block diagonal matrix D is   
                 exactly singular, and division by zero will occur if it   
                 is used to solve a system of equations.   

    Further Details   
    ===============   

    1-96 - Based on modifications by J. Lewis, Boeing Computer Services   
           Company   

    If UPLO = 'U', then A = U*D*U', where   
       U = P(n)*U(n)* ... *P(k)U(k)* ...,   
    i.e., U is a product of terms P(k)*U(k), where k decreases from n to   
    1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1   
    and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as   
    defined by IPIV(k), and U(k) is a unit upper triangular matrix, such   
    that if the diagonal block D(k) is of order s (s = 1 or 2), then   

               (   I    v    0   )   k-s   
       U(k) =  (   0    I    0   )   s   
               (   0    0    I   )   n-k   
                  k-s   s   n-k   

    If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k).   
    If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k),   
    and A(k,k), and v overwrites A(1:k-2,k-1:k).   

    If UPLO = 'L', then A = L*D*L', where   
       L = P(1)*L(1)* ... *P(k)*L(k)* ...,   
    i.e., L is a product of terms P(k)*L(k), where k increases from 1 to   
    n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1   
    and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as   
    defined by IPIV(k), and L(k) is a unit lower triangular matrix, such   
    that if the diagonal block D(k) is of order s (s = 1 or 2), then   

               (   I    0     0   )  k-1   
       L(k) =  (   0    I     0   )  s   
               (   0    v     I   )  n-k-s+1   
                  k-1   s  n-k-s+1   

    If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k).   
    If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k),   
    and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1).   

    =====================================================================   


       Test the input parameters.   

       Parameter adjustments */
    /* Table of constant values */
    static doublecomplex c_b1 = {1.,0.};
    static integer c__1 = 1;
    
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    doublereal d__1, d__2, d__3, d__4;
    doublecomplex z__1, z__2, z__3, z__4;
    /* Builtin functions */
    double sqrt(doublereal), d_imag(doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    /* Local variables */
    static integer imax, jmax;
    extern /* Subroutine */ int zsyr_(char *, integer *, doublecomplex *, 
	    doublecomplex *, integer *, doublecomplex *, integer *);
    static integer i__, j, k;
    static doublecomplex t;
    static doublereal alpha;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int zscal_(integer *, doublecomplex *, 
	    doublecomplex *, integer *);
    static integer kstep;
    static logical upper;
    static doublecomplex r1;
    extern /* Subroutine */ int zswap_(integer *, doublecomplex *, integer *, 
	    doublecomplex *, integer *);
    static doublecomplex d11, d12, d21, d22;
    static integer kk, kp;
    static doublereal absakk;
    static doublecomplex wk;
    extern /* Subroutine */ int xerbla_(char *, integer *);
    static doublereal colmax;
    extern integer izamax_(integer *, doublecomplex *, integer *);
    static doublereal rowmax;
    static doublecomplex wkm1, wkp1;
#define a_subscr(a_1,a_2) (a_2)*a_dim1 + a_1
#define a_ref(a_1,a_2) a[a_subscr(a_1,a_2)]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    --ipiv;

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
	xerbla_("ZSYTF2", &i__1);
	return 0;
    }

/*     Initialize ALPHA for use in choosing pivot block size. */

    alpha = (sqrt(17.) + 1.) / 8.;

    if (upper) {

/*        Factorize A as U*D*U' using the upper triangle of A   

          K is the main loop index, decreasing from N to 1 in steps of   
          1 or 2 */

	k = *n;
L10:

/*        If K < 1, exit from loop */

	if (k < 1) {
	    goto L70;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether   
          a 1-by-1 or 2-by-2 pivot block will be used */

	i__1 = a_subscr(k, k);
	absakk = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(k, k)),
		 abs(d__2));

/*        IMAX is the row-index of the largest off-diagonal element in   
          column K, and COLMAX is its absolute value */

	if (k > 1) {
	    i__1 = k - 1;
	    imax = izamax_(&i__1, &a_ref(1, k), &c__1);
	    i__1 = a_subscr(imax, k);
	    colmax = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(
		    imax, k)), abs(d__2));
	} else {
	    colmax = 0.;
	}

	if (max(absakk,colmax) == 0.) {

/*           Column K is zero: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              JMAX is the column-index of the largest off-diagonal   
                element in row IMAX, and ROWMAX is its absolute value */

		i__1 = k - imax;
		jmax = imax + izamax_(&i__1, &a_ref(imax, imax + 1), lda);
		i__1 = a_subscr(imax, jmax);
		rowmax = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&
			a_ref(imax, jmax)), abs(d__2));
		if (imax > 1) {
		    i__1 = imax - 1;
		    jmax = izamax_(&i__1, &a_ref(1, imax), &c__1);
/* Computing MAX */
		    i__1 = a_subscr(jmax, imax);
		    d__3 = rowmax, d__4 = (d__1 = a[i__1].r, abs(d__1)) + (
			    d__2 = d_imag(&a_ref(jmax, imax)), abs(d__2));
		    rowmax = max(d__3,d__4);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else /* if(complicated condition) */ {
		    i__1 = a_subscr(imax, imax);
		    if ((d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(
			    imax, imax)), abs(d__2)) >= alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1   
                   pivot block */

			kp = imax;
		    } else {

/*                 interchange rows and columns K-1 and IMAX, use 2-by-2   
                   pivot block */

			kp = imax;
			kstep = 2;
		    }
		}
	    }

	    kk = k - kstep + 1;
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the leading   
                submatrix A(1:k,1:k) */

		i__1 = kp - 1;
		zswap_(&i__1, &a_ref(1, kk), &c__1, &a_ref(1, kp), &c__1);
		i__1 = kk - kp - 1;
		zswap_(&i__1, &a_ref(kp + 1, kk), &c__1, &a_ref(kp, kp + 1), 
			lda);
		i__1 = a_subscr(kk, kk);
		t.r = a[i__1].r, t.i = a[i__1].i;
		i__1 = a_subscr(kk, kk);
		i__2 = a_subscr(kp, kp);
		a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
		i__1 = a_subscr(kp, kp);
		a[i__1].r = t.r, a[i__1].i = t.i;
		if (kstep == 2) {
		    i__1 = a_subscr(k - 1, k);
		    t.r = a[i__1].r, t.i = a[i__1].i;
		    i__1 = a_subscr(k - 1, k);
		    i__2 = a_subscr(kp, k);
		    a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
		    i__1 = a_subscr(kp, k);
		    a[i__1].r = t.r, a[i__1].i = t.i;
		}
	    }

/*           Update the leading submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds   

                W(k) = U(k)*D(k)   

                where U(k) is the k-th column of U   

                Perform a rank-1 update of A(1:k-1,1:k-1) as   

                A := A - U(k)*D(k)*U(k)' = A - W(k)*1/D(k)*W(k)' */

		z_div(&z__1, &c_b1, &a_ref(k, k));
		r1.r = z__1.r, r1.i = z__1.i;
		i__1 = k - 1;
		z__1.r = -r1.r, z__1.i = -r1.i;
		zsyr_(uplo, &i__1, &z__1, &a_ref(1, k), &c__1, &a[a_offset], 
			lda);

/*              Store U(k) in column k */

		i__1 = k - 1;
		zscal_(&i__1, &r1, &a_ref(1, k), &c__1);
	    } else {

/*              2-by-2 pivot block D(k): columns k and k-1 now hold   

                ( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k)   

                where U(k) and U(k-1) are the k-th and (k-1)-th columns   
                of U   

                Perform a rank-2 update of A(1:k-2,1:k-2) as   

                A := A - ( U(k-1) U(k) )*D(k)*( U(k-1) U(k) )'   
                   = A - ( W(k-1) W(k) )*inv(D(k))*( W(k-1) W(k) )' */

		if (k > 2) {

		    i__1 = a_subscr(k - 1, k);
		    d12.r = a[i__1].r, d12.i = a[i__1].i;
		    z_div(&z__1, &a_ref(k - 1, k - 1), &d12);
		    d22.r = z__1.r, d22.i = z__1.i;
		    z_div(&z__1, &a_ref(k, k), &d12);
		    d11.r = z__1.r, d11.i = z__1.i;
		    z__3.r = d11.r * d22.r - d11.i * d22.i, z__3.i = d11.r * 
			    d22.i + d11.i * d22.r;
		    z__2.r = z__3.r - 1., z__2.i = z__3.i + 0.;
		    z_div(&z__1, &c_b1, &z__2);
		    t.r = z__1.r, t.i = z__1.i;
		    z_div(&z__1, &t, &d12);
		    d12.r = z__1.r, d12.i = z__1.i;

		    for (j = k - 2; j >= 1; --j) {
			i__1 = a_subscr(j, k - 1);
			z__3.r = d11.r * a[i__1].r - d11.i * a[i__1].i, 
				z__3.i = d11.r * a[i__1].i + d11.i * a[i__1]
				.r;
			i__2 = a_subscr(j, k);
			z__2.r = z__3.r - a[i__2].r, z__2.i = z__3.i - a[i__2]
				.i;
			z__1.r = d12.r * z__2.r - d12.i * z__2.i, z__1.i = 
				d12.r * z__2.i + d12.i * z__2.r;
			wkm1.r = z__1.r, wkm1.i = z__1.i;
			i__1 = a_subscr(j, k);
			z__3.r = d22.r * a[i__1].r - d22.i * a[i__1].i, 
				z__3.i = d22.r * a[i__1].i + d22.i * a[i__1]
				.r;
			i__2 = a_subscr(j, k - 1);
			z__2.r = z__3.r - a[i__2].r, z__2.i = z__3.i - a[i__2]
				.i;
			z__1.r = d12.r * z__2.r - d12.i * z__2.i, z__1.i = 
				d12.r * z__2.i + d12.i * z__2.r;
			wk.r = z__1.r, wk.i = z__1.i;
			for (i__ = j; i__ >= 1; --i__) {
			    i__1 = a_subscr(i__, j);
			    i__2 = a_subscr(i__, j);
			    i__3 = a_subscr(i__, k);
			    z__3.r = a[i__3].r * wk.r - a[i__3].i * wk.i, 
				    z__3.i = a[i__3].r * wk.i + a[i__3].i * 
				    wk.r;
			    z__2.r = a[i__2].r - z__3.r, z__2.i = a[i__2].i - 
				    z__3.i;
			    i__4 = a_subscr(i__, k - 1);
			    z__4.r = a[i__4].r * wkm1.r - a[i__4].i * wkm1.i, 
				    z__4.i = a[i__4].r * wkm1.i + a[i__4].i * 
				    wkm1.r;
			    z__1.r = z__2.r - z__4.r, z__1.i = z__2.i - 
				    z__4.i;
			    a[i__1].r = z__1.r, a[i__1].i = z__1.i;
/* L20: */
			}
			i__1 = a_subscr(j, k);
			a[i__1].r = wk.r, a[i__1].i = wk.i;
			i__1 = a_subscr(j, k - 1);
			a[i__1].r = wkm1.r, a[i__1].i = wkm1.i;
/* L30: */
		    }

		}

	    }
	}

/*        Store details of the interchanges in IPIV */

	if (kstep == 1) {
	    ipiv[k] = kp;
	} else {
	    ipiv[k] = -kp;
	    ipiv[k - 1] = -kp;
	}

/*        Decrease K and return to the start of the main loop */

	k -= kstep;
	goto L10;

    } else {

/*        Factorize A as L*D*L' using the lower triangle of A   

          K is the main loop index, increasing from 1 to N in steps of   
          1 or 2 */

	k = 1;
L40:

/*        If K > N, exit from loop */

	if (k > *n) {
	    goto L70;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether   
          a 1-by-1 or 2-by-2 pivot block will be used */

	i__1 = a_subscr(k, k);
	absakk = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(k, k)),
		 abs(d__2));

/*        IMAX is the row-index of the largest off-diagonal element in   
          column K, and COLMAX is its absolute value */

	if (k < *n) {
	    i__1 = *n - k;
	    imax = k + izamax_(&i__1, &a_ref(k + 1, k), &c__1);
	    i__1 = a_subscr(imax, k);
	    colmax = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(
		    imax, k)), abs(d__2));
	} else {
	    colmax = 0.;
	}

	if (max(absakk,colmax) == 0.) {

/*           Column K is zero: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              JMAX is the column-index of the largest off-diagonal   
                element in row IMAX, and ROWMAX is its absolute value */

		i__1 = imax - k;
		jmax = k - 1 + izamax_(&i__1, &a_ref(imax, k), lda);
		i__1 = a_subscr(imax, jmax);
		rowmax = (d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&
			a_ref(imax, jmax)), abs(d__2));
		if (imax < *n) {
		    i__1 = *n - imax;
		    jmax = imax + izamax_(&i__1, &a_ref(imax + 1, imax), &
			    c__1);
/* Computing MAX */
		    i__1 = a_subscr(jmax, imax);
		    d__3 = rowmax, d__4 = (d__1 = a[i__1].r, abs(d__1)) + (
			    d__2 = d_imag(&a_ref(jmax, imax)), abs(d__2));
		    rowmax = max(d__3,d__4);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else /* if(complicated condition) */ {
		    i__1 = a_subscr(imax, imax);
		    if ((d__1 = a[i__1].r, abs(d__1)) + (d__2 = d_imag(&a_ref(
			    imax, imax)), abs(d__2)) >= alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1   
                   pivot block */

			kp = imax;
		    } else {

/*                 interchange rows and columns K+1 and IMAX, use 2-by-2   
                   pivot block */

			kp = imax;
			kstep = 2;
		    }
		}
	    }

	    kk = k + kstep - 1;
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the trailing   
                submatrix A(k:n,k:n) */

		if (kp < *n) {
		    i__1 = *n - kp;
		    zswap_(&i__1, &a_ref(kp + 1, kk), &c__1, &a_ref(kp + 1, 
			    kp), &c__1);
		}
		i__1 = kp - kk - 1;
		zswap_(&i__1, &a_ref(kk + 1, kk), &c__1, &a_ref(kp, kk + 1), 
			lda);
		i__1 = a_subscr(kk, kk);
		t.r = a[i__1].r, t.i = a[i__1].i;
		i__1 = a_subscr(kk, kk);
		i__2 = a_subscr(kp, kp);
		a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
		i__1 = a_subscr(kp, kp);
		a[i__1].r = t.r, a[i__1].i = t.i;
		if (kstep == 2) {
		    i__1 = a_subscr(k + 1, k);
		    t.r = a[i__1].r, t.i = a[i__1].i;
		    i__1 = a_subscr(k + 1, k);
		    i__2 = a_subscr(kp, k);
		    a[i__1].r = a[i__2].r, a[i__1].i = a[i__2].i;
		    i__1 = a_subscr(kp, k);
		    a[i__1].r = t.r, a[i__1].i = t.i;
		}
	    }

/*           Update the trailing submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds   

                W(k) = L(k)*D(k)   

                where L(k) is the k-th column of L */

		if (k < *n) {

/*                 Perform a rank-1 update of A(k+1:n,k+1:n) as   

                   A := A - L(k)*D(k)*L(k)' = A - W(k)*(1/D(k))*W(k)' */

		    z_div(&z__1, &c_b1, &a_ref(k, k));
		    r1.r = z__1.r, r1.i = z__1.i;
		    i__1 = *n - k;
		    z__1.r = -r1.r, z__1.i = -r1.i;
		    zsyr_(uplo, &i__1, &z__1, &a_ref(k + 1, k), &c__1, &a_ref(
			    k + 1, k + 1), lda);

/*                 Store L(k) in column K */

		    i__1 = *n - k;
		    zscal_(&i__1, &r1, &a_ref(k + 1, k), &c__1);
		}
	    } else {

/*              2-by-2 pivot block D(k) */

		if (k < *n - 1) {

/*                 Perform a rank-2 update of A(k+2:n,k+2:n) as   

                   A := A - ( L(k) L(k+1) )*D(k)*( L(k) L(k+1) )'   
                      = A - ( W(k) W(k+1) )*inv(D(k))*( W(k) W(k+1) )'   

                   where L(k) and L(k+1) are the k-th and (k+1)-th   
                   columns of L */

		    i__1 = a_subscr(k + 1, k);
		    d21.r = a[i__1].r, d21.i = a[i__1].i;
		    z_div(&z__1, &a_ref(k + 1, k + 1), &d21);
		    d11.r = z__1.r, d11.i = z__1.i;
		    z_div(&z__1, &a_ref(k, k), &d21);
		    d22.r = z__1.r, d22.i = z__1.i;
		    z__3.r = d11.r * d22.r - d11.i * d22.i, z__3.i = d11.r * 
			    d22.i + d11.i * d22.r;
		    z__2.r = z__3.r - 1., z__2.i = z__3.i + 0.;
		    z_div(&z__1, &c_b1, &z__2);
		    t.r = z__1.r, t.i = z__1.i;
		    z_div(&z__1, &t, &d21);
		    d21.r = z__1.r, d21.i = z__1.i;

		    i__1 = *n;
		    for (j = k + 2; j <= i__1; ++j) {
			i__2 = a_subscr(j, k);
			z__3.r = d11.r * a[i__2].r - d11.i * a[i__2].i, 
				z__3.i = d11.r * a[i__2].i + d11.i * a[i__2]
				.r;
			i__3 = a_subscr(j, k + 1);
			z__2.r = z__3.r - a[i__3].r, z__2.i = z__3.i - a[i__3]
				.i;
			z__1.r = d21.r * z__2.r - d21.i * z__2.i, z__1.i = 
				d21.r * z__2.i + d21.i * z__2.r;
			wk.r = z__1.r, wk.i = z__1.i;
			i__2 = a_subscr(j, k + 1);
			z__3.r = d22.r * a[i__2].r - d22.i * a[i__2].i, 
				z__3.i = d22.r * a[i__2].i + d22.i * a[i__2]
				.r;
			i__3 = a_subscr(j, k);
			z__2.r = z__3.r - a[i__3].r, z__2.i = z__3.i - a[i__3]
				.i;
			z__1.r = d21.r * z__2.r - d21.i * z__2.i, z__1.i = 
				d21.r * z__2.i + d21.i * z__2.r;
			wkp1.r = z__1.r, wkp1.i = z__1.i;
			i__2 = *n;
			for (i__ = j; i__ <= i__2; ++i__) {
			    i__3 = a_subscr(i__, j);
			    i__4 = a_subscr(i__, j);
			    i__5 = a_subscr(i__, k);
			    z__3.r = a[i__5].r * wk.r - a[i__5].i * wk.i, 
				    z__3.i = a[i__5].r * wk.i + a[i__5].i * 
				    wk.r;
			    z__2.r = a[i__4].r - z__3.r, z__2.i = a[i__4].i - 
				    z__3.i;
			    i__6 = a_subscr(i__, k + 1);
			    z__4.r = a[i__6].r * wkp1.r - a[i__6].i * wkp1.i, 
				    z__4.i = a[i__6].r * wkp1.i + a[i__6].i * 
				    wkp1.r;
			    z__1.r = z__2.r - z__4.r, z__1.i = z__2.i - 
				    z__4.i;
			    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
/* L50: */
			}
			i__2 = a_subscr(j, k);
			a[i__2].r = wk.r, a[i__2].i = wk.i;
			i__2 = a_subscr(j, k + 1);
			a[i__2].r = wkp1.r, a[i__2].i = wkp1.i;
/* L60: */
		    }
		}
	    }
	}

/*        Store details of the interchanges in IPIV */

	if (kstep == 1) {
	    ipiv[k] = kp;
	} else {
	    ipiv[k] = -kp;
	    ipiv[k + 1] = -kp;
	}

/*        Increase K and return to the start of the main loop */

	k += kstep;
	goto L40;

    }

L70:
    return 0;

/*     End of ZSYTF2 */

} /* zsytf2_ */

#undef a_ref
#undef a_subscr


