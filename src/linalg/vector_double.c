
/************************************************************************/
/* Copyright J�r�me Lelong <jerome.lelong@gmail.com>                    */
/* Copyright C�line Labart                                              */
/*                                                                      */
/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU Lesser General Public License as       */
/* published by the Free Software Foundation, either version 3 of the   */
/* License, or (at your option) any later version.                      */
/*                                                                      */
/* This program is distributed in the hope that it will be useful, but  */
/* WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    */
/* Lesser General Public License for more details.                      */
/*                                                                      */
/* You should have received a copy of the GNU Lesser General Public     */
/* License along with this program.  If not, see                        */
/* <http://www.gnu.org/licenses/>.                                      */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "pnl_matrix.h"
#include "pnl_mathtools.h"

static char pnl_vector_compact_label[] = "PnlVectCompact";

/**
 * Creates a new PnlVectCompact
 * @return a pointeur to PnlVectCompact
 */
PnlVectCompact* pnl_vect_compact_new ()
{
  PnlVectCompact *o;
  if ( (o=malloc (sizeof (PnlVectCompact)))==NULL) return NULL;
  o->object.type = PNL_TYPE_VECTOR_COMPACT;
  o->object.parent_type = PNL_TYPE_OBJECT;
  o->object.label = pnl_vector_compact_label;
  o->size = 0;
  o->convert = 'd';
  o->val = 0;
  return o;
}

/**
 * allocates a PnlVectCompact. By default type='d'.
 * @param n size
 * @param x value to fill the vector
 * @return a pointeur to PnlVectCompact
 */
PnlVectCompact* pnl_vect_compact_create (int n, double x)
{
  PnlVectCompact *v;
  if ((v=malloc (sizeof(PnlVectCompact)))==NULL) return NULL;
  v->size = n;
  v->convert = 'd';
  v->val = x;
  return v;
}

/**
 * resize a PnlVectCompact.
 * @param v the PvlVectCompact to be resized
 * @param size new size
 * @param x new value to set
 * @return OK or WRONG
 */
int pnl_vect_compact_resize (PnlVectCompact *v, int size, double x)
{
  if (v->convert == 'a')
    {
      free (v->array); v->array = NULL;
    }
  v->size = size;
  v->convert = 'd';
  v->val = x;
  return OK;
}

/**
 * copies a PnlVectCompact
 *
 * @param v : a constant PnlVectCompact pointer
 * @return  a PnlVectCompact  pointer initialised with v
 */
PnlVectCompact* pnl_vect_compact_copy(const PnlVectCompact *v)
{
  PnlVectCompact *ret = NULL;

  if (v->convert == 'd')
    ret = pnl_vect_compact_create (v->size, v->val);
  else
    {
      if ((ret=malloc(sizeof(PnlVectCompact)))==NULL) return NULL;
      ret->size = v->size;
      ret->convert = 'a';
      if ((ret->array=malloc(v->size*sizeof(double)))==NULL) return NULL;
      memcpy(ret->array, v->array, sizeof(double)*ret->size);
    }
  return ret;
}


/**
 * free a PnlVectCompact
 * @param v address of a PnlVectCompact
 */
void pnl_vect_compact_free (PnlVectCompact **v)
{
  if ((*v) == NULL) return;
  if ((*v)->convert == 'a' && (*v)->array!= NULL)
    {
      free ((*v)->array); (*v)->array = NULL;
    }
  free (*v); *v=NULL;
}

/**
 * Convert a PnlVectCompact pointeur to a PnlVect pointeur
 * @param C the PnlVectCompact to be expanded
 * @return a PnlVect pointeur
 */
PnlVect* pnl_vect_compact_to_pnl_vect (const PnlVectCompact *C)
{
  PnlVect *v;
  if (C->convert == 'd')
    {
      v = pnl_vect_create_from_double (C->size, C->val);
    }
  else
    {
      v = pnl_vect_create (C->size);
      memcpy (v->array, C->array, C->size*sizeof(double));
    }
  return v;
}

/**
 * access function
 * @param C a PnlVectCompact
 * @param i index
 * @return C[i]
 */
double pnl_vect_compact_get (const PnlVectCompact *C, int i)
{
  if (C->convert == 'd') return C->val;
  CheckIndexVect (C, i);
  return C->array[i];
}
