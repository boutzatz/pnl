
#include "pnl_f2c.h"

/* Subroutine */ int zlacrt_(integer *n, doublecomplex *cx, integer *incx, 
	doublecomplex *cy, integer *incy, doublecomplex *c__, doublecomplex *
	s)
{
/*  -- LAPACK auxiliary routine (version 3.0) --   
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,   
       Courant Institute, Argonne National Lab, and Rice University   
       October 31, 1992   


    Purpose   
    =======   

    ZLACRT performs the operation   

       (  c  s )( x )  ==> ( x )   
       ( -s  c )( y )      ( y )   

    where c and s are complex and the vectors x and y are complex.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The number of elements in the vectors CX and CY.   

    CX      (input/output) COMPLEX*16 array, dimension (N)   
            On input, the vector x.   
            On output, CX is overwritten with c*x + s*y.   

    INCX    (input) INTEGER   
            The increment between successive values of CX.  INCX <> 0.   

    CY      (input/output) COMPLEX*16 array, dimension (N)   
            On input, the vector y.   
            On output, CY is overwritten with -s*x + c*y.   

    INCY    (input) INTEGER   
            The increment between successive values of CY.  INCY <> 0.   

    C       (input) COMPLEX*16   
    S       (input) COMPLEX*16   
            C and S define the matrix   
               [  C   S  ].   
               [ -S   C  ]   

   =====================================================================   


       Parameter adjustments */
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublecomplex z__1, z__2, z__3;
    /* Local variables */
    static integer i__;
    static doublecomplex ctemp;
    static integer ix, iy;

    --cy;
    --cx;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }

/*     Code for unequal increments or equal increments not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = ix;
	z__2.r = c__->r * cx[i__2].r - c__->i * cx[i__2].i, z__2.i = c__->r * 
		cx[i__2].i + c__->i * cx[i__2].r;
	i__3 = iy;
	z__3.r = s->r * cy[i__3].r - s->i * cy[i__3].i, z__3.i = s->r * cy[
		i__3].i + s->i * cy[i__3].r;
	z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
	ctemp.r = z__1.r, ctemp.i = z__1.i;
	i__2 = iy;
	i__3 = iy;
	z__2.r = c__->r * cy[i__3].r - c__->i * cy[i__3].i, z__2.i = c__->r * 
		cy[i__3].i + c__->i * cy[i__3].r;
	i__4 = ix;
	z__3.r = s->r * cx[i__4].r - s->i * cx[i__4].i, z__3.i = s->r * cx[
		i__4].i + s->i * cx[i__4].r;
	z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
	cy[i__2].r = z__1.r, cy[i__2].i = z__1.i;
	i__2 = ix;
	cx[i__2].r = ctemp.r, cx[i__2].i = ctemp.i;
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    return 0;

/*     Code for both increments equal to 1 */

L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	z__2.r = c__->r * cx[i__2].r - c__->i * cx[i__2].i, z__2.i = c__->r * 
		cx[i__2].i + c__->i * cx[i__2].r;
	i__3 = i__;
	z__3.r = s->r * cy[i__3].r - s->i * cy[i__3].i, z__3.i = s->r * cy[
		i__3].i + s->i * cy[i__3].r;
	z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
	ctemp.r = z__1.r, ctemp.i = z__1.i;
	i__2 = i__;
	i__3 = i__;
	z__2.r = c__->r * cy[i__3].r - c__->i * cy[i__3].i, z__2.i = c__->r * 
		cy[i__3].i + c__->i * cy[i__3].r;
	i__4 = i__;
	z__3.r = s->r * cx[i__4].r - s->i * cx[i__4].i, z__3.i = s->r * cx[
		i__4].i + s->i * cx[i__4].r;
	z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
	cy[i__2].r = z__1.r, cy[i__2].i = z__1.i;
	i__2 = i__;
	cx[i__2].r = ctemp.r, cx[i__2].i = ctemp.i;
/* L30: */
    }
    return 0;
} /* zlacrt_ */

