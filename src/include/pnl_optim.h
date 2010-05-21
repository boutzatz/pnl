#ifndef PNL_INTPOINTS_BFGS_H
#define PNL_INTPOINTS_BFGS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "pnl_mathtools.h"
#include "pnl_vector.h"
#include "pnl_matrix.h"


extern int pnl_optim_intpoints_bfgs_solve(PnlRnFuncR * func, PnlRnFuncRm *grad_func, PnlRnFuncRm * nl_constraints, PnlVect* lower_bounds, PnlVect* upper_bounds, PnlVect * x_input , double tolerance, int iter_max, int print_inner_steps, PnlVect *output);


#endif // PNL_INTPOINTS_BFGS_H
