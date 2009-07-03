#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdio.h>

#ifndef PNL_RANGE_CHECK_OFF
#define CheckIndexVect(v,i) {                                           \
    if (i>=v->size || i<0) {perror("index out of range"); abort();}}
#define CheckVectMatch(lhs, rhs) { if ((lhs)->size != (rhs)->size)  \
      {perror("non compatible dimensions"); abort();}}
#else
#define CheckIndexVect(v,i) {}
#define CheckVectMatch(lhs, rhs){}
#endif /* PNL_RANGE_CHECK_OFF */

#include "pnl_vector_double.h"
#include "pnl_vector_complex.h"
#include "pnl_vector_int.h"
#include "pnl_vector_uint.h"

#endif /* __VECTOR_H__ */
