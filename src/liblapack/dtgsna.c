
#include "pnl_f2c.h"

/* Subroutine */ int dtgsna_(char *job, char *howmny, logical *select, 
	integer *n, doublereal *a, integer *lda, doublereal *b, integer *ldb, 
	doublereal *vl, integer *ldvl, doublereal *vr, integer *ldvr, 
	doublereal *s, doublereal *dif, integer *mm, integer *m, doublereal *
	work, integer *lwork, integer *iwork, integer *info)
{
/*  -- LAPACK routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    DTGSNA estimates reciprocal condition numbers for specified   
    eigenvalues and/or eigenvectors of a matrix pair (A, B) in   
    generalized real Schur canonical form (or of any matrix pair   
    (Q*A*Z', Q*B*Z') with orthogonal matrices Q and Z, where   
    Z' denotes the transpose of Z.   

    (A, B) must be in generalized real Schur form (as returned by DGGES),   
    i.e. A is block upper triangular with 1-by-1 and 2-by-2 diagonal   
    blocks. B is upper triangular.   


    Arguments   
    =========   

    JOB     (input) CHARACTER*1   
            Specifies whether condition numbers are required for   
            eigenvalues (S) or eigenvectors (DIF):   
            = 'E': for eigenvalues only (S);   
            = 'V': for eigenvectors only (DIF);   
            = 'B': for both eigenvalues and eigenvectors (S and DIF).   

    HOWMNY  (input) CHARACTER*1   
            = 'A': compute condition numbers for all eigenpairs;   
            = 'S': compute condition numbers for selected eigenpairs   
                   specified by the array SELECT.   

    SELECT  (input) LOGICAL array, dimension (N)   
            If HOWMNY = 'S', SELECT specifies the eigenpairs for which   
            condition numbers are required. To select condition numbers   
            for the eigenpair corresponding to a real eigenvalue w(j),   
            SELECT(j) must be set to .TRUE.. To select condition numbers   
            corresponding to a complex conjugate pair of eigenvalues w(j)   
            and w(j+1), either SELECT(j) or SELECT(j+1) or both, must be   
            set to .TRUE..   
            If HOWMNY = 'A', SELECT is not referenced.   

    N       (input) INTEGER   
            The order of the square matrix pair (A, B). N >= 0.   

    A       (input) DOUBLE PRECISION array, dimension (LDA,N)   
            The upper quasi-triangular matrix A in the pair (A,B).   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,N).   

    B       (input) DOUBLE PRECISION array, dimension (LDB,N)   
            The upper triangular matrix B in the pair (A,B).   

    LDB     (input) INTEGER   
            The leading dimension of the array B. LDB >= max(1,N).   

    VL      (input) DOUBLE PRECISION array, dimension (LDVL,M)   
            If JOB = 'E' or 'B', VL must contain left eigenvectors of   
            (A, B), corresponding to the eigenpairs specified by HOWMNY   
            and SELECT. The eigenvectors must be stored in consecutive   
            columns of VL, as returned by DTGEVC.   
            If JOB = 'V', VL is not referenced.   

    LDVL    (input) INTEGER   
            The leading dimension of the array VL. LDVL >= 1.   
            If JOB = 'E' or 'B', LDVL >= N.   

    VR      (input) DOUBLE PRECISION array, dimension (LDVR,M)   
            If JOB = 'E' or 'B', VR must contain right eigenvectors of   
            (A, B), corresponding to the eigenpairs specified by HOWMNY   
            and SELECT. The eigenvectors must be stored in consecutive   
            columns ov VR, as returned by DTGEVC.   
            If JOB = 'V', VR is not referenced.   

    LDVR    (input) INTEGER   
            The leading dimension of the array VR. LDVR >= 1.   
            If JOB = 'E' or 'B', LDVR >= N.   

    S       (output) DOUBLE PRECISION array, dimension (MM)   
            If JOB = 'E' or 'B', the reciprocal condition numbers of the   
            selected eigenvalues, stored in consecutive elements of the   
            array. For a complex conjugate pair of eigenvalues two   
            consecutive elements of S are set to the same value. Thus   
            S(j), DIF(j), and the j-th columns of VL and VR all   
            correspond to the same eigenpair (but not in general the   
            j-th eigenpair, unless all eigenpairs are selected).   
            If JOB = 'V', S is not referenced.   

    DIF     (output) DOUBLE PRECISION array, dimension (MM)   
            If JOB = 'V' or 'B', the estimated reciprocal condition   
            numbers of the selected eigenvectors, stored in consecutive   
            elements of the array. For a complex eigenvector two   
            consecutive elements of DIF are set to the same value. If   
            the eigenvalues cannot be reordered to compute DIF(j), DIF(j)   
            is set to 0; this can only occur when the true value would be   
            very small anyway.   
            If JOB = 'E', DIF is not referenced.   

    MM      (input) INTEGER   
            The number of elements in the arrays S and DIF. MM >= M.   

    M       (output) INTEGER   
            The number of elements of the arrays S and DIF used to store   
            the specified condition numbers; for each selected real   
            eigenvalue one element is used, and for each selected complex   
            conjugate pair of eigenvalues, two elements are used.   
            If HOWMNY = 'A', M is set to N.   

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)   
            If JOB = 'E', WORK is not referenced.  Otherwise,   
            on exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= N.   
            If JOB = 'V' or 'B' LWORK >= 2*N*(N+2)+16.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    IWORK   (workspace) INTEGER array, dimension (N + 6)   
            If JOB = 'E', IWORK is not referenced.   

    INFO    (output) INTEGER   
            =0: Successful exit   
            <0: If INFO = -i, the i-th argument had an illegal value   


    Further Details   
    ===============   

    The reciprocal of the condition number of a generalized eigenvalue   
    w = (a, b) is defined as   

         S(w) = (|u'Av|**2 + |u'Bv|**2)**(1/2) / (norm(u)*norm(v))   

    where u and v are the left and right eigenvectors of (A, B)   
    corresponding to w; |z| denotes the absolute value of the complex   
    number, and norm(u) denotes the 2-norm of the vector u.   
    The pair (a, b) corresponds to an eigenvalue w = a/b (= u'Av/u'Bv)   
    of the matrix pair (A, B). If both a and b equal zero, then (A B) is   
    singular and S(I) = -1 is returned.   

    An approximate error bound on the chordal distance between the i-th   
    computed generalized eigenvalue w and the corresponding exact   
    eigenvalue lambda is   

         chord(w, lambda) <= EPS * norm(A, B) / S(I)   

    where EPS is the machine precision.   

    The reciprocal of the condition number DIF(i) of right eigenvector u   
    and left eigenvector v corresponding to the generalized eigenvalue w   
    is defined as follows:   

    a) If the i-th eigenvalue w = (a,b) is real   

       Suppose U and V are orthogonal transformations such that   

                  U'*(A, B)*V  = (S, T) = ( a   *  ) ( b  *  )  1   
                                          ( 0  S22 ),( 0 T22 )  n-1   
                                            1  n-1     1 n-1   

       Then the reciprocal condition number DIF(i) is   

                  Difl((a, b), (S22, T22)) = sigma-min( Zl ),   

       where sigma-min(Zl) denotes the smallest singular value of the   
       2(n-1)-by-2(n-1) matrix   

           Zl = [ kron(a, In-1)  -kron(1, S22) ]   
                [ kron(b, In-1)  -kron(1, T22) ] .   

       Here In-1 is the identity matrix of size n-1. kron(X, Y) is the   
       Kronecker product between the matrices X and Y.   

       Note that if the default method for computing DIF(i) is wanted   
       (see DLATDF), then the parameter DIFDRI (see below) should be   
       changed from 3 to 4 (routine DLATDF(IJOB = 2 will be used)).   
       See DTGSYL for more details.   

    b) If the i-th and (i+1)-th eigenvalues are complex conjugate pair,   

       Suppose U and V are orthogonal transformations such that   

                  U'*(A, B)*V = (S, T) = ( S11  *   ) ( T11  *  )  2   
                                         ( 0    S22 ),( 0    T22) n-2   
                                           2    n-2     2    n-2   

       and (S11, T11) corresponds to the complex conjugate eigenvalue   
       pair (w, conjg(w)). There exist unitary matrices U1 and V1 such   
       that   

           U1'*S11*V1 = ( s11 s12 )   and U1'*T11*V1 = ( t11 t12 )   
                        (  0  s22 )                    (  0  t22 )   

       where the generalized eigenvalues w = s11/t11 and   
       conjg(w) = s22/t22.   

       Then the reciprocal condition number DIF(i) is bounded by   

           min( d1, max( 1, |real(s11)/real(s22)| )*d2 )   

       where, d1 = Difl((s11, t11), (s22, t22)) = sigma-min(Z1), where   
       Z1 is the complex 2-by-2 matrix   

                Z1 =  [ s11  -s22 ]   
                      [ t11  -t22 ],   

       This is done by computing (using real arithmetic) the   
       roots of the characteristical polynomial det(Z1' * Z1 - lambda I),   
       where Z1' denotes the conjugate transpose of Z1 and det(X) denotes   
       the determinant of X.   

       and d2 is an upper bound on Difl((S11, T11), (S22, T22)), i.e. an   
       upper bound on sigma-min(Z2), where Z2 is (2n-2)-by-(2n-2)   

                Z2 = [ kron(S11', In-2)  -kron(I2, S22) ]   
                     [ kron(T11', In-2)  -kron(I2, T22) ]   

       Note that if the default method for computing DIF is wanted (see   
       DLATDF), then the parameter DIFDRI (see below) should be changed   
       from 3 to 4 (routine DLATDF(IJOB = 2 will be used)). See DTGSYL   
       for more details.   

    For each eigenvalue/vector specified by SELECT, DIF stores a   
    Frobenius norm-based estimate of Difl.   

    An approximate error bound for the i-th computed eigenvector VL(i) or   
    VR(i) is given by   

               EPS * norm(A, B) / DIF(i).   

    See ref. [2-3] for more details and further references.   

    Based on contributions by   
       Bo Kagstrom and Peter Poromaa, Department of Computing Science,   
       Umea University, S-901 87 Umea, Sweden.   

    References   
    ==========   

    [1] B. Kagstrom; A Direct Method for Reordering Eigenvalues in the   
        Generalized Real Schur Form of a Regular Matrix Pair (A, B), in   
        M.S. Moonen et al (eds), Linear Algebra for Large Scale and   
        Real-Time Applications, Kluwer Academic Publ. 1993, pp 195-218.   

    [2] B. Kagstrom and P. Poromaa; Computing Eigenspaces with Specified   
        Eigenvalues of a Regular Matrix Pair (A, B) and Condition   
        Estimation: Theory, Algorithms and Software,   
        Report UMINF - 94.04, Department of Computing Science, Umea   
        University, S-901 87 Umea, Sweden, 1994. Also as LAPACK Working   
        Note 87. To appear in Numerical Algorithms, 1996.   

    [3] B. Kagstrom and P. Poromaa, LAPACK-Style Algorithms and Software   
        for Solving the Generalized Sylvester Equation and Estimating the   
        Separation between Regular Matrix Pairs, Report UMINF - 93.23,   
        Department of Computing Science, Umea University, S-901 87 Umea,   
        Sweden, December 1993, Revised April 1994, Also as LAPACK Working   
        Note 75.  To appear in ACM Trans. on Math. Software, Vol 22,   
        No 1, 1996.   

    =====================================================================   


       Decode and test the input parameters   

       Parameter adjustments */
    /* Table of constant values */
    static integer c__1 = 1;
    static doublereal c_b19 = 1.;
    static doublereal c_b21 = 0.;
    static integer c__2 = 2;
    static logical c_false = FALSE_;
    static integer c__3 = 3;
    
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1, 
	    vr_offset, i__1, i__2;
    doublereal d__1, d__2;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static doublereal beta, cond;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    static logical pair;
    static integer ierr;
    static doublereal uhav, uhbv;
    static integer ifst;
    static doublereal lnrm;
    static integer ilst;
    static doublereal rnrm;
    extern /* Subroutine */ int dlag2_(doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *);
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    static doublereal root1, root2;
    static integer i__, k;
    static doublereal scale;
    extern logical lsame_(char *, char *);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *);
    static doublereal uhavi, uhbvi, tmpii, c1, c2;
    static integer lwmin;
    static logical wants;
    static doublereal tmpir;
    static integer n1, n2;
    static doublereal tmpri, dummy[1], tmprr;
    extern doublereal dlapy2_(doublereal *, doublereal *);
    static doublereal dummy1[1];
    extern doublereal dlamch_(char *);
    static integer ks;
    static doublereal alphai;
    static integer iz;
    static doublereal alphar;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    xerbla_(char *, integer *), dtgexc_(logical *, logical *, 
	    integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, integer *, integer *);
    static logical wantbh, wantdf, somcon;
    static doublereal alprqt;
    extern /* Subroutine */ int dtgsyl_(char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *, integer *);
    static doublereal smlnum;
    static logical lquery;
    static doublereal eps;
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define vl_ref(a_1,a_2) vl[(a_2)*vl_dim1 + a_1]
#define vr_ref(a_1,a_2) vr[(a_2)*vr_dim1 + a_1]


    --select;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1 * 1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1 * 1;
    vr -= vr_offset;
    --s;
    --dif;
    --work;
    --iwork;

    /* Function Body */
    wantbh = lsame_(job, "B");
    wants = lsame_(job, "E") || wantbh;
    wantdf = lsame_(job, "V") || wantbh;

    somcon = lsame_(howmny, "S");

    *info = 0;
    lquery = *lwork == -1;

    if (lsame_(job, "V") || lsame_(job, "B")) {
/* Computing MAX */
	i__1 = 1, i__2 = (*n << 1) * (*n + 2) + 16;
	lwmin = max(i__1,i__2);
    } else {
	lwmin = 1;
    }

    if (! wants && ! wantdf) {
	*info = -1;
    } else if (! lsame_(howmny, "A") && ! somcon) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < max(1,*n)) {
	*info = -6;
    } else if (*ldb < max(1,*n)) {
	*info = -8;
    } else if (wants && *ldvl < *n) {
	*info = -10;
    } else if (wants && *ldvr < *n) {
	*info = -12;
    } else {

/*        Set M to the number of eigenpairs for which condition numbers   
          are required, and test MM. */

	if (somcon) {
	    *m = 0;
	    pair = FALSE_;
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		if (pair) {
		    pair = FALSE_;
		} else {
		    if (k < *n) {
			if (a_ref(k + 1, k) == 0.) {
			    if (select[k]) {
				++(*m);
			    }
			} else {
			    pair = TRUE_;
			    if (select[k] || select[k + 1]) {
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*mm < *m) {
	    *info = -15;
	} else if (*lwork < lwmin && ! lquery) {
	    *info = -18;
/*        ELSE IF( WANTDF .AND. LWORK.LT.2*N*( N+2 )+16 ) THEN   
             INFO = -18 */
	}
    }

    if (*info == 0) {
	work[1] = (doublereal) lwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSNA", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    ks = 0;
    pair = FALSE_;

    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {

/*        Determine whether A(k,k) begins a 1-by-1 or 2-by-2 block. */

	if (pair) {
	    pair = FALSE_;
	    goto L20;
	} else {
	    if (k < *n) {
		pair = a_ref(k + 1, k) != 0.;
	    }
	}

/*        Determine whether condition numbers are required for the k-th   
          eigenpair. */

	if (somcon) {
	    if (pair) {
		if (! select[k] && ! select[k + 1]) {
		    goto L20;
		}
	    } else {
		if (! select[k]) {
		    goto L20;
		}
	    }
	}

	++ks;

	if (wants) {

/*           Compute the reciprocal condition number of the k-th   
             eigenvalue. */

	    if (pair) {

/*              Complex eigenvalue pair. */

		d__1 = dnrm2_(n, &vr_ref(1, ks), &c__1);
		d__2 = dnrm2_(n, &vr_ref(1, ks + 1), &c__1);
		rnrm = dlapy2_(&d__1, &d__2);
		d__1 = dnrm2_(n, &vl_ref(1, ks), &c__1);
		d__2 = dnrm2_(n, &vl_ref(1, ks + 1), &c__1);
		lnrm = dlapy2_(&d__1, &d__2);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr_ref(1, ks), &
			c__1, &c_b21, &work[1], &c__1);
		tmprr = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		tmpri = ddot_(n, &work[1], &c__1, &vl_ref(1, ks + 1), &c__1);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr_ref(1, ks + 
			1), &c__1, &c_b21, &work[1], &c__1);
		tmpii = ddot_(n, &work[1], &c__1, &vl_ref(1, ks + 1), &c__1);
		tmpir = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		uhav = tmprr + tmpii;
		uhavi = tmpir - tmpri;
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr_ref(1, ks), &
			c__1, &c_b21, &work[1], &c__1);
		tmprr = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		tmpri = ddot_(n, &work[1], &c__1, &vl_ref(1, ks + 1), &c__1);
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr_ref(1, ks + 
			1), &c__1, &c_b21, &work[1], &c__1);
		tmpii = ddot_(n, &work[1], &c__1, &vl_ref(1, ks + 1), &c__1);
		tmpir = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		uhbv = tmprr + tmpii;
		uhbvi = tmpir - tmpri;
		uhav = dlapy2_(&uhav, &uhavi);
		uhbv = dlapy2_(&uhbv, &uhbvi);
		cond = dlapy2_(&uhav, &uhbv);
		s[ks] = cond / (rnrm * lnrm);
		s[ks + 1] = s[ks];

	    } else {

/*              Real eigenvalue. */

		rnrm = dnrm2_(n, &vr_ref(1, ks), &c__1);
		lnrm = dnrm2_(n, &vl_ref(1, ks), &c__1);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr_ref(1, ks), &
			c__1, &c_b21, &work[1], &c__1);
		uhav = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr_ref(1, ks), &
			c__1, &c_b21, &work[1], &c__1);
		uhbv = ddot_(n, &work[1], &c__1, &vl_ref(1, ks), &c__1);
		cond = dlapy2_(&uhav, &uhbv);
		if (cond == 0.) {
		    s[ks] = -1.;
		} else {
		    s[ks] = cond / (rnrm * lnrm);
		}
	    }
	}

	if (wantdf) {
	    if (*n == 1) {
		dif[ks] = dlapy2_(&a_ref(1, 1), &b_ref(1, 1));
		goto L20;
	    }

/*           Estimate the reciprocal condition number of the k-th   
             eigenvectors. */
	    if (pair) {

/*              Copy the  2-by 2 pencil beginning at (A(k,k), B(k, k)).   
                Compute the eigenvalue(s) at position K. */

		work[1] = a_ref(k, k);
		work[2] = a_ref(k + 1, k);
		work[3] = a_ref(k, k + 1);
		work[4] = a_ref(k + 1, k + 1);
		work[5] = b_ref(k, k);
		work[6] = b_ref(k + 1, k);
		work[7] = b_ref(k, k + 1);
		work[8] = b_ref(k + 1, k + 1);
		d__1 = smlnum * eps;
		dlag2_(&work[1], &c__2, &work[5], &c__2, &d__1, &beta, dummy1,
			 &alphar, dummy, &alphai);
		alprqt = 1.;
		c1 = (alphar * alphar + alphai * alphai + beta * beta) * 2.;
		c2 = beta * 4. * beta * alphai * alphai;
		root1 = c1 + sqrt(c1 * c1 - c2 * 4.);
		root2 = c2 / root1;
		root1 /= 2.;
/* Computing MIN */
		d__1 = sqrt(root1), d__2 = sqrt(root2);
		cond = min(d__1,d__2);
	    }

/*           Copy the matrix (A, B) to the array WORK and swap the   
             diagonal block beginning at A(k,k) to the (1,1) position. */

	    dlacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
	    dlacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
	    ifst = k;
	    ilst = 1;

	    i__2 = *lwork - (*n << 1) * *n;
	    dtgexc_(&c_false, &c_false, n, &work[1], n, &work[*n * *n + 1], n,
		     dummy, &c__1, dummy1, &c__1, &ifst, &ilst, &work[(*n * *
		    n << 1) + 1], &i__2, &ierr);

	    if (ierr > 0) {

/*              Ill-conditioned problem - swap rejected. */

		dif[ks] = 0.;
	    } else {

/*              Reordering successful, solve generalized Sylvester   
                equation for R and L,   
                           A22 * R - L * A11 = A12   
                           B22 * R - L * B11 = B12,   
                and compute estimate of Difl((A11,B11), (A22, B22)). */

		n1 = 1;
		if (work[2] != 0.) {
		    n1 = 2;
		}
		n2 = *n - n1;
		if (n2 == 0) {
		    dif[ks] = cond;
		} else {
		    i__ = *n * *n + 1;
		    iz = (*n << 1) * *n + 1;
		    i__2 = *lwork - (*n << 1) * *n;
		    dtgsyl_("N", &c__3, &n2, &n1, &work[*n * n1 + n1 + 1], n, 
			    &work[1], n, &work[n1 + 1], n, &work[*n * n1 + n1 
			    + i__], n, &work[i__], n, &work[n1 + i__], n, &
			    scale, &dif[ks], &work[iz + 1], &i__2, &iwork[1], 
			    &ierr);

		    if (pair) {
/* Computing MIN */
			d__1 = max(1.,alprqt) * dif[ks];
			dif[ks] = min(d__1,cond);
		    }
		}
	    }
	    if (pair) {
		dif[ks + 1] = dif[ks];
	    }
	}
	if (pair) {
	    ++ks;
	}

L20:
	;
    }
    work[1] = (doublereal) lwmin;
    return 0;

/*     End of DTGSNA */

} /* dtgsna_ */

#undef vr_ref
#undef vl_ref
#undef b_ref
#undef a_ref


