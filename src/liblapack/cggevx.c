
#include "pnl_f2c.h"

/* Subroutine */ int cggevx_(char *balanc, char *jobvl, char *jobvr, char *
	sense, integer *n, complex *a, integer *lda, complex *b, integer *ldb,
	 complex *alpha, complex *beta, complex *vl, integer *ldvl, complex *
	vr, integer *ldvr, integer *ilo, integer *ihi, real *lscale, real *
	rscale, real *abnrm, real *bbnrm, real *rconde, real *rcondv, complex 
	*work, integer *lwork, real *rwork, integer *iwork, logical *bwork, 
	integer *info)
{
/*  -- LAPACK driver routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       June 30, 1999   


    Purpose   
    =======   

    CGGEVX computes for a pair of N-by-N complex nonsymmetric matrices   
    (A,B) the generalized eigenvalues, and optionally, the left and/or   
    right generalized eigenvectors.   

    Optionally, it also computes a balancing transformation to improve   
    the conditioning of the eigenvalues and eigenvectors (ILO, IHI,   
    LSCALE, RSCALE, ABNRM, and BBNRM), reciprocal condition numbers for   
    the eigenvalues (RCONDE), and reciprocal condition numbers for the   
    right eigenvectors (RCONDV).   

    A generalized eigenvalue for a pair of matrices (A,B) is a scalar   
    lambda or a ratio alpha/beta = lambda, such that A - lambda*B is   
    singular. It is usually represented as the pair (alpha,beta), as   
    there is a reasonable interpretation for beta=0, and even for both   
    being zero.   

    The right eigenvector v(j) corresponding to the eigenvalue lambda(j)   
    of (A,B) satisfies   
                     A * v(j) = lambda(j) * B * v(j) .   
    The left eigenvector u(j) corresponding to the eigenvalue lambda(j)   
    of (A,B) satisfies   
                     u(j)**H * A  = lambda(j) * u(j)**H * B.   
    where u(j)**H is the conjugate-transpose of u(j).   


    Arguments   
    =========   

    BALANC  (input) CHARACTER*1   
            Specifies the balance option to be performed:   
            = 'N':  do not diagonally scale or permute;   
            = 'P':  permute only;   
            = 'S':  scale only;   
            = 'B':  both permute and scale.   
            Computed reciprocal condition numbers will be for the   
            matrices after permuting and/or balancing. Permuting does   
            not change condition numbers (in exact arithmetic), but   
            balancing does.   

    JOBVL   (input) CHARACTER*1   
            = 'N':  do not compute the left generalized eigenvectors;   
            = 'V':  compute the left generalized eigenvectors.   

    JOBVR   (input) CHARACTER*1   
            = 'N':  do not compute the right generalized eigenvectors;   
            = 'V':  compute the right generalized eigenvectors.   

    SENSE   (input) CHARACTER*1   
            Determines which reciprocal condition numbers are computed.   
            = 'N': none are computed;   
            = 'E': computed for eigenvalues only;   
            = 'V': computed for eigenvectors only;   
            = 'B': computed for eigenvalues and eigenvectors.   

    N       (input) INTEGER   
            The order of the matrices A, B, VL, and VR.  N >= 0.   

    A       (input/output) COMPLEX array, dimension (LDA, N)   
            On entry, the matrix A in the pair (A,B).   
            On exit, A has been overwritten. If JOBVL='V' or JOBVR='V'   
            or both, then A contains the first part of the complex Schur   
            form of the "balanced" versions of the input A and B.   

    LDA     (input) INTEGER   
            The leading dimension of A.  LDA >= max(1,N).   

    B       (input/output) COMPLEX array, dimension (LDB, N)   
            On entry, the matrix B in the pair (A,B).   
            On exit, B has been overwritten. If JOBVL='V' or JOBVR='V'   
            or both, then B contains the second part of the complex   
            Schur form of the "balanced" versions of the input A and B.   

    LDB     (input) INTEGER   
            The leading dimension of B.  LDB >= max(1,N).   

    ALPHA   (output) COMPLEX array, dimension (N)   
    BETA    (output) COMPLEX array, dimension (N)   
            On exit, ALPHA(j)/BETA(j), j=1,...,N, will be the generalized   
            eigenvalues.   

            Note: the quotient ALPHA(j)/BETA(j) ) may easily over- or   
            underflow, and BETA(j) may even be zero.  Thus, the user   
            should avoid naively computing the ratio ALPHA/BETA.   
            However, ALPHA will be always less than and usually   
            comparable with norm(A) in magnitude, and BETA always less   
            than and usually comparable with norm(B).   

    VL      (output) COMPLEX array, dimension (LDVL,N)   
            If JOBVL = 'V', the left generalized eigenvectors u(j) are   
            stored one after another in the columns of VL, in the same   
            order as their eigenvalues.   
            Each eigenvector will be scaled so the largest component   
            will have abs(real part) + abs(imag. part) = 1.   
            Not referenced if JOBVL = 'N'.   

    LDVL    (input) INTEGER   
            The leading dimension of the matrix VL. LDVL >= 1, and   
            if JOBVL = 'V', LDVL >= N.   

    VR      (output) COMPLEX array, dimension (LDVR,N)   
            If JOBVR = 'V', the right generalized eigenvectors v(j) are   
            stored one after another in the columns of VR, in the same   
            order as their eigenvalues.   
            Each eigenvector will be scaled so the largest component   
            will have abs(real part) + abs(imag. part) = 1.   
            Not referenced if JOBVR = 'N'.   

    LDVR    (input) INTEGER   
            The leading dimension of the matrix VR. LDVR >= 1, and   
            if JOBVR = 'V', LDVR >= N.   

    ILO,IHI (output) INTEGER   
            ILO and IHI are integer values such that on exit   
            A(i,j) = 0 and B(i,j) = 0 if i > j and   
            j = 1,...,ILO-1 or i = IHI+1,...,N.   
            If BALANC = 'N' or 'S', ILO = 1 and IHI = N.   

    LSCALE  (output) REAL array, dimension (N)   
            Details of the permutations and scaling factors applied   
            to the left side of A and B.  If PL(j) is the index of the   
            row interchanged with row j, and DL(j) is the scaling   
            factor applied to row j, then   
              LSCALE(j) = PL(j)  for j = 1,...,ILO-1   
                        = DL(j)  for j = ILO,...,IHI   
                        = PL(j)  for j = IHI+1,...,N.   
            The order in which the interchanges are made is N to IHI+1,   
            then 1 to ILO-1.   

    RSCALE  (output) REAL array, dimension (N)   
            Details of the permutations and scaling factors applied   
            to the right side of A and B.  If PR(j) is the index of the   
            column interchanged with column j, and DR(j) is the scaling   
            factor applied to column j, then   
              RSCALE(j) = PR(j)  for j = 1,...,ILO-1   
                        = DR(j)  for j = ILO,...,IHI   
                        = PR(j)  for j = IHI+1,...,N   
            The order in which the interchanges are made is N to IHI+1,   
            then 1 to ILO-1.   

    ABNRM   (output) REAL   
            The one-norm of the balanced matrix A.   

    BBNRM   (output) REAL   
            The one-norm of the balanced matrix B.   

    RCONDE  (output) REAL array, dimension (N)   
            If SENSE = 'E' or 'B', the reciprocal condition numbers of   
            the selected eigenvalues, stored in consecutive elements of   
            the array.   
            If SENSE = 'V', RCONDE is not referenced.   

    RCONDV  (output) REAL array, dimension (N)   
            If JOB = 'V' or 'B', the estimated reciprocal condition   
            numbers of the selected eigenvectors, stored in consecutive   
            elements of the array. If the eigenvalues cannot be reordered   
            to compute RCONDV(j), RCONDV(j) is set to 0; this can only   
            occur when the true value would be very small anyway.   
            If SENSE = 'E', RCONDV is not referenced.   
            Not referenced if JOB = 'E'.   

    WORK    (workspace/output) COMPLEX array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= max(1,2*N).   
            If SENSE = 'N' or 'E', LWORK >= 2*N.   
            If SENSE = 'V' or 'B', LWORK >= 2*N*N+2*N.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    RWORK   (workspace) REAL array, dimension (6*N)   
            Real workspace.   

    IWORK   (workspace) INTEGER array, dimension (N+2)   
            If SENSE = 'E', IWORK is not referenced.   

    BWORK   (workspace) LOGICAL array, dimension (N)   
            If SENSE = 'N', BWORK is not referenced.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            = 1,...,N:   
                  The QZ iteration failed.  No eigenvectors have been   
                  calculated, but ALPHA(j) and BETA(j) should be correct   
                  for j=INFO+1,...,N.   
            > N:  =N+1: other than QZ iteration failed in CHGEQZ.   
                  =N+2: error return from CTGEVC.   

    Further Details   
    ===============   

    Balancing a matrix pair (A,B) includes, first, permuting rows and   
    columns to isolate eigenvalues, second, applying diagonal similarity   
    transformation to the rows and columns to make the rows and columns   
    as close in norm as possible. The computed reciprocal condition   
    numbers correspond to the balanced matrix. Permuting rows and columns   
    will not change the condition numbers (in exact arithmetic) but   
    diagonal scaling will.  For further explanation of balancing, see   
    section 4.11.1.2 of LAPACK Users' Guide.   

    An approximate error bound on the chordal distance between the i-th   
    computed generalized eigenvalue w and the corresponding exact   
    eigenvalue lambda is   

         chord(w, lambda) <= EPS * norm(ABNRM, BBNRM) / RCONDE(I)   

    An approximate error bound for the angle between the i-th computed   
    eigenvector VL(i) or VR(i) is given by   

         EPS * norm(ABNRM, BBNRM) / DIF(i).   

    For further explanation of the reciprocal condition numbers RCONDE   
    and RCONDV, see section 4.11 of LAPACK User's Guide.   

    =====================================================================   


       Decode the input arguments   

       Parameter adjustments */
    /* Table of constant values */
    static complex c_b1 = {0.f,0.f};
    static complex c_b2 = {1.f,0.f};
    static integer c__1 = 1;
    static integer c__0 = 0;
    
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1, 
	    vr_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4;
    complex q__1;
    /* Builtin functions */
    double sqrt(doublereal), r_imag(complex *);
    /* Local variables */
    static real anrm, bnrm;
    static integer ierr, itau;
    static real temp;
    static logical ilvl, ilvr;
    static integer iwrk, iwrk1, i__, j, m;
    extern logical lsame_(char *, char *);
    static integer icols, irows, jc;
    extern /* Subroutine */ int cggbak_(char *, char *, integer *, integer *, 
	    integer *, real *, real *, integer *, complex *, integer *, 
	    integer *), cggbal_(char *, integer *, complex *, 
	    integer *, complex *, integer *, integer *, integer *, real *, 
	    real *, real *, integer *), slabad_(real *, real *);
    static integer in;
    extern doublereal clange_(char *, integer *, integer *, complex *, 
	    integer *, real *);
    static integer jr;
    extern /* Subroutine */ int cgghrd_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, integer *, complex *, 
	    integer *, complex *, integer *, integer *), 
	    clascl_(char *, integer *, integer *, real *, real *, integer *, 
	    integer *, complex *, integer *, integer *);
    static logical ilascl, ilbscl;
    extern /* Subroutine */ int cgeqrf_(integer *, integer *, complex *, 
	    integer *, complex *, complex *, integer *, integer *), clacpy_(
	    char *, integer *, integer *, complex *, integer *, complex *, 
	    integer *), claset_(char *, integer *, integer *, complex 
	    *, complex *, complex *, integer *);
    static logical ldumma[1];
    static char chtemp[1];
    static real bignum;
    extern /* Subroutine */ int chgeqz_(char *, char *, char *, integer *, 
	    integer *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, complex *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, integer *), 
	    ctgevc_(char *, char *, logical *, integer *, complex *, integer *
	    , complex *, integer *, complex *, integer *, complex *, integer *
	    , integer *, integer *, complex *, real *, integer *);
    static integer ijobvl;
    extern /* Subroutine */ int ctgsna_(char *, char *, logical *, integer *, 
	    complex *, integer *, complex *, integer *, complex *, integer *, 
	    complex *, integer *, real *, real *, integer *, integer *, 
	    complex *, integer *, integer *, integer *), 
	    slascl_(char *, integer *, integer *, real *, real *, integer *, 
	    integer *, real *, integer *, integer *), xerbla_(char *, 
	    integer *);
    extern integer ilaenv_(integer *, char *, char *, integer *, integer *, 
	    integer *, integer *, ftnlen, ftnlen);
    extern doublereal slamch_(char *);
    static integer ijobvr;
    static logical wantsb;
    extern /* Subroutine */ int cungqr_(integer *, integer *, integer *, 
	    complex *, integer *, complex *, complex *, integer *, integer *);
    static real anrmto;
    static logical wantse;
    static real bnrmto;
    extern /* Subroutine */ int cunmqr_(char *, char *, integer *, integer *, 
	    integer *, complex *, integer *, complex *, complex *, integer *, 
	    complex *, integer *, integer *);
    static integer minwrk, maxwrk;
    static logical wantsn;
    static real smlnum;
    static logical lquery, wantsv;
    static real eps;
    static logical ilv;
#define a_subscr(a_1,a_2) (a_2)*a_dim1 + a_1
#define a_ref(a_1,a_2) a[a_subscr(a_1,a_2)]
#define b_subscr(a_1,a_2) (a_2)*b_dim1 + a_1
#define b_ref(a_1,a_2) b[b_subscr(a_1,a_2)]
#define vl_subscr(a_1,a_2) (a_2)*vl_dim1 + a_1
#define vl_ref(a_1,a_2) vl[vl_subscr(a_1,a_2)]
#define vr_subscr(a_1,a_2) (a_2)*vr_dim1 + a_1
#define vr_ref(a_1,a_2) vr[vr_subscr(a_1,a_2)]


    a_dim1 = *lda;
    a_offset = 1 + a_dim1 * 1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1 * 1;
    b -= b_offset;
    --alpha;
    --beta;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1 * 1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1 * 1;
    vr -= vr_offset;
    --lscale;
    --rscale;
    --rconde;
    --rcondv;
    --work;
    --rwork;
    --iwork;
    --bwork;

    /* Function Body */
    if (lsame_(jobvl, "N")) {
	ijobvl = 1;
	ilvl = FALSE_;
    } else if (lsame_(jobvl, "V")) {
	ijobvl = 2;
	ilvl = TRUE_;
    } else {
	ijobvl = -1;
	ilvl = FALSE_;
    }

    if (lsame_(jobvr, "N")) {
	ijobvr = 1;
	ilvr = FALSE_;
    } else if (lsame_(jobvr, "V")) {
	ijobvr = 2;
	ilvr = TRUE_;
    } else {
	ijobvr = -1;
	ilvr = FALSE_;
    }
    ilv = ilvl || ilvr;

    wantsn = lsame_(sense, "N");
    wantse = lsame_(sense, "E");
    wantsv = lsame_(sense, "V");
    wantsb = lsame_(sense, "B");

/*     Test the input arguments */

    *info = 0;
    lquery = *lwork == -1;
    if (! (lsame_(balanc, "N") || lsame_(balanc, "S") || lsame_(balanc, "P") 
	    || lsame_(balanc, "B"))) {
	*info = -1;
    } else if (ijobvl <= 0) {
	*info = -2;
    } else if (ijobvr <= 0) {
	*info = -3;
    } else if (! (wantsn || wantse || wantsb || wantsv)) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < max(1,*n)) {
	*info = -7;
    } else if (*ldb < max(1,*n)) {
	*info = -9;
    } else if (*ldvl < 1 || ilvl && *ldvl < *n) {
	*info = -13;
    } else if (*ldvr < 1 || ilvr && *ldvr < *n) {
	*info = -15;
    }

/*     Compute workspace   
        (Note: Comments in the code beginning "Workspace:" describe the   
         minimal amount of workspace needed at that point in the code,   
         as well as the preferred amount for good performance.   
         NB refers to the optimal block size for the immediately   
         following subroutine, as returned by ILAENV. The workspace is   
         computed assuming ILO = 1 and IHI = N, the worst case.) */

    minwrk = 1;
    if (*info == 0 && (*lwork >= 1 || lquery)) {
	maxwrk = *n + *n * ilaenv_(&c__1, "CGEQRF", " ", n, &c__1, n, &c__0, (
		ftnlen)6, (ftnlen)1);
	if (wantse) {
/* Computing MAX */
	    i__1 = 1, i__2 = *n << 1;
	    minwrk = max(i__1,i__2);
	} else if (wantsv || wantsb) {
	    minwrk = (*n << 1) * *n + (*n << 1);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = (*n << 1) * *n + (*n << 1);
	    maxwrk = max(i__1,i__2);
	}
	work[1].r = (real) maxwrk, work[1].i = 0.f;
    }

    if (*lwork < minwrk && ! lquery) {
	*info = -25;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("CGGEVX", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = slamch_("P");
    smlnum = slamch_("S");
    bignum = 1.f / smlnum;
    slabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1.f / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = clange_("M", n, n, &a[a_offset], lda, &rwork[1]);
    ilascl = FALSE_;
    if (anrm > 0.f && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = TRUE_;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = TRUE_;
    }
    if (ilascl) {
	clascl_("G", &c__0, &c__0, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = clange_("M", n, n, &b[b_offset], ldb, &rwork[1]);
    ilbscl = FALSE_;
    if (bnrm > 0.f && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = TRUE_;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = TRUE_;
    }
    if (ilbscl) {
	clascl_("G", &c__0, &c__0, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		ierr);
    }

/*     Permute and/or balance the matrix pair (A,B)   
       (Real Workspace: need 6*N) */

    cggbal_(balanc, n, &a[a_offset], lda, &b[b_offset], ldb, ilo, ihi, &
	    lscale[1], &rscale[1], &rwork[1], &ierr);

/*     Compute ABNRM and BBNRM */

    *abnrm = clange_("1", n, n, &a[a_offset], lda, &rwork[1]);
    if (ilascl) {
	rwork[1] = *abnrm;
	slascl_("G", &c__0, &c__0, &anrmto, &anrm, &c__1, &c__1, &rwork[1], &
		c__1, &ierr);
	*abnrm = rwork[1];
    }

    *bbnrm = clange_("1", n, n, &b[b_offset], ldb, &rwork[1]);
    if (ilbscl) {
	rwork[1] = *bbnrm;
	slascl_("G", &c__0, &c__0, &bnrmto, &bnrm, &c__1, &c__1, &rwork[1], &
		c__1, &ierr);
	*bbnrm = rwork[1];
    }

/*     Reduce B to triangular form (QR decomposition of B)   
       (Complex Workspace: need N, prefer N*NB ) */

    irows = *ihi + 1 - *ilo;
    if (ilv || ! wantsn) {
	icols = *n + 1 - *ilo;
    } else {
	icols = irows;
    }
    itau = 1;
    iwrk = itau + irows;
    i__1 = *lwork + 1 - iwrk;
    cgeqrf_(&irows, &icols, &b_ref(*ilo, *ilo), ldb, &work[itau], &work[iwrk],
	     &i__1, &ierr);

/*     Apply the unitary transformation to A   
       (Complex Workspace: need N, prefer N*NB) */

    i__1 = *lwork + 1 - iwrk;
    cunmqr_("L", "C", &irows, &icols, &irows, &b_ref(*ilo, *ilo), ldb, &work[
	    itau], &a_ref(*ilo, *ilo), lda, &work[iwrk], &i__1, &ierr);

/*     Initialize VL and/or VR   
       (Workspace: need N, prefer N*NB) */

    if (ilvl) {
	claset_("Full", n, n, &c_b1, &c_b2, &vl[vl_offset], ldvl);
	i__1 = irows - 1;
	i__2 = irows - 1;
	clacpy_("L", &i__1, &i__2, &b_ref(*ilo + 1, *ilo), ldb, &vl_ref(*ilo 
		+ 1, *ilo), ldvl);
	i__1 = *lwork + 1 - iwrk;
	cungqr_(&irows, &irows, &irows, &vl_ref(*ilo, *ilo), ldvl, &work[itau]
		, &work[iwrk], &i__1, &ierr);
    }

    if (ilvr) {
	claset_("Full", n, n, &c_b1, &c_b2, &vr[vr_offset], ldvr);
    }

/*     Reduce to generalized Hessenberg form   
       (Workspace: none needed) */

    if (ilv || ! wantsn) {

/*        Eigenvectors requested -- work on whole matrix. */

	cgghrd_(jobvl, jobvr, n, ilo, ihi, &a[a_offset], lda, &b[b_offset], 
		ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &ierr);
    } else {
	cgghrd_("N", "N", &irows, &c__1, &irows, &a_ref(*ilo, *ilo), lda, &
		b_ref(*ilo, *ilo), ldb, &vl[vl_offset], ldvl, &vr[vr_offset], 
		ldvr, &ierr);
    }

/*     Perform QZ algorithm (Compute eigenvalues, and optionally, the   
       Schur forms and Schur vectors)   
       (Complex Workspace: need N)   
       (Real Workspace: need N) */

    iwrk = itau;
    if (ilv || ! wantsn) {
	*(unsigned char *)chtemp = 'S';
    } else {
	*(unsigned char *)chtemp = 'E';
    }

    i__1 = *lwork + 1 - iwrk;
    chgeqz_(chtemp, jobvl, jobvr, n, ilo, ihi, &a[a_offset], lda, &b[b_offset]
	    , ldb, &alpha[1], &beta[1], &vl[vl_offset], ldvl, &vr[vr_offset], 
	    ldvr, &work[iwrk], &i__1, &rwork[1], &ierr);
    if (ierr != 0) {
	if (ierr > 0 && ierr <= *n) {
	    *info = ierr;
	} else if (ierr > *n && ierr <= *n << 1) {
	    *info = ierr - *n;
	} else {
	    *info = *n + 1;
	}
	goto L90;
    }

/*     Compute Eigenvectors and estimate condition numbers if desired   
       CTGEVC: (Complex Workspace: need 2*N )   
               (Real Workspace:    need 2*N )   
       CTGSNA: (Complex Workspace: need 2*N*N if SENSE='V' or 'B')   
               (Integer Workspace: need N+2 ) */

    if (ilv || ! wantsn) {
	if (ilv) {
	    if (ilvl) {
		if (ilvr) {
		    *(unsigned char *)chtemp = 'B';
		} else {
		    *(unsigned char *)chtemp = 'L';
		}
	    } else {
		*(unsigned char *)chtemp = 'R';
	    }

	    ctgevc_(chtemp, "B", ldumma, n, &a[a_offset], lda, &b[b_offset], 
		    ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, n, &in, &
		    work[iwrk], &rwork[1], &ierr);
	    if (ierr != 0) {
		*info = *n + 2;
		goto L90;
	    }
	}

	if (! wantsn) {

/*           compute eigenvectors (STGEVC) and estimate condition   
             numbers (STGSNA). Note that the definition of the condition   
             number is not invariant under transformation (u,v) to   
             (Q*u, Z*v), where (u,v) are eigenvectors of the generalized   
             Schur form (S,T), Q and Z are orthogonal matrices. In order   
             to avoid using extra 2*N*N workspace, we have to   
             re-calculate eigenvectors and estimate the condition numbers   
             one at a time. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {

		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
		    bwork[j] = FALSE_;
/* L10: */
		}
		bwork[i__] = TRUE_;

		iwrk = *n + 1;
		iwrk1 = iwrk + *n;

		if (wantse || wantsb) {
		    ctgevc_("B", "S", &bwork[1], n, &a[a_offset], lda, &b[
			    b_offset], ldb, &work[1], n, &work[iwrk], n, &
			    c__1, &m, &work[iwrk1], &rwork[1], &ierr);
		    if (ierr != 0) {
			*info = *n + 2;
			goto L90;
		    }
		}

		i__2 = *lwork - iwrk1 + 1;
		ctgsna_(sense, "S", &bwork[1], n, &a[a_offset], lda, &b[
			b_offset], ldb, &work[1], n, &work[iwrk], n, &rconde[
			i__], &rcondv[i__], &c__1, &m, &work[iwrk1], &i__2, &
			iwork[1], &ierr);

/* L20: */
	    }
	}
    }

/*     Undo balancing on VL and VR and normalization   
       (Workspace: none needed) */

    if (ilvl) {
	cggbak_(balanc, "L", n, ilo, ihi, &lscale[1], &rscale[1], n, &vl[
		vl_offset], ldvl, &ierr);

	i__1 = *n;
	for (jc = 1; jc <= i__1; ++jc) {
	    temp = 0.f;
	    i__2 = *n;
	    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		i__3 = vl_subscr(jr, jc);
		r__3 = temp, r__4 = (r__1 = vl[i__3].r, dabs(r__1)) + (r__2 = 
			r_imag(&vl_ref(jr, jc)), dabs(r__2));
		temp = dmax(r__3,r__4);
/* L30: */
	    }
	    if (temp < smlnum) {
		goto L50;
	    }
	    temp = 1.f / temp;
	    i__2 = *n;
	    for (jr = 1; jr <= i__2; ++jr) {
		i__3 = vl_subscr(jr, jc);
		i__4 = vl_subscr(jr, jc);
		q__1.r = temp * vl[i__4].r, q__1.i = temp * vl[i__4].i;
		vl[i__3].r = q__1.r, vl[i__3].i = q__1.i;
/* L40: */
	    }
L50:
	    ;
	}
    }

    if (ilvr) {
	cggbak_(balanc, "R", n, ilo, ihi, &lscale[1], &rscale[1], n, &vr[
		vr_offset], ldvr, &ierr);
	i__1 = *n;
	for (jc = 1; jc <= i__1; ++jc) {
	    temp = 0.f;
	    i__2 = *n;
	    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		i__3 = vr_subscr(jr, jc);
		r__3 = temp, r__4 = (r__1 = vr[i__3].r, dabs(r__1)) + (r__2 = 
			r_imag(&vr_ref(jr, jc)), dabs(r__2));
		temp = dmax(r__3,r__4);
/* L60: */
	    }
	    if (temp < smlnum) {
		goto L80;
	    }
	    temp = 1.f / temp;
	    i__2 = *n;
	    for (jr = 1; jr <= i__2; ++jr) {
		i__3 = vr_subscr(jr, jc);
		i__4 = vr_subscr(jr, jc);
		q__1.r = temp * vr[i__4].r, q__1.i = temp * vr[i__4].i;
		vr[i__3].r = q__1.r, vr[i__3].i = q__1.i;
/* L70: */
	    }
L80:
	    ;
	}
    }

/*     Undo scaling if necessary */

    if (ilascl) {
	clascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alpha[1], n, &
		ierr);
    }

    if (ilbscl) {
	clascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		ierr);
    }

L90:
    work[1].r = (real) maxwrk, work[1].i = 0.f;

    return 0;

/*     End of CGGEVX */

} /* cggevx_ */

#undef vr_ref
#undef vr_subscr
#undef vl_ref
#undef vl_subscr
#undef b_ref
#undef b_subscr
#undef a_ref
#undef a_subscr


