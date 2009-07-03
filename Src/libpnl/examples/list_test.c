
/*************************************************************************/
/* Written and (C) by David Pommier <david.pommier@gmail.com>            */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pnl_list.h"
#include "tests.h"

/*test des fonctions pnl_sortlist_ */


static void pnl_sort_list_add_test()
{
  PnlContains * Cm1=pnl_contains_create(-1,1.);
  PnlContains * Cm2=pnl_contains_create(-2,1.);
  PnlContains * Cm3=pnl_contains_create(-3,1.);
  PnlContains * Cm4=pnl_contains_create(-4,1.);
  PnlContains * Cm5=pnl_contains_create(-5,1.);
  PnlContains * Cm6=pnl_contains_create(-6,1.);
  PnlContains * Cm7=pnl_contains_create(-7,1.);
  PnlContains * Cm8=pnl_contains_create(-8,1.);
  PnlContains * C1=pnl_contains_create(1,1.);
  PnlContains * C2=pnl_contains_create(2,1.);
  PnlContains * C3=pnl_contains_create(3,1.);
  PnlContains * C4=pnl_contains_create(4,1.);
  PnlContains * C5=pnl_contains_create(5,1.);
  PnlContains * C6=pnl_contains_create(6,1.);
  PnlContains * C7=pnl_contains_create(7,1.);
  PnlContains * C8=pnl_contains_create(8,1.);

  PnlSortList * L=pnl_sort_list_create();
  pnl_sort_list_add(L,C3);
  pnl_sort_list_add(L,C2);
  pnl_sort_list_add(L,C4);
  pnl_sort_list_add(L,C8);
  pnl_sort_list_add(L,C5);
  pnl_sort_list_add(L,Cm6);
  pnl_sort_list_add(L,C7);
  pnl_sort_list_add(L,Cm2);
  pnl_sort_list_add(L,C1);
  pnl_sort_list_add(L,Cm1);
  pnl_sort_list_add(L,Cm6);
  pnl_sort_list_add(L,Cm1);
  pnl_sort_list_add(L,C8);
  pnl_sort_list_add(L,C3);
  pnl_sort_list_add(L,C2);
  pnl_sort_list_add(L,C4);
  printf("{-6,-2,-1,1,2,3,4,5,7,8)\n");
  printf("{2,1,2,1,2,2,2,1,1,2)\n");
  pnl_sort_list_print(L);
  pnl_sort_list_free(&L);
  free(C1);
  free(C2);
  free(C3);
  free(C4);
  free(C5);
  free(C6);
  free(C7);
  free(C8);
  free(Cm1);
  free(Cm2);
  free(Cm3);
  free(Cm4);
  free(Cm5);
  free(Cm6);
  free(Cm7);
  free(Cm8);

}

static void pnl_sort_list_add_dicho_test()
{
  PnlContains * Cm1=pnl_contains_create(-1,1.);
  PnlContains * Cm2=pnl_contains_create(-2,1.);
  PnlContains * Cm3=pnl_contains_create(-3,1.);
  PnlContains * Cm4=pnl_contains_create(-4,1.);
  PnlContains * Cm5=pnl_contains_create(-5,1.);
  PnlContains * Cm6=pnl_contains_create(-6,1.);
  PnlContains * Cm7=pnl_contains_create(-7,1.);
  PnlContains * Cm8=pnl_contains_create(-8,1.);
  PnlContains * C1=pnl_contains_create(1,1.);
  PnlContains * C2=pnl_contains_create(2,1.);
  PnlContains * C3=pnl_contains_create(3,1.);
  PnlContains * C4=pnl_contains_create(4,1.);
  PnlContains * C5=pnl_contains_create(5,1.);
  PnlContains * C6=pnl_contains_create(6,1.);
  PnlContains * C7=pnl_contains_create(7,1.);
  PnlContains * C8=pnl_contains_create(8,1.);

  PnlSortList * L=pnl_sort_list_create();
  pnl_sort_list_add_dicho(L,C3);
  pnl_sort_list_add_dicho(L,C2);
  pnl_sort_list_add_dicho(L,C4);
  pnl_sort_list_add_dicho(L,C8);
  pnl_sort_list_add_dicho(L,C5);
  pnl_sort_list_add_dicho(L,Cm6);
  pnl_sort_list_add_dicho(L,C7);
  pnl_sort_list_add_dicho(L,Cm2);
  pnl_sort_list_add_dicho(L,C1);
  pnl_sort_list_add_dicho(L,Cm1);
  pnl_sort_list_add_dicho(L,C3);
  pnl_sort_list_add_dicho(L,Cm6);
  pnl_sort_list_add_dicho(L,Cm1);
  pnl_sort_list_add_dicho(L,C8);
  pnl_sort_list_add_dicho(L,Cm2);
  pnl_sort_list_add_dicho(L,C2);
  pnl_sort_list_add_dicho(L,C4);
  printf("{-6,-2,-1,1,3,4,5,7,8)\n");
  printf("{2,2,2,1,2,2,2,1,1,2)\n");
  pnl_sort_list_print(L);
  pnl_sort_list_free(&L);
  free(C1);
  free(C2);
  free(C3);
  free(C4);
  free(C5);
  free(C6);
  free(C7);
  free(C8);
  free(Cm1);
  free(Cm2);
  free(Cm3);
  free(Cm4);
  free(Cm5);
  free(Cm6);
  free(Cm7);
  free(Cm8);
}

static void all_list_test ();
static list list_tests[] =
  {
    MAKE_ENUM(1, all_list_test),
    MAKE_ENUM(2, pnl_sort_list_add_test),
    MAKE_ENUM(3, pnl_sort_list_add_dicho_test),
    MAKE_ENUM(NULL_INT, NULL)
  };

static void all_list_test ()
{
  int len=0;
  while (list_tests[len].id != NULL_INT)
    {
      if (list_tests[len].func != all_list_test) (list_tests[len].func)();
      len ++;
    }
}

void list_test()
{
  int len=0, n=0, choice;

  while (list_tests[len].id != NULL_INT) len++;
        
    
  while (list_tests[n].id != NULL_INT)
    {
      printf("%2d. %s\n",  list_tests[n].id, list_tests[n].label);
      n ++;
      if (n/10 == (double)n/10.0)
        {
          printf("Which test do you want to run? (type 0 to continue displaying the list)\n");
          while(1)
            {
              scanf("%d", &choice);
              if (choice ==0) break;
              choice --;
              if (choice <0 || choice > len) printf("illegal choice\n");
              else { (list_tests[choice].func)(); return; }
            }
        }
    }
  printf("Which test do you want to run?\n");
  while(1)
    {
      scanf("%d", &choice); choice --;
      if (choice <0 || choice > len) printf("illegal choice\n");
      else { (list_tests[choice].func)(); break; }
    }
}

