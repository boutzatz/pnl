/* dormtr.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "pnl/pnl_f2c.h"

/* Table of constant values */

static int c__1 = 1;
static int c_n1 = -1;
static int c__2 = 2;

 int dormtr_(char *side, char *uplo, char *trans, int *m, 
	int *n, double *a, int *lda, double *tau, double *
	c__, int *ldc, double *work, int *lwork, int *info)
{
    /* System generated locals */
    address a__1[2];
    int a_dim1, a_offset, c_dim1, c_offset, i__1[2], i__2, i__3;
    char ch__1[2];

    /* Builtin functions */
     int s_cat(char *, char **, int *, int *, unsigned long);

    /* Local variables */
    int i1, i2, nb, mi, ni, nq, nw;
    int left;
    extern int lsame_(char *, char *);
    int iinfo;
    int upper;
    extern  int xerbla_(char *, int *);
    extern int ilaenv_(int *, char *, char *, int *, int *, 
	    int *, int *);
    extern  int dormql_(char *, char *, int *, int *, 
	    int *, double *, int *, double *, double *, 
	    int *, double *, int *, int *), 
	    dormqr_(char *, char *, int *, int *, int *, 
	    double *, int *, double *, double *, int *, 
	    double *, int *, int *);
    int lwkopt;
    int lquery;


/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DORMTR overwrites the general float M-by-N matrix C with */

/*                  SIDE = 'L'     SIDE = 'R' */
/*  TRANS = 'N':      Q * C          C * Q */
/*  TRANS = 'T':      Q**T * C       C * Q**T */

/*  where Q is a float orthogonal matrix of order nq, with nq = m if */
/*  SIDE = 'L' and nq = n if SIDE = 'R'. Q is defined as the product of */
/*  nq-1 elementary reflectors, as returned by DSYTRD: */

/*  if UPLO = 'U', Q = H(nq-1) . . . H(2) H(1); */

/*  if UPLO = 'L', Q = H(1) H(2) . . . H(nq-1). */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': apply Q or Q**T from the Left; */
/*          = 'R': apply Q or Q**T from the Right. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U': Upper triangle of A contains elementary reflectors */
/*                 from DSYTRD; */
/*          = 'L': Lower triangle of A contains elementary reflectors */
/*                 from DSYTRD. */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N':  No transpose, apply Q; */
/*          = 'T':  Transpose, apply Q**T. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension */
/*                               (LDA,M) if SIDE = 'L' */
/*                               (LDA,N) if SIDE = 'R' */
/*          The vectors which define the elementary reflectors, as */
/*          returned by DSYTRD. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */
/*          LDA >= MAX(1,M) if SIDE = 'L'; LDA >= MAX(1,N) if SIDE = 'R'. */

/*  TAU     (input) DOUBLE PRECISION array, dimension */
/*                               (M-1) if SIDE = 'L' */
/*                               (N-1) if SIDE = 'R' */
/*          TAU(i) must contain the scalar factor of the elementary */
/*          reflector H(i), as returned by DSYTRD. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the M-by-N matrix C. */
/*          On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= MAX(1,M). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If SIDE = 'L', LWORK >= MAX(1,N); */
/*          if SIDE = 'R', LWORK >= MAX(1,M). */
/*          For optimum performance LWORK >= N*NB if SIDE = 'L', and */
/*          LWORK >= M*NB if SIDE = 'R', where NB is the optimal */
/*          blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    left = lsame_(side, "L");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;

/*     NQ is the order of Q and NW is the minimum dimension of WORK */

    if (left) {
	nq = *m;
	nw = *n;
    } else {
	nq = *n;
	nw = *m;
    }
    if (! left && ! lsame_(side, "R")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (! lsame_(trans, "N") && ! lsame_(trans, 
	    "T")) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < MAX(1,nq)) {
	*info = -7;
    } else if (*ldc < MAX(1,*m)) {
	*info = -10;
    } else if (*lwork < MAX(1,nw) && ! lquery) {
	*info = -12;
    }

    if (*info == 0) {
	if (upper) {
	    if (left) {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (unsigned long)2);
		i__2 = *m - 1;
		i__3 = *m - 1;
		nb = ilaenv_(&c__1, "DORMQL", ch__1, &i__2, n, &i__3, &c_n1);
	    } else {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (unsigned long)2);
		i__2 = *n - 1;
		i__3 = *n - 1;
		nb = ilaenv_(&c__1, "DORMQL", ch__1, m, &i__2, &i__3, &c_n1);
	    }
	} else {
	    if (left) {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (unsigned long)2);
		i__2 = *m - 1;
		i__3 = *m - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, &i__2, n, &i__3, &c_n1);
	    } else {
/* Writing concatenation */
		i__1[0] = 1, a__1[0] = side;
		i__1[1] = 1, a__1[1] = trans;
		s_cat(ch__1, a__1, i__1, &c__2, (unsigned long)2);
		i__2 = *n - 1;
		i__3 = *n - 1;
		nb = ilaenv_(&c__1, "DORMQR", ch__1, m, &i__2, &i__3, &c_n1);
	    }
	}
	lwkopt = MAX(1,nw) * nb;
	work[1] = (double) lwkopt;
    }

    if (*info != 0) {
	i__2 = -(*info);
	xerbla_("DORMTR", &i__2);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0 || nq == 1) {
	work[1] = 1.;
	return 0;
    }

    if (left) {
	mi = *m - 1;
	ni = *n;
    } else {
	mi = *m;
	ni = *n - 1;
    }

    if (upper) {

/*        Q was determined by a call to DSYTRD with UPLO = 'U' */

	i__2 = nq - 1;
	dormql_(side, trans, &mi, &ni, &i__2, &a[(a_dim1 << 1) + 1], lda, &
		tau[1], &c__[c_offset], ldc, &work[1], lwork, &iinfo);
    } else {

/*        Q was determined by a call to DSYTRD with UPLO = 'L' */

	if (left) {
	    i1 = 2;
	    i2 = 1;
	} else {
	    i1 = 1;
	    i2 = 2;
	}
	i__2 = nq - 1;
	dormqr_(side, trans, &mi, &ni, &i__2, &a[a_dim1 + 2], lda, &tau[1], &
		c__[i1 + i2 * c_dim1], ldc, &work[1], lwork, &iinfo);
    }
    work[1] = (double) lwkopt;
    return 0;

/*     End of DORMTR */

} /* dormtr_ */
