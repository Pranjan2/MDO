from math import isnan
import numpy as np
import pandas as pd
import sys
from readwriteData import update_eta, apply_heavisidefilter, readElemList
import matplotlib.pyplot as plt

#------------------------------------------------------------------------------------------------#
#--                                    GET ELEMENT TYPE                                        --#
#------------------------------------------------------------------------------------------------#
def get_Zonetype(id):
    """
    Get element type based on id
    """
    if id == 38: #C3D8
        ZONETYPE = 'FEBRICK'
    elif id ==  34: #C3D4
        ZONETYPE = 'FETETRAHEDRON'
    elif id == 53:  #S3 Shell
        ZONETYPE = 'FETRIANGLE'
    elif id == 54:  #S4 Shell
        ZONETYPE = 'FEQUADRILATERAL'
    return ZONETYPE



#------------------------------------------------------------------------------------------------#
#--                                  HEXAHEDRAL ELEMENTS                                       --#
#------------------------------------------------------------------------------------------------#
def nodal_density_8(Eleid, RHO_FILTERED, E1, E2, E3, E4, E5, E6, E7, E8, Nnodes):
    """
    Convert element density to node densities for element with 8 nodes
    """
    print("Finding elements belonging to a node...")
    densityinterp = np.zeros(Nnodes)    #To store sum of connecting element rhos at node
    #Look for all elements connecting at a node, and add their rhos
    for elcount, elem in enumerate(Eleid):
        rhoe=RHO_FILTERED[elem-1]
        
        densityinterp[E1[elem-1]-1]= max(densityinterp[E1[elem-1]-1],rhoe)
        densityinterp[E2[elem-1]-1]= max(densityinterp[E2[elem-1]-1],rhoe)
        densityinterp[E3[elem-1]-1]= max(densityinterp[E3[elem-1]-1],rhoe)
        densityinterp[E4[elem-1]-1]= max(densityinterp[E4[elem-1]-1],rhoe)
        densityinterp[E5[elem-1]-1]= max(densityinterp[E5[elem-1]-1],rhoe)
        densityinterp[E6[elem-1]-1]= max(densityinterp[E6[elem-1]-1],rhoe)
        densityinterp[E7[elem-1]-1]= max(densityinterp[E7[elem-1]-1],rhoe)
        densityinterp[E8[elem-1]-1]= max(densityinterp[E8[elem-1]-1],rhoe)
    densityinterp_ave = densityinterp.reshape(Nnodes,1) 
    return densityinterp_ave


def interp_hex(mesh,inputrhofile, Nnodes, Nelems, beta):
    #Extract the node information
    Nodes = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2, nrows=Nnodes)

    #Extract the element information
    Element = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2+2+Nnodes, nrows=Nelems)

    #Extract the density information
    Rhos = pd.read_csv(inputrhofile,header = None, sep = ',', nrows=Nelems)

    LOC = Nodes.to_numpy()
    ELE = Element.to_numpy()
    RHO = Rhos.to_numpy()

    RHO_FILTERED=RHO[0:Nelems,1]        
    RHO_FILTERED=RHO_FILTERED.reshape(Nelems,1)
    #RHO_UNFILTERED=RHO[0:Nelems,0]
    #RHO_UNFILTERED=RHO_UNFILTERED.reshape(Nelems,1)

    Eleid = ELE[0:Nelems,0]
    E1 = ELE[0:Nelems,1]
    E2 = ELE[0:Nelems,2]
    E3 = ELE[0:Nelems,3]
    E4 = ELE[0:Nelems,4]
    E5 = ELE[0:Nelems,5]
    E6 = ELE[0:Nelems,6]
    E7 = ELE[0:Nelems,7]
    E8 = ELE[0:Nelems,8]

    Eleid= Eleid.reshape(Nelems,1)
    E1= E1.reshape(Nelems,1)
    E2= E2.reshape(Nelems,1)
    E3= E3.reshape(Nelems,1)
    E4= E4.reshape(Nelems,1)
    E5= E5.reshape(Nelems,1)
    E6= E6.reshape(Nelems,1)
    E7= E7.reshape(Nelems,1)
    E8= E8.reshape(Nelems,1)

    if Eleid[-1]!=Nelems:
        sys.exit("ERROR : Inconsistent number of elements read from mesh file")

    Nodeid = LOC[0:Nnodes,0]
    X = LOC[0:Nnodes,1]
    Y = LOC[0:Nnodes,2]
    Z = LOC[0:Nnodes,3]

    Nodeid= Nodeid.reshape(Nnodes,1)
    X = X.reshape(Nnodes,1)
    Y = Y.reshape(Nnodes,1)
    Z = Z.reshape(Nnodes,1)

    if Nodeid[-1]!=Nnodes:
        sys.exit("ERROR : Inconsistent number of nodes read from mesh file")

    #-----------------------------------------------------------------------------#
    ## Extract nodal density
    #-----------------------------------------------------------------------------#
    densityinterp = nodal_density_8(Eleid, RHO_FILTERED, E1, E2, E3, E4, E5, E6, E7, E8, Nnodes) 

    #-----------------------------------------------------------------------------#
    ## Extract nodal density with volume preserving threshold
    #-----------------------------------------------------------------------------#
    if beta>0:
        """
        Project final result using volume preserving threshold
        """
        eta = update_eta(RHO_FILTERED, beta)
        eta= np.round(eta, 3)
        print('Eta:', eta)
        RHO_FILTERED_H = apply_heavisidefilter(RHO_FILTERED, eta, beta)[0]
        densityinterp_H = nodal_density_8(Eleid, RHO_FILTERED_H, E1, E2, E3, E4, E5, E6, E7, E8, Nnodes) 

    return Nodeid, X, Y, Z, Eleid, E1, E2, E3, E4, E5, E6, E7, E8, densityinterp, densityinterp_H, eta


def write_C3D8(mesh, Nnodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename):
    '''
    Write tecplot file for C3D8 elements 
    '''
    if isinstance(passivefilename, str):
        print('Getting passive elements')
        passiveEl = readElemList(passivefilename)-1
    else:
        passiveEl = np.nan

    fullElList = np.arange(0, Nelems, dtype = int)
    designEl = np.setdiff1d(fullElList, passiveEl)

    Nodeid, X, Y, Z, Eleid, E1, E2, E3, E4, E5, E6, E7, E8, densityinterp, densityinterp_H, eta= interp_hex(mesh, rho, Nnodes, Nelems, beta)

    file = open(outputfile, "w")
    file.write("TITLE = \"Visualization of the solid solution\"")
    file.write("\n")
    file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
    file.write("\n")
    file.write("ZONE STRANDID=1, SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(designEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
    for i in range(Nnodes):
        file.write("\n" +"{:.5e}" .format(X[i,0]) +'  \t  '+ "{:.5e}".format(Y[i,0]) + ' \t \t ' + "{:.5e}".format(Z[i,0]) + ' \t ' +"{:.5e}".format(densityinterp[i,0])+ ' \t ' +"{:.5e}".format(densityinterp_H[i,0]))

    for i in designEl:
        file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0])+ ' \t ' +str(E4[i,0]) +' \t '+str(E5[i,0]) +' \t '+ str(E6[i,0]) + ' \t ' + str(E7[i,0])+ ' \t ' +str(E8[i,0]))
    file.write("\n")


    # Write passive elements separately if defined
    if ~np.isnan(passiveEl).any():
        file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
        file.write("\n")    
        file.write("ZONE STRANDID=2,VARSHARELIST=([1,2,3,4,5]), SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(passiveEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
        for i in passiveEl:
            file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0])+ ' \t ' +str(E4[i,0]) +' \t '+str(E5[i,0]) +' \t '+ str(E6[i,0]) + ' \t ' + str(E7[i,0])+ ' \t ' +str(E8[i,0]))
        file.write("\n")
    
    file.close()



#------------------------------------------------------------------------------------------------#
#--                                  TETRAHEDRAL ELEMENTS                                      --#
#------------------------------------------------------------------------------------------------#

def nodal_density_4(Eleid, RHO_FILTERED, E1, E2, E3, E4, Nnodes):
    """
    Convert element density to node densities for element with 4 nodes
    """
    print("Finding elements belonging to a node...")
    densityinterp = np.zeros(Nnodes)    #To store sum of connecting element rhos at node
    #Look for all elements connecting at a node, and add their rhos
    for elcount, elem in enumerate(Eleid):
        rhoe=RHO_FILTERED[elem-1]
        
        densityinterp[E1[elem-1]-1]= max(densityinterp[E1[elem-1]-1],rhoe)
        densityinterp[E2[elem-1]-1]= max(densityinterp[E2[elem-1]-1],rhoe)
        densityinterp[E3[elem-1]-1]= max(densityinterp[E3[elem-1]-1],rhoe)
        densityinterp[E4[elem-1]-1]= max(densityinterp[E4[elem-1]-1],rhoe)
    densityinterp_ave = densityinterp.reshape(Nnodes,1) 
    return densityinterp_ave


def interp_tet(mesh,inputrhofile, Nnodes, Nelems, beta):
    #Extract the node information
    Nodes = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2, nrows=Nnodes)

    #Extract the element information
    Element = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2+2+Nnodes, nrows=Nelems)

    #Extract the density information
    Rhos = pd.read_csv(inputrhofile,header = None, sep = ',', nrows=Nelems)

    LOC = Nodes.to_numpy()
    ELE = Element.to_numpy()
    RHO = Rhos.to_numpy()

    RHO_FILTERED=RHO[0:Nelems,1]        
    RHO_FILTERED=RHO_FILTERED.reshape(Nelems,1)

    Eleid = ELE[0:Nelems,0]
    E1 = ELE[0:Nelems,1]
    E2 = ELE[0:Nelems,2]
    E3 = ELE[0:Nelems,3]
    E4 = ELE[0:Nelems,4]

    Eleid= Eleid.reshape(Nelems,1)
    E1= E1.reshape(Nelems,1)
    E2= E2.reshape(Nelems,1)
    E3= E3.reshape(Nelems,1)
    E4= E4.reshape(Nelems,1)

    if Eleid[-1]!=Nelems:
        sys.exit("ERROR : Inconsistent number of elements read from mesh file")

    Nodeid = LOC[0:Nnodes,0]
    X = LOC[0:Nnodes,1]
    Y = LOC[0:Nnodes,2]
    Z = LOC[0:Nnodes,3]

    Nodeid= Nodeid.reshape(Nnodes,1)
    X = X.reshape(Nnodes,1)
    Y = Y.reshape(Nnodes,1)
    Z = Z.reshape(Nnodes,1)

    if Nodeid[-1]!=Nnodes:
        sys.exit("ERROR : Inconsistent number of nodes read from mesh file")

    #-----------------------------------------------------------------------------#
    ## Extract nodal density
    #-----------------------------------------------------------------------------#
    densityinterp = nodal_density_4(Eleid, RHO_FILTERED, E1, E2, E3, E4, Nnodes) 

    #-----------------------------------------------------------------------------#
    ## Extract nodal density with volume preserving threshold
    #-----------------------------------------------------------------------------#
    if beta>0:
        """
        Project final result using volume preserving threshold
        """
        eta = update_eta(RHO_FILTERED, beta)
        eta = np.round(eta, 3)
        print('Eta:', eta)
        RHO_FILTERED_H = apply_heavisidefilter(RHO_FILTERED, eta, beta)[0]
        densityinterp_H = nodal_density_4(Eleid, RHO_FILTERED_H, E1, E2, E3, E4, Nnodes) 

    return Nodeid, X, Y, Z, Eleid, E1, E2, E3, E4, densityinterp, densityinterp_H, eta



def write_C3D4(mesh, NNodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename):
    '''
    Write tecplot file for C3D4 elements 
    '''
    designEl = np.arange(0, Nelems, dtype = int)
    ZONEID = 1
    for eachFilename in passivefilename:
        if isinstance(eachFilename, str):
            print('To determine design domain, getting element set from file ' + eachFilename)
            passiveEl = readElemList(eachFilename)-1
        else:
            passiveEl = np.array(np.nan)

        designEl = np.setdiff1d(designEl, passiveEl)

    Nodeid, X, Y, Z, Eleid, E1, E2, E3, E4, densityinterp, densityinterp_H, eta= interp_tet(mesh, rho, Nnodes, Nelems, beta)

    file = open(outputfile, "w")
    file.write("TITLE = \"Visualization of the solid solution\"")
    file.write("\n")
    file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
    file.write("\n")
    file.write("ZONE STRANDID="+str(ZONEID)+", SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(designEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
    for i in range(Nnodes):
        file.write("\n" +"{:.5e}" .format(X[i,0]) +'  \t  '+ "{:.5e}".format(Y[i,0]) + ' \t \t ' + "{:.5e}".format(Z[i,0]) + ' \t ' +"{:.5e}".format(densityinterp[i,0])+ ' \t ' +"{:.5e}".format(densityinterp_H[i,0]))

    for i in designEl:
        file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0])+ ' \t ' +str(E4[i,0]))
    file.write("\n")

    # Write passive elements separately if defined
    for eachFilename in passivefilename:
        if isinstance(eachFilename, str):
            print('To write set, getting element set from file ' + eachFilename)
            passiveEl = readElemList(eachFilename)-1
            ZONEID = ZONEID + 1
            file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
            file.write("\n")
            file.write("ZONE STRANDID="+str(ZONEID)+",VARSHARELIST=([1,2,3,4,5]), SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(passiveEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
            for i in passiveEl:
                file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0])+ ' \t ' +str(E4[i,0]))
            file.write("\n")
        
    file.close()




#------------------------------------------------------------------------------------------------#
#--                                  S3 SHELL ELEMENTS                                         --#
#------------------------------------------------------------------------------------------------#

def nodal_density_3(Eleid, RHO_FILTERED, E1, E2, E3, Nnodes):
    """
    Convert element density to node densities for element with 3 nodes
    s3
    """
    print("Finding elements belonging to a node...")
    densityinterp = np.zeros(Nnodes)    #To store sum of connecting element rhos at node
    #Look for all elements connecting at a node, and add their rhos
    for elcount, elem in enumerate(Eleid):
        rhoe=RHO_FILTERED[elem-1]
        
        densityinterp[E1[elem-1]-1]= max(densityinterp[E1[elem-1]-1],rhoe)
        densityinterp[E2[elem-1]-1]= max(densityinterp[E2[elem-1]-1],rhoe)
        densityinterp[E3[elem-1]-1]= max(densityinterp[E3[elem-1]-1],rhoe)
    densityinterp_ave = densityinterp.reshape(Nnodes,1) 
    return densityinterp_ave


def interp_S3(mesh,inputrhofile, Nnodes, Nelems, beta):
    #Extract the node information
    Nodes = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2, nrows=Nnodes)

    #Extract the element information
    Element = pd.read_csv(mesh,header = None, sep = ',', skiprows = 2+2+Nnodes, nrows=Nelems)

    #Extract the density information
    Rhos = pd.read_csv(inputrhofile,header = None, sep = ',', nrows=Nelems)

    LOC = Nodes.to_numpy()
    ELE = Element.to_numpy()
    RHO = Rhos.to_numpy()

    RHO_FILTERED=RHO[0:Nelems,1]        
    RHO_FILTERED=RHO_FILTERED.reshape(Nelems,1)

    Eleid = ELE[0:Nelems,0]
    E1 = ELE[0:Nelems,1]
    E2 = ELE[0:Nelems,2]
    E3 = ELE[0:Nelems,3]

    Eleid= Eleid.reshape(Nelems,1)
    E1= E1.reshape(Nelems,1)
    E2= E2.reshape(Nelems,1)
    E3= E3.reshape(Nelems,1)

    if Eleid[-1]!=Nelems:
        sys.exit("ERROR : Inconsistent number of elements read from mesh file")

    Nodeid = LOC[0:Nnodes,0]
    X = LOC[0:Nnodes,1]
    Y = LOC[0:Nnodes,2]
    Z = LOC[0:Nnodes,3]

    Nodeid= Nodeid.reshape(Nnodes,1)
    X = X.reshape(Nnodes,1)
    Y = Y.reshape(Nnodes,1)
    Z = Z.reshape(Nnodes,1)

    if Nodeid[-1]!=Nnodes:
        sys.exit("ERROR : Inconsistent number of nodes read from mesh file")

    #-----------------------------------------------------------------------------#
    ## Extract nodal density
    #-----------------------------------------------------------------------------#
    densityinterp = nodal_density_3(Eleid, RHO_FILTERED, E1, E2, E3, Nnodes) 

    #-----------------------------------------------------------------------------#
    ## Extract nodal density with volume preserving threshold
    #-----------------------------------------------------------------------------#
    if beta>0:
        """
        Project final result using volume preserving threshold
        """
        eta = update_eta(RHO_FILTERED, beta)
        eta = np.round(eta, 3)
        print('Eta:', eta)
        RHO_FILTERED_H = apply_heavisidefilter(RHO_FILTERED, eta, beta)[0]
        densityinterp_H = nodal_density_3(Eleid, RHO_FILTERED_H, E1, E2, E3, Nnodes) 

    return Nodeid, X, Y, Z, Eleid, E1, E2, E3, densityinterp, densityinterp_H, eta


def write_S3(mesh, NNodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename):
    '''
    Write tecplot file for S3 elements 
    '''
    designEl = np.arange(0, Nelems, dtype = int)
    ZONEID = 1
    for eachFilename in passivefilename:
        if isinstance(eachFilename, str):
            print('To determine design domain, getting element set from file ' + eachFilename)
            passiveEl = readElemList(eachFilename)-1
        else:
            passiveEl = np.array(np.nan)

        designEl = np.setdiff1d(designEl, passiveEl)

    Nodeid, X, Y, Z, Eleid, E1, E2, E3, densityinterp, densityinterp_H, eta= interp_S3(mesh, rho, Nnodes, Nelems, beta)

    file = open(outputfile, "w")
    file.write("TITLE = \"Visualization of the shell solution\"")
    file.write("\n")
    file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
    file.write("\n")
    file.write("ZONE STRANDID="+str(ZONEID)+", SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(designEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
    for i in range(Nnodes):
        file.write("\n" +"{:.5e}" .format(X[i,0]) +'  \t  '+ "{:.5e}".format(Y[i,0]) + ' \t \t ' + "{:.5e}".format(Z[i,0]) + ' \t ' +"{:.5e}".format(densityinterp[i,0])+ ' \t ' +"{:.5e}".format(densityinterp_H[i,0]))

    for i in designEl:
        file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0]))
    file.write("\n")

    # Write passive elements separately if defined
    for eachFilename in passivefilename:
        if isinstance(eachFilename, str):
            print('To write set, getting element set from file ' + eachFilename)
            passiveEl = readElemList(eachFilename)-1
            ZONEID = ZONEID + 1
            file.write("VARIABLES = \"x\"\"y\"\"z\"\"Rho\"\"RhoH"+str(eta)+"\"")
            file.write("\n")
            file.write("ZONE STRANDID="+str(ZONEID)+",VARSHARELIST=([1,2,3,4,5]), SOLUTIONTIME=1, NODES="+str(Nnodes)+", ELEMENTS="+ str(np.size(passiveEl))+", DATAPACKING=POINT, ZONETYPE="+get_Zonetype(elementType))
            for i in passiveEl:
                file.write("\n" + str(E1[i,0]) +' \t '+ str(E2[i,0]) + ' \t ' + str(E3[i,0]))
            file.write("\n")
        
    file.close()


#------------------------------------------------------------------------------------------------#
#--                                  Write file                                                --#
#------------------------------------------------------------------------------------------------#
def write2tecio(mesh, passivefilename, Nnodes, Nelems, elementType, iterList, beta = 1000):
    """
    Convert from caltop to tecio format.
    Inputs: mesh filename, passiveElementfilename, NumNodes, NumElements, ElementType, iteration List
    Note: Element Types: 38= C3D8, 34=C3D4, 53=S3
    Example : write2tecio('mesh.MSH', 'passiveElemList.nam', 5000, 400, 38, np.arange([0]))

    """
    for iter in iterList: # 0: only converged solution
        print("Iteration number:"+str(iter))      
        if iter == 0:
            rho = 'rhos.dat'
        else:
            rho = "rho"+str(iter)+".dat"
        
        outputfile= "output"+str(iter)+".dat"
           
        print("Creating tecplot file for Element Type: "+ get_Zonetype(elementType))
        if elementType == 38:
            write_C3D8(mesh, Nnodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename)
            print("Complete!")
        elif elementType == 34:
            write_C3D4(mesh, Nnodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename)
            print("Complete!")
        elif elementType == 53:
            write_S3(mesh, Nnodes, Nelems, rho, outputfile, iter, elementType, beta, passivefilename)
        
    print("Done!")
        
if __name__ == "__main__":

    mesh = "messh.msh"
    passivefilename = ['skinElementList.nam'] #, 'sparElementList.nam'] #'passiveElemList.nam'   # elements excluded from design doamin, put a randon integer if empty
    Nnodes= 249635
    Nelems= 529460
    elementType = 53 # 38= C3D8, 34=C3D4, 54=S4, 53=S3
    iterList = np.array([0])  # array of iterations to plot

    import sys

    argc = len(sys.argv)
    if argc > 1:
        for i, argv in enumerate(sys.argv):
            if argv == "-n": Nnodes = int(sys.argv[i+1])
            elif argv == "-e": Nelems = int(sys.argv[i+1])
            elif argv == "-m": mesh = str(sys.argv[i+1])
            elif argv == "-t": elementType = int(sys.argv[i+1])

    print("**--------------------------------------------**")
    print("Mesh file:", mesh)
    print("Number of Nodes:", Nnodes)
    print("Number of Elements :", Nelems)
    print("Element type:", get_Zonetype(elementType))

    write2tecio(mesh, passivefilename, Nnodes, Nelems, elementType, iterList)
    