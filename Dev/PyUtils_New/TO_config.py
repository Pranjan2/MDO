#!/usr/bin/env python3

from __future__ import print_function
import nlopt
import numpy as np
from callrunccx import callrunccx
from getVolconstraint import getVolconstraint
import readwriteData as rwd

#################################################################################################3

def TopOpt(restartFlag):
    ccxversion="ccx_2.15_mkleqnnz"

    inp= "tet"

    dimension= 316715

    volfrac=0.2
    
    rmin=0.006
    
    p=3

    passivefilename = 'passive_el_set.nam'   # elements excluded from design doamin

    callrunccx.nnz = 300 # Number of nonzeros in each row of density filter assumed

    callrunccx.counter=0    #Overall iteration counter

    callrunccx.q =3     #Filter order start

    callrunccx.q_max=3   #Max filter order

    callrunccx.q_iter = 30  #Iteration number to update the fitler order

    callrunccx.q_factor = 1.5 #Factor for filter order update

    #restartFlag = False

    plotIterationWiseFlag = False # True if post process for each TO iteration 
    
    compliance_scaling = 10**6

    volume_scaling =10**6

    #########################################################################################
    #########################################################################################
    opt = nlopt.opt(nlopt.LD_MMA, dimension)
    #opt = nlopt.opt(nlopt.LD_SLSQP, dimension)
    #opt = nlopt.opt(nlopt.LD_CCSAQ, dimension)
    outputCompliance="Obj_iterations.dat"

    lower=0.00001*np.ones(dimension)
    upper=0.999999*np.ones(dimension)

    if restartFlag:
        x0 = rwd.rho_column("density_FSI.dat")
    else:
        x0 = volfrac*np.ones(dimension)

    # Initialize passive elements to 1
    rwd.parse_fea_set(passivefilename, 'pse.nam')   
    el = rwd.read_fea_set('pse.nam')
    rwd.value_fea_set(x0, el, 0.99)

    # DV Box bounds
    opt.set_lower_bounds(lower)
    opt.set_upper_bounds(upper)

    # Objective function
    f=lambda x,grad: callrunccx(x,grad,ccxversion,inp,volfrac,rmin,p,outputCompliance, plotIterationWiseFlag, compliance_scaling)


    ff = open(outputCompliance, "w")
    ff.write("#Contains all compliance in each iteration")
    ff.close()


    opt.set_min_objective(f)

    # Volume constraint
    opt.add_inequality_constraint(lambda x, grad: getVolconstraint(x,grad,ccxversion,inp,volfrac,rmin,p, volume_scaling), 1e-8)
    #opt.add_inequality_constraint(lambda x, grad: myconstraint(x,grad, -1, 1), 1e-8)
    #opt.set_ftol_rel(1e-4)
    opt.set_xtol_rel(1e-3)
    x = opt.optimize(x0)
    minf = opt.last_optimum_value()
    #print('optimum at ', x)
    print('minimum value = ', minf)
    print('result code = ', opt.last_optimize_result())
    print('nevals = ', opt.get_numevals())
    #print('initial step =', opt.get_initial_step(x0))

    return 0


if __name__=='__main__':

    restartFlag = False
    topopt = TopOpt(restartFlag)
