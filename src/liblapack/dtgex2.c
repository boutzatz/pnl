
#include "pnl_f2c.h"

/* Subroutine */ int dtgex2_(logical *wantq, logical *wantz, integer *n, 
	doublereal *a, integer *lda, doublereal *b, integer *ldb, doublereal *
	q, integer *ldq, doublereal *z__, integer *ldz, integer *j1, integer *
	n1, integer *n2, doublereal *work, integer *lwork, integer *info)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    DTGEX2 swaps adjacent diagonal blocks (A11, B11) and (A22, B22)   
    of size 1-by-1 or 2-by-2 in an upper (quasi) triangular matrix pair   
    (A, B) by an orthogonal equivalence transformation.   

    (A, B) must be in generalized real Schur canonical form (as returned   
    by DGGES), i.e. A is block upper triangular with 1-by-1 and 2-by-2   
    diagonal blocks. B is upper triangular.   

    Optionally, the matrices Q and Z of generalized Schur vectors are   
    updated.   

           Q(in) * A(in) * Z(in)' = Q(out) * A(out) * Z(out)'   
           Q(in) * B(in) * Z(in)' = Q(out) * B(out) * Z(out)'   


    Arguments   
    =========   

    WANTQ   (input) LOGICAL   
            .TRUE. : update the left transformation matrix Q;   
            .FALSE.: do not update Q.   

    WANTZ   (input) LOGICAL   
            .TRUE. : update the right transformation matrix Z;   
            .FALSE.: do not update Z.   

    N       (input) INTEGER   
            The order of the matrices A and B. N >= 0.   

    A      (input/output) DOUBLE PRECISION arrays, dimensions (LDA,N)   
            On entry, the matrix A in the pair (A, B).   
            On exit, the updated matrix A.   

    LDA     (input)  INTEGER   
            The leading dimension of the array A. LDA >= max(1,N).   

    B      (input/output) DOUBLE PRECISION arrays, dimensions (LDB,N)   
            On entry, the matrix B in the pair (A, B).   
            On exit, the updated matrix B.   

    LDB     (input)  INTEGER   
            The leading dimension of the array B. LDB >= max(1,N).   

    Q       (input/output) DOUBLE PRECISION array, dimension (LDZ,N)   
            On entry, if WANTQ = .TRUE., the orthogonal matrix Q.   
            On exit, the updated matrix Q.   
            Not referenced if WANTQ = .FALSE..   

    LDQ     (input) INTEGER   
            The leading dimension of the array Q. LDQ >= 1.   
            If WANTQ = .TRUE., LDQ >= N.   

    Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N)   
            On entry, if WANTZ =.TRUE., the orthogonal matrix Z.   
            On exit, the updated matrix Z.   
            Not referenced if WANTZ = .FALSE..   

    LDZ     (input) INTEGER   
            The leading dimension of the array Z. LDZ >= 1.   
            If WANTZ = .TRUE., LDZ >= N.   

    J1      (input) INTEGER   
            The index to the first block (A11, B11). 1 <= J1 <= N.   

    N1      (input) INTEGER   
            The order of the first block (A11, B11). N1 = 0, 1 or 2.   

    N2      (input) INTEGER   
            The order of the second block (A22, B22). N2 = 0, 1 or 2.   

    WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK).   

    LWORK   (input) INTEGER   
            The dimension of the array WORK.   
            LWORK >=  MAX( N*(N2+N1), (N2+N1)*(N2+N1)*2 )   

    INFO    (output) INTEGER   
              =0: Successful exit   
              >0: If INFO = 1, the transformed matrix (A, B) would be   
                  too far from generalized Schur form; the blocks are   
                  not swapped and (A, B) and (Q, Z) are unchanged.   
                  The problem of swapping is too ill-conditioned.   
              <0: If INFO = -16: LWORK is too small. Appropriate value   
                  for LWORK is returned in WORK(1).   

    Further Details   
    ===============   

    Based on contributions by   
       Bo Kagstrom and Peter Poromaa, Department of Computing Science,   
       Umea University, S-901 87 Umea, Sweden.   

    In the current code both weak and strong stability tests are   
    performed. The user can omit the strong stability test by changing   
    the internal logical parameter WANDS to .FALSE.. See ref. [2] for   
    details.   

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

    =====================================================================   


       Parameter adjustments */
    /* Table of constant values */
    static integer c__16 = 16;
    static doublereal c_b3 = 0.;
    static integer c__0 = 0;
    static integer c__1 = 1;
    static integer c__4 = 4;
    static integer c__2 = 2;
    static doublereal c_b38 = 1.;
    static doublereal c_b44 = -1.;
    
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1, 
	    z_offset, i__1, i__2;
    doublereal d__1, d__2;
    /* Builtin functions */
    double sqrt(doublereal);
    /* Local variables */
    static logical weak;
    static doublereal ddum;
    static integer idum;
    static doublereal taul[4], dsum;
    extern /* Subroutine */ int drot_(integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *);
    static doublereal taur[4], scpy[16]	/* was [4][4] */, tcpy[16]	/* 
	    was [4][4] */, f, g;
    static integer i__, m;
    static doublereal s[16]	/* was [4][4] */, t[16]	/* was [4][4] */;
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *, 
	    integer *);
    static doublereal scale, bqra21, brqa21;
    extern /* Subroutine */ int dgemm_(char *, char *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, integer *, doublereal *, 
	    integer *, doublereal *, doublereal *, integer *);
    static doublereal licop[16]	/* was [4][4] */;
    static integer linfo;
    static doublereal ircop[16]	/* was [4][4] */;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *, 
	    doublereal *, integer *);
    static doublereal dnorm;
    static integer iwork[4];
    extern /* Subroutine */ int dlagv2_(doublereal *, integer *, doublereal *,
	     integer *, doublereal *, doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *), dgeqr2_(integer *, 
	    integer *, doublereal *, integer *, doublereal *, doublereal *, 
	    integer *), dgerq2_(integer *, integer *, doublereal *, integer *,
	     doublereal *, doublereal *, integer *), dorg2r_(integer *, 
	    integer *, integer *, doublereal *, integer *, doublereal *, 
	    doublereal *, integer *), dorgr2_(integer *, integer *, integer *,
	     doublereal *, integer *, doublereal *, doublereal *, integer *), 
	    dorm2r_(char *, char *, integer *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, integer *, 
	    doublereal *, integer *), dormr2_(char *, char *, 
	    integer *, integer *, integer *, doublereal *, integer *, 
	    doublereal *, doublereal *, integer *, doublereal *, integer *);
    static doublereal be[2], ai[2];
    extern /* Subroutine */ int dtgsy2_(char *, integer *, integer *, integer 
	    *, doublereal *, integer *, doublereal *, integer *, doublereal *,
	     integer *, doublereal *, integer *, doublereal *, integer *, 
	    doublereal *, integer *, doublereal *, doublereal *, doublereal *,
	     integer *, integer *, integer *);
    static doublereal ar[2], sa, sb, li[16]	/* was [4][4] */;
    extern doublereal dlamch_(char *);
    static doublereal dscale, ir[16]	/* was [4][4] */, ss, ws;
    extern /* Subroutine */ int dlacpy_(char *, integer *, integer *, 
	    doublereal *, integer *, doublereal *, integer *), 
	    dlartg_(doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *), dlassq_(integer *, doublereal *, integer *, 
	    doublereal *, doublereal *);
    static logical dtrong;
    static doublereal thresh, smlnum, eps;
#define scpy_ref(a_1,a_2) scpy[(a_2)*4 + a_1 - 5]
#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define q_ref(a_1,a_2) q[(a_2)*q_dim1 + a_1]
#define s_ref(a_1,a_2) s[(a_2)*4 + a_1 - 5]
#define t_ref(a_1,a_2) t[(a_2)*4 + a_1 - 5]
#define z___ref(a_1,a_2) z__[(a_2)*z_dim1 + a_1]
#define li_ref(a_1,a_2) li[(a_2)*4 + a_1 - 5]
#define ir_ref(a_1,a_2) ir[(a_2)*4 + a_1 - 5]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1 * 1;
    q -= q_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1 * 1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n <= 1 || *n1 <= 0 || *n2 <= 0) {
	return 0;
    }
    if (*n1 > *n || *j1 + *n1 > *n) {
	return 0;
    }
    m = *n1 + *n2;
/* Computing MAX */
    i__1 = *n * m, i__2 = m * m << 1;
    if (*lwork < max(i__1,i__2)) {
	*info = -16;
/* Computing MAX */
	i__1 = *n * m, i__2 = m * m << 1;
	work[1] = (doublereal) max(i__1,i__2);
	return 0;
    }

    weak = FALSE_;
    dtrong = FALSE_;

/*     Make a local copy of selected block */

    dcopy_(&c__16, &c_b3, &c__0, li, &c__1);
    dcopy_(&c__16, &c_b3, &c__0, ir, &c__1);
    dlacpy_("Full", &m, &m, &a_ref(*j1, *j1), lda, s, &c__4);
    dlacpy_("Full", &m, &m, &b_ref(*j1, *j1), ldb, t, &c__4);

/*     Compute threshold for testing acceptance of swapping. */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    dscale = 0.;
    dsum = 1.;
    dlacpy_("Full", &m, &m, s, &c__4, &work[1], &m);
    i__1 = m * m;
    dlassq_(&i__1, &work[1], &c__1, &dscale, &dsum);
    dlacpy_("Full", &m, &m, t, &c__4, &work[1], &m);
    i__1 = m * m;
    dlassq_(&i__1, &work[1], &c__1, &dscale, &dsum);
    dnorm = dscale * sqrt(dsum);
/* Computing MAX */
    d__1 = eps * 10. * dnorm;
    thresh = max(d__1,smlnum);

    if (m == 2) {

/*        CASE 1: Swap 1-by-1 and 1-by-1 blocks.   

          Compute orthogonal QL and RQ that swap 1-by-1 and 1-by-1 blocks   
          using Givens rotations and perform the swap tentatively. */

	f = s_ref(2, 2) * t_ref(1, 1) - t_ref(2, 2) * s_ref(1, 1);
	g = s_ref(2, 2) * t_ref(1, 2) - t_ref(2, 2) * s_ref(1, 2);
	sb = (d__1 = t_ref(2, 2), abs(d__1));
	sa = (d__1 = s_ref(2, 2), abs(d__1));
	dlartg_(&f, &g, &ir_ref(1, 2), &ir_ref(1, 1), &ddum);
	ir_ref(2, 1) = -ir_ref(1, 2);
	ir_ref(2, 2) = ir_ref(1, 1);
	drot_(&c__2, &s_ref(1, 1), &c__1, &s_ref(1, 2), &c__1, &ir_ref(1, 1), 
		&ir_ref(2, 1));
	drot_(&c__2, &t_ref(1, 1), &c__1, &t_ref(1, 2), &c__1, &ir_ref(1, 1), 
		&ir_ref(2, 1));
	if (sa >= sb) {
	    dlartg_(&s_ref(1, 1), &s_ref(2, 1), &li_ref(1, 1), &li_ref(2, 1), 
		    &ddum);
	} else {
	    dlartg_(&t_ref(1, 1), &t_ref(2, 1), &li_ref(1, 1), &li_ref(2, 1), 
		    &ddum);
	}
	drot_(&c__2, &s_ref(1, 1), &c__4, &s_ref(2, 1), &c__4, &li_ref(1, 1), 
		&li_ref(2, 1));
	drot_(&c__2, &t_ref(1, 1), &c__4, &t_ref(2, 1), &c__4, &li_ref(1, 1), 
		&li_ref(2, 1));
	li_ref(2, 2) = li_ref(1, 1);
	li_ref(1, 2) = -li_ref(2, 1);

/*        Weak stability test:   
             |S21| + |T21| <= O(EPS * F-norm((S, T))) */

	ws = (d__1 = s_ref(2, 1), abs(d__1)) + (d__2 = t_ref(2, 1), abs(d__2))
		;
	weak = ws <= thresh;
	if (! weak) {
	    goto L70;
	}

	if (TRUE_) {

/*           Strong stability test:   
               F-norm((A-QL'*S*QR, B-QL'*T*QR)) <= O(EPS*F-norm((A,B))) */

	    dlacpy_("Full", &m, &m, &a_ref(*j1, *j1), lda, &work[m * m + 1], &
		    m);
	    dgemm_("N", "N", &m, &m, &m, &c_b38, li, &c__4, s, &c__4, &c_b3, &
		    work[1], &m);
	    dgemm_("N", "T", &m, &m, &m, &c_b44, &work[1], &m, ir, &c__4, &
		    c_b38, &work[m * m + 1], &m);
	    dscale = 0.;
	    dsum = 1.;
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);

	    dlacpy_("Full", &m, &m, &b_ref(*j1, *j1), ldb, &work[m * m + 1], &
		    m);
	    dgemm_("N", "N", &m, &m, &m, &c_b38, li, &c__4, t, &c__4, &c_b3, &
		    work[1], &m);
	    dgemm_("N", "T", &m, &m, &m, &c_b44, &work[1], &m, ir, &c__4, &
		    c_b38, &work[m * m + 1], &m);
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);
	    ss = dscale * sqrt(dsum);
	    dtrong = ss <= thresh;
	    if (! dtrong) {
		goto L70;
	    }
	}

/*        Update (A(J1:J1+M-1, M+J1:N), B(J1:J1+M-1, M+J1:N)) and   
                 (A(1:J1-1, J1:J1+M), B(1:J1-1, J1:J1+M)). */

	i__1 = *j1 + 1;
	drot_(&i__1, &a_ref(1, *j1), &c__1, &a_ref(1, *j1 + 1), &c__1, &
		ir_ref(1, 1), &ir_ref(2, 1));
	i__1 = *j1 + 1;
	drot_(&i__1, &b_ref(1, *j1), &c__1, &b_ref(1, *j1 + 1), &c__1, &
		ir_ref(1, 1), &ir_ref(2, 1));
	i__1 = *n - *j1 + 1;
	drot_(&i__1, &a_ref(*j1, *j1), lda, &a_ref(*j1 + 1, *j1), lda, &
		li_ref(1, 1), &li_ref(2, 1));
	i__1 = *n - *j1 + 1;
	drot_(&i__1, &b_ref(*j1, *j1), ldb, &b_ref(*j1 + 1, *j1), ldb, &
		li_ref(1, 1), &li_ref(2, 1));

/*        Set  N1-by-N2 (2,1) - blocks to ZERO. */

	a_ref(*j1 + 1, *j1) = 0.;
	b_ref(*j1 + 1, *j1) = 0.;

/*        Accumulate transformations into Q and Z if requested. */

	if (*wantz) {
	    drot_(n, &z___ref(1, *j1), &c__1, &z___ref(1, *j1 + 1), &c__1, &
		    ir_ref(1, 1), &ir_ref(2, 1));
	}
	if (*wantq) {
	    drot_(n, &q_ref(1, *j1), &c__1, &q_ref(1, *j1 + 1), &c__1, &
		    li_ref(1, 1), &li_ref(2, 1));
	}

/*        Exit with INFO = 0 if swap was successfully performed. */

	return 0;

    } else {

/*        CASE 2: Swap 1-by-1 and 2-by-2 blocks, or 2-by-2   
                  and 2-by-2 blocks.   

          Solve the generalized Sylvester equation   
                   S11 * R - L * S22 = SCALE * S12   
                   T11 * R - L * T22 = SCALE * T12   
          for R and L. Solutions in LI and IR. */

	dlacpy_("Full", n1, n2, &t_ref(1, *n1 + 1), &c__4, li, &c__4);
	dlacpy_("Full", n1, n2, &s_ref(1, *n1 + 1), &c__4, &ir_ref(*n2 + 1, *
		n1 + 1), &c__4);
	dtgsy2_("N", &c__0, n1, n2, s, &c__4, &s_ref(*n1 + 1, *n1 + 1), &c__4,
		 &ir_ref(*n2 + 1, *n1 + 1), &c__4, t, &c__4, &t_ref(*n1 + 1, *
		n1 + 1), &c__4, li, &c__4, &scale, &dsum, &dscale, iwork, &
		idum, &linfo);

/*        Compute orthogonal matrix QL:   

                      QL' * LI = [ TL ]   
                                 [ 0  ]   
          where   
                      LI =  [      -L              ]   
                            [ SCALE * identity(N2) ] */

	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(n1, &c_b44, &li_ref(1, i__), &c__1);
	    li_ref(*n1 + i__, i__) = scale;
/* L10: */
	}
	dgeqr2_(&m, n2, li, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorg2r_(&m, &m, n2, li, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute orthogonal matrix RQ:   

                      IR * RQ' =   [ 0  TR],   

           where IR = [ SCALE * identity(N1), R ] */

	i__1 = *n1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ir_ref(*n2 + i__, i__) = scale;
/* L20: */
	}
	dgerq2_(n1, &m, &ir_ref(*n2 + 1, 1), &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorgr2_(&m, &m, n1, ir, &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Perform the swapping tentatively: */

	dgemm_("T", "N", &m, &m, &m, &c_b38, li, &c__4, s, &c__4, &c_b3, &
		work[1], &m);
	dgemm_("N", "T", &m, &m, &m, &c_b38, &work[1], &m, ir, &c__4, &c_b3, 
		s, &c__4);
	dgemm_("T", "N", &m, &m, &m, &c_b38, li, &c__4, t, &c__4, &c_b3, &
		work[1], &m);
	dgemm_("N", "T", &m, &m, &m, &c_b38, &work[1], &m, ir, &c__4, &c_b3, 
		t, &c__4);
	dlacpy_("F", &m, &m, s, &c__4, scpy, &c__4);
	dlacpy_("F", &m, &m, t, &c__4, tcpy, &c__4);
	dlacpy_("F", &m, &m, ir, &c__4, ircop, &c__4);
	dlacpy_("F", &m, &m, li, &c__4, licop, &c__4);

/*        Triangularize the B-part by an RQ factorization.   
          Apply transformation (from left) to A-part, giving S. */

	dgerq2_(&m, &m, t, &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dormr2_("R", "T", &m, &m, &m, t, &c__4, taur, s, &c__4, &work[1], &
		linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dormr2_("L", "N", &m, &m, &m, t, &c__4, taur, ir, &c__4, &work[1], &
		linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute F-norm(S21) in BRQA21. (T21 is 0.) */

	dscale = 0.;
	dsum = 1.;
	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlassq_(n1, &s_ref(*n2 + 1, i__), &c__1, &dscale, &dsum);
/* L30: */
	}
	brqa21 = dscale * sqrt(dsum);

/*        Triangularize the B-part by a QR factorization.   
          Apply transformation (from right) to A-part, giving S. */

	dgeqr2_(&m, &m, tcpy, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorm2r_("L", "T", &m, &m, &m, tcpy, &c__4, taul, scpy, &c__4, &work[1]
		, info);
	dorm2r_("R", "N", &m, &m, &m, tcpy, &c__4, taul, licop, &c__4, &work[
		1], info);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute F-norm(S21) in BQRA21. (T21 is 0.) */

	dscale = 0.;
	dsum = 1.;
	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlassq_(n1, &scpy_ref(*n2 + 1, i__), &c__1, &dscale, &dsum);
/* L40: */
	}
	bqra21 = dscale * sqrt(dsum);

/*        Decide which method to use.   
            Weak stability test:   
               F-norm(S21) <= O(EPS * F-norm((S, T))) */

	if (bqra21 <= brqa21 && bqra21 <= thresh) {
	    dlacpy_("F", &m, &m, scpy, &c__4, s, &c__4);
	    dlacpy_("F", &m, &m, tcpy, &c__4, t, &c__4);
	    dlacpy_("F", &m, &m, ircop, &c__4, ir, &c__4);
	    dlacpy_("F", &m, &m, licop, &c__4, li, &c__4);
	} else if (brqa21 >= thresh) {
	    goto L70;
	}

/*        Set lower triangle of B-part to zero */

	i__1 = m;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    i__2 = m - i__ + 1;
	    dcopy_(&i__2, &c_b3, &c__0, &t_ref(i__, i__ - 1), &c__1);
/* L50: */
	}

	if (TRUE_) {

/*           Strong stability test:   
                F-norm((A-QL*S*QR', B-QL*T*QR')) <= O(EPS*F-norm((A,B))) */

	    dlacpy_("Full", &m, &m, &a_ref(*j1, *j1), lda, &work[m * m + 1], &
		    m);
	    dgemm_("N", "N", &m, &m, &m, &c_b38, li, &c__4, s, &c__4, &c_b3, &
		    work[1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b44, &work[1], &m, ir, &c__4, &
		    c_b38, &work[m * m + 1], &m);
	    dscale = 0.;
	    dsum = 1.;
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);

	    dlacpy_("Full", &m, &m, &b_ref(*j1, *j1), ldb, &work[m * m + 1], &
		    m);
	    dgemm_("N", "N", &m, &m, &m, &c_b38, li, &c__4, t, &c__4, &c_b3, &
		    work[1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b44, &work[1], &m, ir, &c__4, &
		    c_b38, &work[m * m + 1], &m);
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);
	    ss = dscale * sqrt(dsum);
	    dtrong = ss <= thresh;
	    if (! dtrong) {
		goto L70;
	    }

	}

/*        If the swap is accepted ("weakly" and "strongly"), apply the   
          transformations and set N1-by-N2 (2,1)-block to zero. */

	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dcopy_(n1, &c_b3, &c__0, &s_ref(*n2 + 1, i__), &c__1);
/* L60: */
	}

/*        copy back M-by-M diagonal block starting at index J1 of (A, B) */

	dlacpy_("F", &m, &m, s, &c__4, &a_ref(*j1, *j1), lda);
	dlacpy_("F", &m, &m, t, &c__4, &b_ref(*j1, *j1), ldb);
	dcopy_(&c__16, &c_b3, &c__0, t, &c__1);

/*        Standardize existing 2-by-2 blocks. */

	i__1 = m * m;
	dcopy_(&i__1, &c_b3, &c__0, &work[1], &c__1);
	work[1] = 1.;
	t_ref(1, 1) = 1.;
	idum = *lwork - m * m - 2;
	if (*n2 > 1) {
	    dlagv2_(&a_ref(*j1, *j1), lda, &b_ref(*j1, *j1), ldb, ar, ai, be, 
		    &work[1], &work[2], &t_ref(1, 1), &t_ref(2, 1));
	    work[m + 1] = -work[2];
	    work[m + 2] = work[1];
	    t_ref(*n2, *n2) = t_ref(1, 1);
	    t_ref(1, 2) = -t_ref(2, 1);
	}
	work[m * m] = 1.;
	t_ref(m, m) = 1.;

	if (*n1 > 1) {
	    dlagv2_(&a_ref(*j1 + *n2, *j1 + *n2), lda, &b_ref(*j1 + *n2, *j1 
		    + *n2), ldb, taur, taul, &work[m * m + 1], &work[*n2 * m 
		    + *n2 + 1], &work[*n2 * m + *n2 + 2], &t_ref(*n2 + 1, *n2 
		    + 1), &t_ref(m, m - 1));
	    work[m * m] = work[*n2 * m + *n2 + 1];
	    work[m * m - 1] = -work[*n2 * m + *n2 + 2];
	    t_ref(m, m) = t_ref(*n2 + 1, *n2 + 1);
	    t_ref(m - 1, m) = -t_ref(m, m - 1);
	}
	dgemm_("T", "N", n2, n1, n2, &c_b38, &work[1], &m, &a_ref(*j1, *j1 + *
		n2), lda, &c_b3, &work[m * m + 1], n2);
	dlacpy_("Full", n2, n1, &work[m * m + 1], n2, &a_ref(*j1, *j1 + *n2), 
		lda);
	dgemm_("T", "N", n2, n1, n2, &c_b38, &work[1], &m, &b_ref(*j1, *j1 + *
		n2), ldb, &c_b3, &work[m * m + 1], n2);
	dlacpy_("Full", n2, n1, &work[m * m + 1], n2, &b_ref(*j1, *j1 + *n2), 
		ldb);
	dgemm_("N", "N", &m, &m, &m, &c_b38, li, &c__4, &work[1], &m, &c_b3, &
		work[m * m + 1], &m);
	dlacpy_("Full", &m, &m, &work[m * m + 1], &m, li, &c__4);
	dgemm_("N", "N", n2, n1, n1, &c_b38, &a_ref(*j1, *j1 + *n2), lda, &
		t_ref(*n2 + 1, *n2 + 1), &c__4, &c_b3, &work[1], n2);
	dlacpy_("Full", n2, n1, &work[1], n2, &a_ref(*j1, *j1 + *n2), lda);
	dgemm_("N", "N", n2, n1, n1, &c_b38, &b_ref(*j1, *j1 + *n2), lda, &
		t_ref(*n2 + 1, *n2 + 1), &c__4, &c_b3, &work[1], n2);
	dlacpy_("Full", n2, n1, &work[1], n2, &b_ref(*j1, *j1 + *n2), ldb);
	dgemm_("T", "N", &m, &m, &m, &c_b38, ir, &c__4, t, &c__4, &c_b3, &
		work[1], &m);
	dlacpy_("Full", &m, &m, &work[1], &m, ir, &c__4);

/*        Accumulate transformations into Q and Z if requested. */

	if (*wantq) {
	    dgemm_("N", "N", n, &m, &m, &c_b38, &q_ref(1, *j1), ldq, li, &
		    c__4, &c_b3, &work[1], n);
	    dlacpy_("Full", n, &m, &work[1], n, &q_ref(1, *j1), ldq);

	}

	if (*wantz) {
	    dgemm_("N", "N", n, &m, &m, &c_b38, &z___ref(1, *j1), ldz, ir, &
		    c__4, &c_b3, &work[1], n);
	    dlacpy_("Full", n, &m, &work[1], n, &z___ref(1, *j1), ldz);

	}

/*        Update (A(J1:J1+M-1, M+J1:N), B(J1:J1+M-1, M+J1:N)) and   
                  (A(1:J1-1, J1:J1+M), B(1:J1-1, J1:J1+M)). */

	i__ = *j1 + m;
	if (i__ <= *n) {
	    i__1 = *n - i__ + 1;
	    dgemm_("T", "N", &m, &i__1, &m, &c_b38, li, &c__4, &a_ref(*j1, 
		    i__), lda, &c_b3, &work[1], &m);
	    i__1 = *n - i__ + 1;
	    dlacpy_("Full", &m, &i__1, &work[1], &m, &a_ref(*j1, i__), lda);
	    i__1 = *n - i__ + 1;
	    dgemm_("T", "N", &m, &i__1, &m, &c_b38, li, &c__4, &b_ref(*j1, 
		    i__), lda, &c_b3, &work[1], &m);
	    i__1 = *n - i__ + 1;
	    dlacpy_("Full", &m, &i__1, &work[1], &m, &b_ref(*j1, i__), lda);
	}
	i__ = *j1 - 1;
	if (i__ > 0) {
	    dgemm_("N", "N", &i__, &m, &m, &c_b38, &a_ref(1, *j1), lda, ir, &
		    c__4, &c_b3, &work[1], &i__);
	    dlacpy_("Full", &i__, &m, &work[1], &i__, &a_ref(1, *j1), lda);
	    dgemm_("N", "N", &i__, &m, &m, &c_b38, &b_ref(1, *j1), ldb, ir, &
		    c__4, &c_b3, &work[1], &i__);
	    dlacpy_("Full", &i__, &m, &work[1], &i__, &b_ref(1, *j1), ldb);
	}

/*        Exit with INFO = 0 if swap was successfully performed. */

	return 0;

    }

/*     Exit with INFO = 1 if swap was rejected. */

L70:

    *info = 1;
    return 0;

/*     End of DTGEX2 */

} /* dtgex2_ */

#undef ir_ref
#undef li_ref
#undef z___ref
#undef t_ref
#undef s_ref
#undef q_ref
#undef b_ref
#undef a_ref
#undef scpy_ref


