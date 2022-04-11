/*     CalculiX - A 3-dimensional finite element program                 */
/*              Copyright (C) 1998-2018 Guido Dhondt                          */

/*     This program is free software; you can redistribute it and/or     */
/*     modify it under the terms of the GNU General Public License as    */
/*     published by the Free Software Foundation(version 2);    */
/*                    */

/*     This program is distributed in the hope that it will be useful,   */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of    */ 
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/*     GNU General Public License for more details.                      */

/*     You should have received a copy of the GNU General Public License */
/*     along with this program; if not, write to the Free Software       */
/*     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.         */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "CalculiX.h"
#include "omp.h"
#ifdef SPOOLES
   #include "spooles.h"
#endif
#ifdef SGI
   #include "sgi.h"
#endif
#ifdef TAUCS
   #include "tau.h"
#endif
#ifdef PARDISO
   #include "pardiso.h"
#endif

/* Adapter: Add header */
#include "adapter/PreciceInterface.h"

#define max(a,b) ((a) >= (b) ? (a) : (b))

/**
 * @brief gkdas2 Add displacement v to updated coordinate list. 
 * TO DO: Verify for 2D
 * @param coUpdated 
 * @param v  : latest displacement vector
 * @param nk : number of nodes
 * @param mt : dof index. 1 temperature + 3 displacement
 */
void NLupdateCO(double *coUpdated, double *v, int nk, int mt)
{	
	int i, a, b;
	int num_cpus=0;
	char *env;

	// get num of threads declaration, if any
	env = getenv("OMP_NUM_THREADS");
    if(num_cpus==0){
    	if (env)
      		num_cpus = atoi(env);
    	if (num_cpus < 1) {
      		num_cpus=1;
    	}
    }

	#pragma omp parallel for num_threads(num_cpus) private(a,b)
	for(i = 0; i<= nk-1; i++)
	{
		a = 3*i;
		b = a + i + 1;
		coUpdated[a]+=v[b];
		coUpdated[a+1]+=v[b+1];
		coUpdated[a+2]+=v[b+2];
	}
}

/**
 * @brief gkdas2  Write the updated coordinate list to updatedCOORD.csv. O
 * TO DO: Verify for 2D
 * @param coUpdated 
 * @param nk 
 * @param mt 
 */
void NLwriteUpdatedCO(double *coUpdated, int nk, int mt)
{
    FILE *fp;

    fp = fopen("updatedCOORD.csv", "w+");
	
	fprintf(fp,"NodeID,x,y,z \n");
    for(int i = 0; i <= nk-1; i++)
    {
        fprintf(fp, "%d ,%0.6lf, %0.6lf, %0.6lf \n", i+1, coUpdated[3*i], coUpdated[3*i+1], coUpdated[3*i+2]);
    }
    fclose(fp);
}

void nonlingeo_precice(double **cop, ITG *nk, ITG **konp, ITG **ipkonp, char **lakonp,
	     ITG *ne,
	     ITG *nodeboun, ITG *ndirboun, double *xboun, ITG *nboun,
	     ITG **ipompcp, ITG **nodempcp, double **coefmpcp, char **labmpcp,
             ITG *nmpc,
	     ITG *nodeforc, ITG *ndirforc,double *xforc, ITG *nforc,
	     ITG **nelemloadp, char **sideloadp, double *xload,ITG *nload,
	     ITG *nactdof,
	     ITG **icolp, ITG *jq, ITG **irowp, ITG *neq, ITG *nzl,
	     ITG *nmethod, ITG **ikmpcp, ITG **ilmpcp, ITG *ikboun,
	     ITG *ilboun,
             double *elcon, ITG *nelcon, double *rhcon, ITG *nrhcon,
	     double *alcon, ITG *nalcon, double *alzero, ITG **ielmatp,
	     ITG **ielorienp, ITG *norien, double *orab, ITG *ntmat_,
	     double *t0, double *t1, double *t1old,
	     ITG *ithermal,double *prestr, ITG *iprestr,
	     double **voldp,ITG *iperturb, double *sti, ITG *nzs,
	     ITG *kode, char *filab,
             ITG *idrct, ITG *jmax, ITG *jout, double *timepar,
             double *eme,
	     double *xbounold, double *xforcold, double *xloadold,
             double *veold, double *accold,
	     char *amname, double *amta, ITG *namta, ITG *nam,
             ITG *iamforc, ITG **iamloadp,
             ITG *iamt1, double *alpha, ITG *iexpl,
	     ITG *iamboun, double *plicon, ITG *nplicon, double *plkcon,
             ITG *nplkcon,
             double **xstatep, ITG *npmat_, ITG *istep, double *ttime,
             char *matname, double *qaold, ITG *mi,
             ITG *isolver, ITG *ncmat_, ITG *nstate_, ITG *iumat,
             double *cs, ITG *mcs, ITG *nkon, double **enerp, ITG *mpcinfo,
             char *output,
             double *shcon, ITG *nshcon, double *cocon, ITG *ncocon,
             double *physcon, ITG *nflow, double *ctrl,
             char *set, ITG *nset, ITG *istartset,
             ITG *iendset, ITG *ialset, ITG *nprint, char *prlab,
             char *prset, ITG *nener,ITG *ikforc,ITG *ilforc, double *trab,
             ITG *inotr, ITG *ntrans, double **fmpcp, char *cbody,
             ITG *ibody, double *xbody, ITG *nbody, double *xbodyold,
             ITG *ielprop, double *prop, ITG *ntie, char *tieset,
	     ITG *itpamp, ITG *iviewfile, char *jobnamec, double *tietol,
	     ITG *nslavs, double *thicke, ITG *ics,
	     ITG *nintpoint,ITG *mortar,ITG *ifacecount,char *typeboun,
	     ITG **islavsurfp,double **pslavsurfp,double **clearinip,
	     ITG *nmat,double *xmodal,ITG *iaxial,ITG *inext,ITG *nprop,
	     ITG *network,char *orname,double *vel,ITG *nef,
	     double *velo,double *veloo,
		 /* Adapter: Add variables for the participant name and the config file */
         char * preciceParticipantName, char * configFilename, 
		 /* GKD: Variables for topology optimization*/
		 double *design, double *penal)
		 {

  char description[13]="            ",*lakon=NULL,jobnamef[396]="",
      *sideface=NULL,*labmpc=NULL,*lakonf=NULL,
      *sideloadref=NULL,*sideload=NULL,stiffmatrix[132]=""; 
 
  ITG *inum=NULL,k,iout=0,icntrl,iinc=0,jprint=0,iit=-1,jnz=0,
      icutb=0,istab=0,ifreebody,uncoupled,n1,n2,itruecontact,
      iperturb_sav[2],ilin,*icol=NULL,*irow=NULL,ielas=0,icmd=0,
      memmpc_,mpcfree,icascade,maxlenmpc,*nodempc=NULL,*iaux=NULL,
      *nodempcref=NULL,memmpcref_,mpcfreeref,*itg=NULL,*ineighe=NULL,
      *ieg=NULL,ntg=0,ntr,*kontri=NULL,*nloadtr=NULL,idamping=0,
      *ipiv=NULL,ntri,newstep,mode=-1,noddiam=-1,nasym=0,im,
      ntrit,*inocs=NULL,inewton=0,*ipobody=NULL,*nacteq=NULL,
      *nactdog=NULL,nteq,*itietri=NULL,*koncont=NULL,istrainfree=0,
      ncont,ne0,nkon0,*ipkon=NULL,*kon=NULL,*ielorien=NULL,
      *ielmat=NULL,itp=0,symmetryflag=0,inputformat=0,kscale=1,
      *iruc=NULL,iitterm=0,iturbulent,ngraph=1,ismallsliding=0,
      *ipompc=NULL,*ikmpc=NULL,*ilmpc=NULL,i0ref,irref,icref,
      *itiefac=NULL,*islavsurf=NULL,*islavnode=NULL,*imastnode=NULL,
      *nslavnode=NULL,*nmastnode=NULL,*imastop=NULL,imat,
      *iponoels=NULL,*inoels=NULL,*islavsurfold=NULL,maxlenmpcref,
      *islavact=NULL,mt=mi[1]+1,*nactdofinv=NULL,*ipe=NULL, 
      *ime=NULL,*ikactmech=NULL,nactmech,inode,idir,neold,neini,
      iemchange=0,nzsrad,*mast1rad=NULL,*irowrad=NULL,*icolrad=NULL,
      *jqrad=NULL,*ipointerrad=NULL,*integerglob=NULL,negpres=0,
      mass[2]={0,0}, stiffness=1, buckling=0, rhsi=1, intscheme=0,idiscon=0,
      coriolis=0,*ipneigh=NULL,*neigh=NULL,maxprevcontel,nslavs_prev_step,
      *nelemface=NULL,*ipoface=NULL,*nodface=NULL,*ifreestream=NULL,iex,
      *isolidsurf=NULL,*neighsolidsurf=NULL,*iponoel=NULL,*inoel=NULL,
      nface,nfreestream,nsolidsurf,i,icfd=0,id,*neij=NULL,
      node,networknode,iflagact=0,*nodorig=NULL,*ipivr=NULL,iglob=0,
      *inomat=NULL,*ipnei=NULL,ntrimax,*nx=NULL,*ny=NULL,*nz=NULL,
      *neifa=NULL,*neiel=NULL,*ielfa=NULL,*ifaext=NULL,nflnei,nfaext,
      idampingwithoutcontact=0,*nactdoh=NULL,*nactdohinv=NULL,*ipkonf=NULL,
      *ielmatf=NULL,*ielorienf=NULL,ialeatoric=0,nloadref,isym,
      *nelemloadref=NULL,*iamloadref=NULL,*idefload=NULL,nload_,
      *nelemload=NULL,*iamload=NULL,ncontacts=0,inccontact=0,nrhs=1,
      j=0,*ifatie=NULL,n,inoelsize=0,isensitivity=0,*istartblk=NULL,
      *iendblk=NULL,*nblket=NULL,*nblkze=NULL,nblk,*konf=NULL,*ielblk=NULL,
      *iwork=NULL,nelt,lrgw,*igwk=NULL,itol,itmax,iter,ierr,iunit,ligw,
      mei[4]={0,0,0,0},initial;

  double *stn=NULL,*v=NULL,*een=NULL,cam[5],*epn=NULL,*cg=NULL,
         *cdn=NULL,*vfa=NULL,*pslavsurfold=NULL,
         *f=NULL,*fn=NULL,qa[4]={0.,0.,-1.,0.},qam[2]={0.,0.},dtheta,theta,
	 err,ram[6]={0.,0.,0.,0.,0.,0.},*areaslav=NULL,
         *springarea=NULL,ram1[6]={0.,0.,0.,0.,0.,0.},
	 ram2[6]={0.,0.,0.,0.,0.,0.},deltmx,ptime,smaxls,sminls,
         uam[2]={0.,0.},*vini=NULL,*ac=NULL,qa0,qau,ea,*straight=NULL,
	 *t1act=NULL,qamold[2],*xbounact=NULL,*bc=NULL,
	 *xforcact=NULL,*xloadact=NULL,*fext=NULL,*clearini=NULL,
         reltime,time,bet=0.,gam=0.,*aux2=NULL,dtime,*fini=NULL,
         *fextini=NULL,*veini=NULL,*accini=NULL,*xstateini=NULL,
	 *ampli=NULL,scal1,*eei=NULL,*t1ini=NULL,pressureratio,
         *xbounini=NULL,dev,*xstiff=NULL,*stx=NULL,*stiini=NULL,
         *enern=NULL,*coefmpc=NULL,*aux=NULL,*xstaten=NULL,
	 *coefmpcref=NULL,*enerini=NULL,*emn=NULL,alpham,betam,
	 *tarea=NULL,*tenv=NULL,*erad=NULL,*fnr=NULL,*fni=NULL,
	 *adview=NULL,*auview=NULL,*qfx=NULL,*cvini=NULL,*cv=NULL,
         *qfn=NULL,*co=NULL,*vold=NULL,*fenv=NULL,sigma=0.,
         *xbodyact=NULL,*cgr=NULL,dthetaref, *vr=NULL,*vi=NULL,
	 *stnr=NULL,*stni=NULL,*vmax=NULL,*stnmax=NULL,*fmpc=NULL,*ener=NULL,
	 *f_cm=NULL, *f_cs=NULL,*adc=NULL,*auc=NULL,*res=NULL,
	 *xstate=NULL,*eenmax=NULL,*adrad=NULL,*aurad=NULL,*bcr=NULL,
	 *xmastnor=NULL,*emeini=NULL,*tinc,*tper,*tmin,*tmax,*tincf,
	 *doubleglob=NULL,*xnoels=NULL,*au=NULL,*resold=NULL,
	 *ad=NULL,*b=NULL,*aub=NULL,*adb=NULL,*pslavsurf=NULL,*pmastsurf=NULL,
	 *x=NULL,*y=NULL,*z=NULL,*xo=NULL,sum1,sum2,flinesearch,
	 *yo=NULL,*zo=NULL,*cdnr=NULL,*cdni=NULL,*fnext=NULL,*fnextini=NULL,
	 allwk=0.,allwkini,energy[4]={0.,0.,0.,0.},energyini[4],
	 energyref,denergymax,dtcont,dtvol,wavespeed[*nmat],emax,r_abs,
         enetoll,dampwk=0.,dampwkini=0.,temax,*tmp=NULL,energystartstep[4],
	 sizemaxinc,*adblump=NULL,*adcpy=NULL,*aucpy=NULL,*rwork=NULL,
	 *sol=NULL,*rgwk=NULL,tol,*sb=NULL,*sx=NULL,delcon,alea;

  FILE *f1;
	 
  // MPADD: initialize rmin to the tolerance
  enetoll=0.02;
  r_abs=0.0;
  emax=0.0;
  // MPADD end

  delcon=ctrl[52];alea=ctrl[53];

#ifdef SGI
  ITG token;
#endif
  
  icol=*icolp;irow=*irowp;co=*cop;vold=*voldp;
  ipkon=*ipkonp;lakon=*lakonp;kon=*konp;ielorien=*ielorienp;
  ielmat=*ielmatp;ener=*enerp;xstate=*xstatep;
  
  ipompc=*ipompcp;labmpc=*labmpcp;ikmpc=*ikmpcp;ilmpc=*ilmpcp;
  fmpc=*fmpcp;nodempc=*nodempcp;coefmpc=*coefmpcp;nelemload=*nelemloadp;
  iamload=*iamloadp;sideload=*sideloadp;

  islavsurf=*islavsurfp;pslavsurf=*pslavsurfp;clearini=*clearinip;

  tinc=&timepar[0];
  tper=&timepar[1];
  tmin=&timepar[2];
  tmax=&timepar[3];
  tincf=&timepar[4];

  	/** @gkdas2
	 * Create a new vector to store updated coordinates and copy initial coordinates
	 */
	double *coUpdated = NULL;
	NNEW(coUpdated,double,3**nk);
	memcpy(coUpdated, co, 3**nk*sizeof(double));

  /* Adapter: Put all the CalculiX data that is needed for the coupling into an array */
  struct SimulationData simulationData = {
      .ialset = ialset,
      .ielmat = ielmat,
      .istartset = istartset,
      .iendset = iendset,
      .kon = kon,
      .ipkon = ipkon,
      .lakon = &lakon,
      .co = co,
      .set = set,
      .nset = *nset,
      .ikboun = ikboun,
      .ikforc = ikforc,
      .ilboun = ilboun,
      .ilforc = ilforc,
      .nboun = *nboun,
      .nforc = *nforc,
      .nelemload = nelemload,
      .nload = *nload,
      .sideload = sideload,
      .mt = mt,
      .nk = *nk,
      .theta = &theta,
      .dtheta = &dtheta,
      .tper = tper,
      .nmethod = nmethod,
      .xload = xload,
      .xforc = xforc,
      .xboun = xboun,
      .ntmat_ = ntmat_,
      .vold = vold,
      .veold = veold,
      .fn = fn,
      .cocon = cocon,
      .ncocon = ncocon,
      .mi = mi
  };

  if(*ithermal==4){
      uncoupled=1;
      *ithermal=3;
  }else{
      uncoupled=0;
  }
  
  if(*mortar!=1){
      maxprevcontel=*nslavs;
  }else if(*mortar==1){
      maxprevcontel=*nintpoint;
      if(*nstate_!=0){
	  if(maxprevcontel!=0){
	      NNEW(islavsurfold,ITG,2**ifacecount+2);
	      NNEW(pslavsurfold,double,3**nintpoint);
	      memcpy(&islavsurfold[0],&islavsurf[0],
		     sizeof(ITG)*(2**ifacecount+2));
	      memcpy(&pslavsurfold[0],&pslavsurf[0],
		     sizeof(double)*(3**nintpoint));
	  }
      }
  }
  nslavs_prev_step=*nslavs;

  /* turbulence model 
     iturbulent==0: laminar
     iturbulent==1: k-epsilon
     iturbulent==2: q-omega
     iturbulent==3: BSL
     iturbulent==4: SST */
  
  iturbulent=(ITG)physcon[8];
  
  for(k=0;k<3;k++){
      strcpy1(&jobnamef[k*132],&jobnamec[k*132],132);
  }
  
  qa0=ctrl[20];qau=ctrl[21];ea=ctrl[23];deltmx=ctrl[26];
  i0ref=ctrl[0];irref=ctrl[1];icref=ctrl[3];

  sminls=ctrl[28];smaxls=ctrl[29];
  
  memmpc_=mpcinfo[0];mpcfree=mpcinfo[1];icascade=mpcinfo[2];
  maxlenmpc=mpcinfo[3];

  alpham=xmodal[0];
  betam=xmodal[1];

  /* check whether, for a dynamic calculation, damping is involved */

  if(*nmethod==4){
      if(*iexpl<=1){
	  if((fabs(alpham)>1.e-30)||(fabs(betam)>1.e-30)){
	      idamping=1;
	      idampingwithoutcontact=1;
	  }else{
	      for(i=0;i<*ne;i++){
		  if(ipkon[i]<0) continue;
		  if(strcmp1(&lakon[i*8],"ED")==0){
		      idamping=1;idampingwithoutcontact=1;break;
		  }
	      }
	  }
      }
  }

  /* check whether a sensitivity step may follow (whether design variables
     were defined */

  for(i=0;i<*ntie;i++){
      if(strcmp1(&tieset[i*243+80],"D")==0){
	  isensitivity=1;
	  NNEW(adcpy,double,neq[1]);
          /* no asymmetric matrices allowed for sensitivity */
	  NNEW(aucpy,double,nzs[1]);
	  break;
      }
  }

  if((icascade==2)&&(*iexpl>=2)){
      printf(" *ERROR in nonlingeo: linear and nonlinear MPC's depend on each other\n");
      printf("        This is not allowed in a explicit dynamic calculation\n");
      FORTRAN(stop,());
  }
  
  /* check whether the submodel is meant for a fluid-structure
     interaction */
  
/*  strcpy(fnffrd,jobnamec);
  strcat(fnffrd,"f.frd");
  if((jobnamec[396]!='\0')&&(strcmp1(&jobnamec[396],fnffrd)==0)){*/
      
      /* fluid-structure interaction: wait till after the compfluid
         call */
      
/*      NNEW(integerglob,ITG,1);
	NNEW(doubleglob,double,1);*/
//  }else{
      
      /* determining the global values to be used as boundary conditions
	 for a submodel */
      
      getglobalresults(jobnamec,&integerglob,&doubleglob,nboun,iamboun,xboun,
		       nload,sideload,iamload,&iglob,nforc,iamforc,xforc,
                       ithermal,nk,t1,iamt1,&sigma);

      if(iglob<0){
	  printf(" *ERROR in nonlingeo: a submodel calculation for which\n");
	  printf("        the global model results from a *FREQUENCY\n");
	  printf("        calculation must be geometrically linear\n");
	  FORTRAN(stop,());
      }
//  }
  
  /* invert nactdof */
  
  NNEW(nactdofinv,ITG,mt**nk);
  NNEW(nodorig,ITG,*nk);
  FORTRAN(gennactdofinv,(nactdof,nactdofinv,nk,mi,nodorig,
			 ipkon,lakon,kon,ne));
  SFREE(nodorig);
  
  /* allocating a field for the stiffness matrix */
  
  NNEW(xstiff,double,(long long)27*mi[0]**ne);
  
  /* allocating force fields */
  
  NNEW(f,double,neq[1]);
  NNEW(fext,double,neq[1]);
  
  NNEW(b,double,neq[1]);
  NNEW(vini,double,mt**nk);
  
  NNEW(aux,double,7*maxlenmpc);
  NNEW(iaux,ITG,2*maxlenmpc);
  
  /* allocating fields for the actual external loading */
  
  NNEW(xbounact,double,*nboun);
  NNEW(xbounini,double,*nboun);
  for(k=0;k<*nboun;++k){xbounact[k]=xbounold[k];}
  NNEW(xforcact,double,*nforc);
  NNEW(xloadact,double,2**nload);
  NNEW(xbodyact,double,7**nbody);
  /* copying the rotation axis and/or acceleration vector */
  for(k=0;k<7**nbody;k++){xbodyact[k]=xbody[k];}
  
  /* assigning the body forces to the elements */ 
  
  if(*nbody>0){
      ifreebody=*ne+1;
      NNEW(ipobody,ITG,2*ifreebody**nbody);
      for(k=1;k<=*nbody;k++){
	  FORTRAN(bodyforce,(cbody,ibody,ipobody,nbody,set,istartset,
			     iendset,ialset,&inewton,nset,&ifreebody,&k));
	  RENEW(ipobody,ITG,2*(*ne+ifreebody));
      }
      RENEW(ipobody,ITG,2*(ifreebody-1));
      if(inewton==1){NNEW(cgr,double,4**ne);}
  }
  
  /* for mechanical calculations: updating boundary conditions
     calculated in a previous thermal step */
  
  if(*ithermal<2) FORTRAN(gasmechbc,(vold,nload,sideload,
				     nelemload,xload,mi));
  
  /* for thermal calculations: forced convection and cavity
     radiation*/
  
  if(*ithermal>1){
      NNEW(itg,ITG,*nload+3**nflow);
      NNEW(ieg,ITG,*nflow);
      /* max 6 triangles per face, 4 entries per triangle */
      NNEW(kontri,ITG,24**nload);
      NNEW(nloadtr,ITG,*nload);
      NNEW(nacteq,ITG,4**nk);
      NNEW(nactdog,ITG,4**nk);
      NNEW(v,double,mt**nk);
      FORTRAN(envtemp,(itg,ieg,&ntg,&ntr,sideload,nelemload,
		       ipkon,kon,lakon,ielmat,ne,nload,
                       kontri,&ntri,nloadtr,nflow,ndirboun,nactdog,
                       nodeboun,nacteq,nboun,ielprop,prop,&nteq,
                       v,network,physcon,shcon,ntmat_,co,
                       vold,set,nshcon,rhcon,nrhcon,mi,nmpc,nodempc,
                       ipompc,labmpc,ikboun,&nasym,ttime,&time,iaxial));
      SFREE(v);
      
      if((*mcs>0)&&(ntr>0)){
	  NNEW(inocs,ITG,*nk);
	  radcyc(nk,kon,ipkon,lakon,ne,cs,mcs,nkon,ialset,istartset,
		 iendset,&kontri,&ntri,&co,&vold,&ntrit,inocs,mi);
      }
      else{ntrit=ntri;}
      
      nzsrad=100*ntr;
      NNEW(mast1rad,ITG,nzsrad);
      NNEW(irowrad,ITG,nzsrad);
      NNEW(icolrad,ITG,ntr);
      NNEW(jqrad,ITG,ntr+1);
      NNEW(ipointerrad,ITG,ntr);
      
      if(ntr>0){
	  mastructrad(&ntr,nloadtr,sideload,ipointerrad,
		      &mast1rad,&irowrad,&nzsrad,
		      jqrad,icolrad);
      }
      
      /* determine the network elements belonging to a given node (for usage
         in user subroutine film */

      if((*network>0)||(ntg>0)){
	  NNEW(iponoel,ITG,*nk);
	  NNEW(inoel,ITG,2**nkon);
	  if(*network>0){
	      FORTRAN(networkelementpernode,(iponoel,inoel,lakon,ipkon,kon,
		  &inoelsize,nflow,ieg,ne,network));
	  }
	  RENEW(inoel,ITG,2*inoelsize);
      }

      SFREE(ipointerrad);SFREE(mast1rad);
      RENEW(irowrad,ITG,nzsrad);
      
      RENEW(itg,ITG,ntg);
      NNEW(ineighe,ITG,ntg);
      RENEW(kontri,ITG,4*ntrit);
      RENEW(nloadtr,ITG,ntr);
      
      NNEW(adview,double,ntr);
      NNEW(auview,double,2*nzsrad);
      NNEW(tarea,double,ntr);
      NNEW(tenv,double,ntr);
      NNEW(fenv,double,ntr);
      NNEW(erad,double,ntr);
      
      NNEW(ac,double,nteq*nteq);
      NNEW(bc,double,nteq);
      NNEW(ipiv,ITG,nteq);
      NNEW(adrad,double,ntr);
      NNEW(aurad,double,2*nzsrad);
      NNEW(bcr,double,ntr);
      NNEW(ipivr,ITG,ntr);
  }
  
  /* check for fluid elements
     check for strain-less elements */
  
  NNEW(nactdoh,ITG,*ne);
  NNEW(nactdohinv,ITG,*ne);
  *nef=0;
  for(i=0;i<*ne;++i){
      if(ipkon[i]<0) continue;
      if(strcmp1(&lakon[8*i],"F")==0){
	  icfd=1;nactdohinv[*nef]=i+1;++*nef;nactdoh[i]=*nef;}
      if(istrainfree==0){
	  if(ielmat[i]<0){istrainfree=1;}
      }
  }
  if(icfd==1){

      /* checking block structures (CFD calculations) */

      NNEW(ielfa,ITG,24**nef);
      NNEW(nodface,ITG,5*6**nef);
      NNEW(neiel,ITG,6**nef);
      NNEW(neij,ITG,6**nef);
      NNEW(neifa,ITG,6**nef);
      NNEW(ipoface,ITG,*nk);
      NNEW(ipnei,ITG,*ne+1);
      NNEW(konf,ITG,*nkon);
      memcpy(&konf[0],&kon[0],sizeof(ITG)**nkon);
      DMEMSET(ipoface,0,*nk,0);
      DMEMSET(neiel,0,6**nef,0);
      DMEMSET(ielfa,0,24**nef,0);

      /* gathering topological information (CFD calculations) */

      RENEW(nactdohinv,ITG,*nef);
      NNEW(ipkonf,ITG,*nef);
      NNEW(lakonf,char,8**nef);
      NNEW(ielmatf,ITG,mi[2]**nef);
      if(*norien>0) NNEW(ielorienf,ITG,mi[2]**nef);
      NNEW(ifatie,ITG,6**nef);
      NNEW(ifaext,ITG,6**nef);
      NNEW(isolidsurf,ITG,6**nef);
//      NNEW(vel,double,8**nef);
      NNEW(vfa,double,8*6**nef);

      n=0;for(i=0;i<*mcs;i++){if(floor(cs[17*i+3])>n)n=floor(cs[17*i+3]);}
      NNEW(xo,double,n);NNEW(yo,double,n);NNEW(zo,double,n);	    
      NNEW(x,double,n);NNEW(y,double,n);NNEW(z,double,n);	   
      NNEW(nx,ITG,n);NNEW(ny,ITG,n);NNEW(nz,ITG,n);

      FORTRAN(precfd,(ne,ipkon,konf,lakon,ipnei,neifa,neiel,ipoface,
       nodface,ielfa,&nflnei,&nface,ifaext,&nfaext,
       isolidsurf,&nsolidsurf,set,nset,istartset,iendset,ialset,
       vel,vold,mi,neij,nef,nactdoh,ipkonf,lakonf,ielmatf,ielmat,
       ielorienf,ielorien,norien,cs,mcs,tieset,x,y,z,xo,yo,zo,
       nx,ny,nz,co,ifatie,velo,veloo,&initial));

      SFREE(xo);SFREE(yo);SFREE(zo);SFREE(x);SFREE(y);SFREE(z);
      SFREE(nx);SFREE(ny);SFREE(nz);

      SFREE(ipoface);
      SFREE(nodface);
      RENEW(neifa,ITG,nflnei);
      RENEW(neiel,ITG,nflnei);
      RENEW(neij,ITG,nflnei);
      RENEW(ielfa,ITG,4*nface);
      RENEW(ifatie,ITG,nface);
      RENEW(ifaext,ITG,nfaext);
      RENEW(isolidsurf,ITG,nsolidsurf);
      RENEW(vfa,double,8*nface);
      RENEW(ipnei,ITG,*nef+1);
  }else{
      SFREE(nactdoh);SFREE(nactdohinv);
  }
  if(*ithermal>1){NNEW(qfx,double,3*mi[0]**ne);}
  
  /* contact conditions */
  
  inicont(nk,&ncont,ntie,tieset,nset,set,istartset,iendset,ialset,&itietri,
	  lakon,ipkon,kon,&koncont,nslavs,tietol,&ismallsliding,&itiefac,
          &islavsurf,&islavnode,&imastnode,&nslavnode,&nmastnode,
          mortar,&imastop,nkon,&iponoels,&inoels,&ipe,&ime,ne,ifacecount,
	  iperturb,ikboun,nboun,co,istep,&xnoels);
  
  if(ncont!=0){
      
      NNEW(cg,double,3*ncont);
      NNEW(straight,double,16*ncont);
	  
      /* 11 instead of 10: last position is reserved for the
	 local contact spring element number; needed as
	 pointer into springarea */
      
      if(*mortar==0){
	  RENEW(kon,ITG,*nkon+11**nslavs);
	  NNEW(springarea,double,2**nslavs);
	  if(*nener==1){
	      RENEW(ener,double,mi[0]*(*ne+*nslavs)*2);

	      /* setting the entries for the friction contact energy to zero */

	      for(k=mi[0]*(2**ne+*nslavs);k<mi[0]*(*ne+*nslavs)*2;k++){ener[k]=0.;}
	  }
	  RENEW(ipkon,ITG,*ne+*nslavs);
	  RENEW(lakon,char,8*(*ne+*nslavs));
	  
	  if(*norien>0){
	      RENEW(ielorien,ITG,mi[2]*(*ne+*nslavs));
	      for(k=mi[2]**ne;k<mi[2]*(*ne+*nslavs);k++) ielorien[k]=0;
	  }

	  RENEW(ielmat,ITG,mi[2]*(*ne+*nslavs));
	  for(k=mi[2]**ne;k<mi[2]*(*ne+*nslavs);k++) ielmat[k]=1;

	  if((maxprevcontel==0)&&(*nslavs!=0)){
	      RENEW(xstate,double,*nstate_*mi[0]*(*ne+*nslavs));
	      for(k=*nstate_*mi[0]**ne;k<*nstate_*mi[0]*(*ne+*nslavs);k++){
		  xstate[k]=0.;
	      }
	  }
	  maxprevcontel=*nslavs;

	  NNEW(areaslav,double,*ifacecount);
      }else if(*mortar==1){
	  NNEW(islavact,ITG,nslavnode[*ntie]);
	  if((*istep==1)||(nslavs_prev_step==0)) NNEW(clearini,double,3*9**ifacecount);

          /* check whether at least one contact definition involves true contact
             and not just tied contact */

	  FORTRAN(checktruecontact,(ntie,tieset,tietol,elcon,&itruecontact,
				    ncmat_,ntmat_));
      }

      NNEW(xmastnor,double,3*nmastnode[*ntie]);
  }
  
  if(icascade==2){
      memmpcref_=memmpc_;mpcfreeref=mpcfree;maxlenmpcref=maxlenmpc;
      NNEW(nodempcref,ITG,3*memmpc_);
      for(k=0;k<3*memmpc_;k++){nodempcref[k]=nodempc[k];}
      NNEW(coefmpcref,double,memmpc_);
      for(k=0;k<memmpc_;k++){coefmpcref[k]=coefmpc[k];}
  }
  
  if((*ithermal==1)||(*ithermal>=3)){
      NNEW(t1ini,double,*nk);
      NNEW(t1act,double,*nk);
      for(k=0;k<*nk;++k){t1act[k]=t1old[k];}
  }
  
  /* allocating a field for the instantaneous amplitude */
  
  NNEW(ampli,double,*nam);
  
  /* fini is also needed in static calculations if ilin=1
     to get correct values of f after a divergent increment */

  NNEW(fini,double,neq[1]);
  
  /* allocating fields for nonlinear dynamics */
  
  if(*nmethod==4){
      mass[0]=1;
      mass[1]=1;
      NNEW(aux2,double,neq[1]);
      NNEW(fextini,double,neq[1]);
      NNEW(fnext,double,mt**nk);
      NNEW(fnextini,double,mt**nk);
      NNEW(veini,double,mt**nk);
      NNEW(accini,double,mt**nk);
      NNEW(adb,double,neq[1]);
      NNEW(aub,double,nzs[1]);
      NNEW(cvini,double,neq[1]);
      NNEW(cv,double,neq[1]);
  }

  if((*nstate_!=0)&&((*mortar!=1)||(ncont==0))){
      NNEW(xstateini,double,*nstate_*mi[0]*(*ne+*nslavs));
      for(k=0;k<*nstate_*mi[0]*(*ne+*nslavs);++k){
	  xstateini[k]=xstate[k];
      }
  }
  if((*nstate_!=0)&&(*mortar==1)) NNEW(xstateini,double,1);
  NNEW(eei,double,6*mi[0]**ne);
  NNEW(stiini,double,6*mi[0]**ne);
  NNEW(emeini,double,6*mi[0]**ne);
  if(*nener==1)
      NNEW(enerini,double,mi[0]**ne);
  
  qa[0]=qaold[0];
  qa[1]=qaold[1];
  
  /* normalizing the time */
  
  FORTRAN(checktime,(itpamp,namta,tinc,ttime,amta,tmin,inext,&itp,istep,tper));
  dtheta=(*tinc)/(*tper);

  /* taking care of a small increment at the end of the step
     for face-to-face penalty contact */

  dthetaref=dtheta;
  if((dtheta<=1.e-6)&&(*iexpl<=1)){
      printf("\n *ERROR in nonlingeo\n");
      printf(" increment size smaller than one millionth of step size\n");
      printf(" increase increment size\n\n");
  }
  *tmin=*tmin/(*tper);
  *tmax=*tmax/(*tper);
  theta=0.;
  
  /* calculating an initial flux norm */
  
  if(*ithermal!=2){
      if(qau>1.e-10){qam[0]=qau;}
      else if(qa0>1.e-10){qam[0]=qa0;}
      else if(qa[0]>1.e-10){qam[0]=qa[0];}
      else {qam[0]=1.e-2;}
  }
  if(*ithermal>1){
      if(qau>1.e-10){qam[1]=qau;}
      else if(qa0>1.e-10){qam[1]=qa0;}
      else if(qa[1]>1.e-10){qam[1]=qa[1];}
      else {qam[1]=1.e-2;}
  }
  
  /* storing the element and topology information before introducing 
     contact elements */
  
  ne0=*ne;nkon0=*nkon;neold=*ne;
  
  /*********************************************************************/
  
  /* calculating of the acceleration due to force discontinuities
     (external - internal force) at the start of a step */
  
  /*********************************************************************/
  
  if((*nmethod==4)&&(*ithermal!=2)&&(icfd!=1)){
      bet=(1.-*alpha)*(1.-*alpha)/4.;
      gam=0.5-*alpha;

      /* initialization of the energy */

      if(ithermal[0]<=1){
	  for(k=0;k<mi[0]*ne0;++k){enerini[k]=ener[k];}
      }
      energyini[3]=energy[3];
      
      /* calculating the stiffness and mass matrix */
      
      reltime=0.;
      time=0.;
      dtime=0.;
      
      FORTRAN(tempload,(xforcold,xforc,xforcact,iamforc,nforc,xloadold,xload,
	      xloadact,iamload,nload,ibody,xbody,nbody,xbodyold,xbodyact,
	      t1old,t1,t1act,iamt1,nk,amta,
	      namta,nam,ampli,&time,&reltime,ttime,&dtime,ithermal,nmethod,
              xbounold,xboun,xbounact,iamboun,nboun,
	      nodeboun,ndirboun,nodeforc,ndirforc,istep,&iinc,
	      co,vold,itg,&ntg,amname,ikboun,ilboun,nelemload,sideload,mi,
              ntrans,trab,inotr,veold,integerglob,doubleglob,tieset,istartset,
              iendset,ialset,ntie,nmpc,ipompc,ikmpc,ilmpc,nodempc,coefmpc,
              ipobody,iponoel,inoel,ipkon,kon,ielprop,prop,ielmat,
              shcon,nshcon,rhcon,nrhcon,cocon,ncocon,ntmat_,lakon));
      
      time=0.;
      dtime=1.;
    
      /*  updating the nonlinear mpc's (also affects the boundary
	  conditions through the nonhomogeneous part of the mpc's)
	  if contact arises the number of MPC's can also change */
      
      cam[0]=0.;cam[1]=0.;cam[2]=0.;
      
      if(icascade==2){
	  memmpc_=memmpcref_;mpcfree=mpcfreeref;maxlenmpc=maxlenmpcref;
	  RENEW(nodempc,ITG,3*memmpcref_);
	  for(k=0;k<3*memmpcref_;k++){nodempc[k]=nodempcref[k];}
	  RENEW(coefmpc,double,memmpcref_);
	  for(k=0;k<memmpcref_;k++){coefmpc[k]=coefmpcref[k];}
      }

      newstep=0;
      FORTRAN(nonlinmpc,(co,vold,ipompc,nodempc,coefmpc,labmpc,
			 nmpc,ikboun,ilboun,nboun,xbounold,aux,iaux,
			 &maxlenmpc,ikmpc,ilmpc,&icascade,
			 kon,ipkon,lakon,ne,&reltime,&newstep,xboun,fmpc,
			 &iit,&idiscon,&ncont,trab,ntrans,ithermal,mi));
      if(icascade==2){
	  for(k=0;k<3*memmpc_;k++){nodempcref[k]=nodempc[k];}
	  for(k=0;k<memmpc_;k++){coefmpcref[k]=coefmpc[k];}
      }
      
      if(icascade>0) remastruct(ipompc,&coefmpc,&nodempc,nmpc,
	      &mpcfree,nodeboun,ndirboun,nboun,ikmpc,ilmpc,ikboun,ilboun,
	      labmpc,nk,&memmpc_,&icascade,&maxlenmpc,
	      kon,ipkon,lakon,ne,nactdof,icol,jq,&irow,isolver,
	      neq,nzs,nmethod,&f,&fext,&b,&aux2,&fini,&fextini,
	      &adb,&aub,ithermal,iperturb,mass,mi,iexpl,mortar,
	      typeboun,&cv,&cvini,&iit,network);
      
      /* invert nactdof */

      SFREE(nactdofinv);
      NNEW(nactdofinv,ITG,1);
      
      iout=-1;
      ielas=1;
      
      NNEW(fn,double,mt**nk);
      NNEW(stx,double,6*mi[0]**ne);
      
      NNEW(inum,ITG,*nk);
      results(co,nk,kon,ipkon,lakon,ne,vold,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1old,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounold,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,&bet,
	      &gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,&icmd,
	      ncmat_,nstate_,sti,vini,ikboun,ilboun,ener,enern,emeini,xstaten,
	      eei,enerini,cocon,ncocon,set,nset,istartset,iendset,
	      ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,fmpc,
	      nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,&reltime,
	      &ne0,thicke,shcon,nshcon,
	      sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
	      mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
	      islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
              inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
      
      SFREE(fn);SFREE(stx);SFREE(inum);
      
      iout=0;
      ielas=0;
      
      reltime=0.;
      time=0.;
      dtime=0.;
      
      if(*iexpl<=1){intscheme=1;}
      
      /* in mafillsm the stiffness and mass matrix are computed;
	 The primary aim is to calculate the mass matrix (not 
	 lumped for an implicit dynamic calculation, lumped for an
	 explicit dynamic calculation). However:
	 - for an implicit calculation the mass matrix is "doped" with
	 a small amount of stiffness matrix, therefore the calculation
	 of the stiffness matrix is needed.
	 - for an explicit calculation the stiffness matrix is not 
	 needed at all. Since the calculation of the mass matrix alone
	 is not possible in mafillsm, the determination of the stiffness
	 matrix is taken as unavoidable "ballast". */
      
      NNEW(ad,double,neq[1]);
      NNEW(au,double,nzs[1]);

      mafillsmmain(co,nk,kon,ipkon,lakon,ne,nodeboun,ndirboun,xbounact,nboun,
		  ipompc,nodempc,coefmpc,nmpc,nodeforc,ndirforc,xforcact,
		  nforc,nelemload,sideload,xloadact,nload,xbodyact,ipobody,
		  nbody,cgr,ad,au,fext,nactdof,icol,jq,irow,neq,nzl,
		  nmethod,ikmpc,ilmpc,ikboun,ilboun,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,
		  ielmat,ielorien,norien,orab,ntmat_,
		  t0,t1act,ithermal,prestr,iprestr,vold,iperturb,sti,
		  nzs,stx,adb,aub,iexpl,plicon,nplicon,plkcon,nplkcon,
		  xstiff,npmat_,&dtime,matname,mi,
                  ncmat_,mass,&stiffness,&buckling,&rhsi,&intscheme,
                  physcon,shcon,nshcon,cocon,ncocon,ttime,&time,istep,&iinc,
		  &coriolis,ibody,xloadold,&reltime,veold,springarea,nstate_,
                  xstateini,xstate,thicke,integerglob,doubleglob,
		  tieset,istartset,iendset,ialset,ntie,&nasym,pslavsurf,
		  pmastsurf,mortar,clearini,ielprop,prop,&ne0,fnext,&kscale,
		  iponoel,inoel,network,ntrans,inotr,trab,design,penal);
      
      if(*nmethod==0){
	  
	  /* error occurred in mafill: storing the geometry in frd format */
	  
	  ++*kode;
	  if(strcmp1(&filab[1044],"ZZS")==0){
	      NNEW(neigh,ITG,40**ne);
	      NNEW(ipneigh,ITG,*nk);
	  }
	  
	  ptime=*ttime+time;
	  frd(co,nk,kon,ipkon,lakon,&ne0,v,stn,inum,nmethod,
	      kode,filab,een,t1,fn,&ptime,epn,ielmat,matname,enern,xstaten,
	      nstate_,istep,&iinc,ithermal,qfn,&mode,&noddiam,trab,inotr,
	      ntrans,orab,ielorien,norien,description,ipneigh,neigh,
	      mi,sti,vr,vi,stnr,stni,vmax,stnmax,&ngraph,veold,ener,ne,
	      cs,set,nset,istartset,iendset,ialset,eenmax,fnr,fni,emn,
	      thicke,jobnamec,output,qfx,cdn,mortar,cdnr,cdni,nmat,
	      ielprop,prop);
	  
	  if(strcmp1(&filab[1044],"ZZS")==0){SFREE(ipneigh);SFREE(neigh);}      
#ifdef COMPANY
	  FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif	  
	  FORTRAN(stop,());
	  
      }
      
      /* mass x acceleration = f(external)-f(internal) 
	 only for the mechanical loading*/
      
      for(k=0;k<neq[0];++k){
	  b[k]=fext[k]-f[k];
      }
      
      if(*iexpl<=1){
	  
	  /* a small amount of stiffness is added to the mass matrix
	     otherwise the system leads to huge accelerations in 
	     case of discontinuous load changes at the start of the step */
	  
	  dtime=*tinc/10.;
	  scal1=bet*dtime*dtime*(1.+*alpha);
	  for(k=0;k<neq[0];++k){
	      ad[k]=adb[k]+scal1*ad[k];
	  }
	  for(k=0;k<nzs[0];++k){
	      au[k]=aub[k]+scal1*au[k];
	  }
	  if(*isolver==0){
#ifdef SPOOLES
	      spooles(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],
		      &symmetryflag,&inputformat,&nzs[2]);
#else
	      printf(" *ERROR in nonlingeo: the SPOOLES library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if((*isolver==2)||(*isolver==3)){
	      preiter(ad,&au,b,&icol,&irow,&neq[0],&nzs[0],isolver,iperturb);
	  }
	  else if(*isolver==4){
#ifdef SGI
	      token=1;
	      sgi_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],token);
#else
	      printf(" *ERROR in nonlingeo: the SGI library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if(*isolver==5){
#ifdef TAUCS
	      tau(ad,&au,adb,aub,&sigma,b,icol,&irow,&neq[0],&nzs[0]);
#else
	      printf(" *ERROR in nonlingeo: the TAUCS library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if(*isolver==7){
#ifdef PARDISO
	      pardiso_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],
			   &symmetryflag,&inputformat,jq,&nzs[2],&nrhs);
#else
	      printf(" *ERROR in nonlingeo: the PARDISO library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
      }
      
      else{
	  for(k=0;k<neq[0];++k){
	      b[k]=(fext[k]-f[k])/adb[k];
	  }
      }
      
      /* for thermal loading the acceleration is set to zero */
      
      for(k=neq[0];k<neq[1];++k){
	  b[k]=0.;
      }
      
      /* calculating the displacements, stresses and forces */
      
      NNEW(v,double,mt**nk);
      memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
      
      NNEW(stx,double,6*mi[0]**ne);
      NNEW(fn,double,mt**nk);
      
      /* setting a "special" time consisting of the first primes;
         used to recognize the initial acceleration procedure
         in file resultsini.f */

      NNEW(inum,ITG,*nk);
      dtime=1.235711130e-20;
      results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounact,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
	      &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
	      &icmd,ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
	      emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
	      iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
	      fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
	      &reltime,&ne0,thicke,shcon,nshcon,
	      sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
	      mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
              islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
              inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
      SFREE(inum);
      dtime=0.;

      memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);
      if(*ithermal!=2){
	  for(k=0;k<6*mi[0]*ne0;++k){
	      sti[k]=stx[k];
	  }
      }

      SFREE(v);SFREE(stx);SFREE(fn);
      SFREE(ad);SFREE(au);
      
      /* the mass matrix is kept for subsequent calculations, therefore,
	 no new mass calculation is necessary for the remaining iterations
	 in the present step */
      
      mass[0]=0;intscheme=0;
      energyref=energy[0]+energy[1]+energy[2]+energy[3];

      /* carlo start */

      if(*iexpl>1){
	  
	  /* CMT: Calculation of stable time increment according to
	     Courant's Law  CARLO MT*/
	  
	  FORTRAN(calcmatwavspeed,(ne,elcon,nelcon,
		  rhcon,nrhcon,alcon,nalcon,orab,ntmat_,ithermal,alzero,
		  plicon,nplicon,plkcon,nplkcon,npmat_,mi,&dtime,
		  xstiff,ncmat_,vold,ielmat,t0,t1,
		  matname,lakon,wavespeed,nmat,ipkon));
	  
	  FORTRAN(calcstabletimeincvol,(&ne0,lakon,co,kon,ipkon,mi,
			      ielmat,&dtvol,alpha,wavespeed));
      
	  printf(" ++CMT DEBUG: courant criterion for stability time inc=%e\n",dtvol);
	  *tinc=dtvol;
	  dtheta=(*tinc)/(*tper);
	  dthetaref=dtheta;
	  
      } else {
              // # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
              // MPADD start
	  /* lumping of the mass matrix for implict calculations to 
	     midify the increment time when contact is involved
	  */
	  
	  NNEW(tmp,double,neq[1]);
	  NNEW(adblump,double,neq[1]);
	  for(k=0;k<neq[1];k++){
	    tmp[k] = 1;
	  }
	  if(nasym==0){
	    FORTRAN(op,(&neq[1],tmp,adblump,adb,aub,jq,irow)); 
	  }else{
	    FORTRAN(opas,(&neq[1],tmp,adblump,adb,aub,jq,irow,nzs)); 
	  }
	  SFREE(tmp);
	  
	  // MPADD end
	  // # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
      }

      /* carlo end */
      
  }
  
  if(*iexpl>1) icmd=3;
  
  /**************************************************************/
  /* starting the loop over the increments                      */
  /**************************************************************/
  
  newstep=1;
	  
//    MPADD start
  if((*nmethod==4)&&(*ithermal<2)&&(*iexpl<=1)){
      neini=*ne;
      for(k=0;k<4;k++){
	  energystartstep[k]=energy[k];
      }
      emax=0.1*energyref; // Anti-stuck at the beginning of simulation
  } 
//    MPADD end

  /* saving the distributed loads (volume heating will be
     added because of friction heating) */

  if((*ithermal==3)&&(ncont!=0)&&(*mortar==1)&&(*ncmat_>=11)){
      nloadref=*nload;
      NNEW(nelemloadref,ITG,2**nload);
      if(*nam>0) NNEW(iamloadref,ITG,2**nload);
      NNEW(sideloadref,char,20**nload);
      
      memcpy(&nelemloadref[0],&nelemload[0],sizeof(ITG)*2**nload);
      if(*nam>0) memcpy(&iamloadref[0],&iamload[0],sizeof(ITG)*2**nload);
      memcpy(&sideloadref[0],&sideload[0],sizeof(char)*20**nload);
  }

  /* Adapter: Create the interfaces and initialize the coupling */
  printf("About to enter preCICE setup in Calculix with names %s and %s \n", preciceParticipantName, configFilename);

  Precice_Setup( configFilename, preciceParticipantName, &simulationData );
  
  /* Adapter: Give preCICE the control of the time stepping */
  while( Precice_IsCouplingOngoing() )
  {
      
      /* Adapter: Adjust solver time step */
      Precice_AdjustSolverTimestep( &simulationData );
	  
      /* Adapter read coupling data if available */
      Precice_ReadCouplingData( &simulationData );
      
    if(icutb==0)
		{
	  
	  /* previous increment converged: update the initial values */
	  
	  iinc++;
	  jprint++;

          /* store number of elements (important for implicit dynamic
             contact */

	  neini=*ne;
	  
	  /* vold is copied into vini */
	  
	  memcpy(&vini[0],&vold[0],sizeof(double)*mt**nk);

	  if( Precice_IsWriteCheckpointRequired() )
      	  {
			printf("WRITING ITERATION CHECKPOINT\n");
          	Precice_WriteIterationCheckpoint( &simulationData, vini );
          	Precice_FulfilledWriteCheckpoint();
          }
	  
	  for(k=0;k<*nboun;++k)
	  {
		xbounini[k]=xbounact[k];
	  }
	  if((*ithermal==1)||(*ithermal>=3))
	  {
	      for(k=0;k<*nk;++k){t1ini[k]=t1act[k];}
	  }

	  for(k=0;k<neq[1];++k)
	  {
	      fini[k]=f[k];
	  }

	  if(*nmethod==4)
	  {
	    for(k=0;k<mt**nk;++k)
		{
		  veini[k]=veold[k];
		  accini[k]=accold[k];
		  fnextini[k]=fnext[k];
	    }

	    for(k=0;k<neq[1];++k)
		  {
		  fextini[k]=fext[k];
		  cvini[k]=cv[k];
	      }

	    if(*ithermal<2)
		{
		  allwkini=allwk;
		  // MPADD start
		  if(idamping==1)dampwkini = dampwk;

		  for(k=0;k<4;k++)
		  {
		    energyini[k]=energy[k];
		  }
		  // MPADD end
	    }
	  }
	  if(*ithermal!=2)
	  {
	      for(k=0;k<6*mi[0]*ne0;++k){
		  stiini[k]=sti[k];
		  emeini[k]=eme[k];
	      }
	  }
	  if(*nener==1)
	      for(k=0;k<mi[0]*ne0;++k){enerini[k]=ener[k];}

	  if(*mortar!=1){
	      if(*nstate_!=0){
		  for(k=0;k<*nstate_*mi[0]*(ne0+*nslavs);++k){
		      xstateini[k]=xstate[k];
		  }
	      }
	  }	
      }
      
      /* check for max. # of increments */
      
      if(iinc>*jmax){
	  printf(" *ERROR: max. # of increments reached\n\n");
	  FORTRAN(stop,());
      }
      printf(" increment %" ITGFORMAT " attempt %" ITGFORMAT " \n",iinc,icutb+1);
      printf(" increment size= %e\n",dtheta**tper);
      printf(" sum of previous increments=%e\n",theta**tper);
      printf(" actual step time=%e\n",(theta+dtheta)**tper);
      printf(" actual total time=%e\n\n",*ttime+(theta+dtheta)**tper);
      
      printf(" iteration 1\n\n");
      
      qamold[0]=qam[0];
      qamold[1]=qam[1];

      icntrl=0;

      /* restoring the distributed loading before adding the
         friction heating */

      if((*ithermal==3)&&(ncont!=0)&&(*mortar==1)&&(*ncmat_>=11)){
	  *nload=nloadref;
	  memcpy(&nelemload[0],&nelemloadref[0],sizeof(ITG)*2**nload);
	  if(*nam>0){
	      memcpy(&iamload[0],&iamloadref[0],sizeof(ITG)*2**nload);
	  }
	  memcpy(&sideload[0],&sideloadref[0],sizeof(char)*20**nload);
      }
      
      /* determining the actual loads at the end of the new increment*/
      
      reltime=theta+dtheta;
      time=reltime**tper;
      dtime=dtheta**tper;
      
      FORTRAN(tempload,(xforcold,xforc,xforcact,iamforc,nforc,xloadold,xload,
	      xloadact,iamload,nload,ibody,xbody,nbody,xbodyold,xbodyact,
	      t1old,t1,t1act,iamt1,nk,amta,
	      namta,nam,ampli,&time,&reltime,ttime,&dtime,ithermal,nmethod,
              xbounold,xboun,xbounact,iamboun,nboun,
              nodeboun,ndirboun,nodeforc,ndirforc,istep,&iinc,
	      co,vold,itg,&ntg,amname,ikboun,ilboun,nelemload,sideload,mi,
              ntrans,trab,inotr,veold,integerglob,doubleglob,tieset,istartset,
              iendset,ialset,ntie,nmpc,ipompc,ikmpc,ilmpc,nodempc,coefmpc,
              ipobody,iponoel,inoel,ipkon,kon,ielprop,prop,ielmat,
              shcon,nshcon,rhcon,nrhcon,cocon,ncocon,ntmat_,lakon));
      
      for(i=0;i<3;i++){cam[i]=0.;}for(i=3;i<5;i++){cam[i]=0.5;}
      if(*ithermal>1){
	  radflowload(itg,ieg,&ntg,&ntr,adrad,aurad,bcr,ipivr,
             ac,bc,nload,sideload,nelemload,xloadact,lakon,ipiv,ntmat_,vold,
             shcon,nshcon,ipkon,kon,co,
             kontri,&ntri,nloadtr,tarea,tenv,physcon,erad,&adview,&auview,
             nflow,ikboun,xbounact,nboun,ithermal,&iinc,&iit,
             cs,mcs,inocs,&ntrit,nk,fenv,istep,&dtime,ttime,&time,ilboun,
             ikforc,ilforc,xforcact,nforc,cam,ielmat,&nteq,prop,ielprop,
             nactdog,nacteq,nodeboun,ndirboun,network,
             rhcon,nrhcon,ipobody,ibody,xbodyact,nbody,iviewfile,jobnamef,
             ctrl,xloadold,&reltime,nmethod,set,mi,istartset,iendset,ialset,nset,
             ineighe,nmpc,nodempc,ipompc,coefmpc,labmpc,&iemchange,nam,iamload,
             jqrad,irowrad,&nzsrad,icolrad,ne,iaxial,qa,cocon,ncocon,iponoel,
             inoel,nprop,amname,namta,amta);
             
              /* check whether network iterations converged */

	  if(qa[2]>0){
	      checkdivergence(co,nk,kon,ipkon,lakon,ne,stn,nmethod, 
   	           kode,filab,een,t1act,&time,epn,ielmat,matname,enern, 
	           xstaten,nstate_,istep,&iinc,iperturb,ener,mi,output,
                   ithermal,qfn,&mode,&noddiam,trab,inotr,ntrans,orab,
	           ielorien,norien,description,sti,&icutb,&iit,&dtime,qa,
	           vold,qam,ram1,ram2,ram,cam,uam,&ntg,ttime,&icntrl,
	           &theta,&dtheta,veold,vini,idrct,tper,&istab,tmax, 
	           nactdof,b,tmin,ctrl,amta,namta,itpamp,inext,&dthetaref,
                   &itp,&jprint,jout,&uncoupled,t1,&iitterm,nelemload,
                   nload,nodeboun,nboun,itg,ndirboun,&deltmx,&iflagact,
	           set,nset,istartset,iendset,ialset,emn,thicke,jobnamec,
	           mortar,nmat,ielprop,prop,&ialeatoric,&kscale,
                   energy, &allwk,&energyref,&emax,&r_abs,&enetoll,energyini,
	           &allwkini,&temax,&sizemaxinc,&ne0,&neini,&dampwk,
	           &dampwkini,energystartstep);

              /* the divergence is flagged by icntrl!=0
                 icutb is reset to zero in order to generate
                 regular contact elements etc.. */

	      icutb--;
	  }
      }
      
      if(icfd==1){
	  compfluid(&co,nk,&ipkonf,konf,&lakonf,&sideface,
            ifreestream,&nfreestream,isolidsurf,neighsolidsurf,&nsolidsurf,
            nshcon,shcon,nrhcon,rhcon,&vold,ntmat_,nodeboun,
            ndirboun,nboun,ipompc,nodempc,nmpc,ikmpc,ilmpc,ithermal,
            ikboun,ilboun,&iturbulent,isolver,iexpl,ttime,
            &time,&dtime,nodeforc,ndirforc,xforc,nforc,nelemload,sideload,
            xload,nload,xbody,ipobody,nbody,ielmatf,matname,mi,ncmat_,
            physcon,istep,&iinc,ibody,xloadold,xboun,coefmpc,
            nmethod,xforcold,xforcact,iamforc,iamload,xbodyold,xbodyact,
            t1old,t1,t1act,iamt1,amta,namta,nam,ampli,xbounold,xbounact,
	    iamboun,itg,&ntg,amname,t0,&nelemface,&nface,cocon,ncocon,xloadact,
	    tper,jmax,jout,set,nset,istartset,iendset,ialset,prset,prlab,
	    nprint,trab,inotr,ntrans,filab,labmpc,sti,norien,orab,jobnamef,
	    tieset,ntie,mcs,ics,cs,nkon,&mpcfree,&memmpc_,fmpc,nef,&inomat,
	    qfx,neifa,neiel,ielfa,ifaext,vfa,vel,ipnei,&nflnei,&nfaext,
	    typeboun,neij,tincf,nactdoh,nactdohinv,ielorienf,jobnamec,
	    ifatie,nstate_,xstate,orname,kon,ctrl,kode,velo,veloo,
            &initial);

	  /* determining the global values to be used as boundary conditions
	     for a submodel */
	  
/*	  SFREE(integerglob);SFREE(doubleglob);
	  getglobalresults(jobnamec,&integerglob,&doubleglob,nboun,iamboun,
			   xboun,nload,sideload,iamload,&iglob,nforc,iamforc,
                           xforc,ithermal,nk,t1,iamt1,&sigma);

	  if(iglob<0){
	      printf(" *ERROR in nonlingeo: a submodel calculation for which\n");
	      printf("        the global model results from a *FREQUENCY\n");
	      printf("        calculation must be geometrically linear\n");
	      FORTRAN(stop,());
	      }*/
      }
      
      if(icascade==2){
	  memmpc_=memmpcref_;mpcfree=mpcfreeref;maxlenmpc=maxlenmpcref;
	  RENEW(nodempc,ITG,3*memmpcref_);
	  for(k=0;k<3*memmpcref_;k++){nodempc[k]=nodempcref[k];}
	  RENEW(coefmpc,double,memmpcref_);
	  for(k=0;k<memmpcref_;k++){coefmpc[k]=coefmpcref[k];}
      }

      /* generating contact elements */
      
      if((ncont!=0)&&(*mortar<=1)&&

/*       for purely thermal calculations: determine contact integration
         points only at the start of a step */

         ((*ithermal!=2)||(iit==-1))){

	  *ne=ne0;*nkon=nkon0;

	  /* at start of new increment: 
             - copy state variables (node-to-face)
	     - determine slave integration points (face-to-face)
	     - interpolate state variables (face-to-face) */

	  if(icutb==0){
	      if(*mortar==1){

		  if(*nstate_!=0){
		      if(maxprevcontel!=0){
			  if(iit!=-1){
			      NNEW(islavsurfold,ITG,2**ifacecount+2);
			      NNEW(pslavsurfold,double,3**nintpoint);
			      memcpy(&islavsurfold[0],&islavsurf[0],
				     sizeof(ITG)*(2**ifacecount+2));
			      memcpy(&pslavsurfold[0],&pslavsurf[0],
				     sizeof(double)*(3**nintpoint));
			  }
		      }
		  }

		  *nintpoint=0;

		  /* determine the location of the slave integration
                     points */

		  precontact(&ncont,ntie,tieset,nset,set,istartset,
                     iendset,ialset,itietri,lakon,ipkon,kon,koncont,ne,
                     cg,straight,co,vold,istep,&iinc,&iit,itiefac,
                     islavsurf,islavnode,imastnode,nslavnode,nmastnode,
                     imastop,mi,ipe,ime,tietol,&iflagact,
		     nintpoint,&pslavsurf,xmastnor,cs,mcs,ics,clearini,
                     nslavs);
		  
		  /* changing the dimension of element-related fields */
		  
		  RENEW(kon,ITG,*nkon+22**nintpoint);
		  RENEW(springarea,double,2**nintpoint);
		  RENEW(pmastsurf,double,6**nintpoint);
		  
		  if(*nener==1){
		      RENEW(ener,double,mi[0]*(*ne+*nintpoint)*2);

		      /* setting the entries for the friction contact energy to zero */

		      for(k=mi[0]*(2**ne+*nintpoint);k<mi[0]*(*ne+*nintpoint)*2;k++){ener[k]=0.;}

		  }
		  RENEW(ipkon,ITG,*ne+*nintpoint);
		  RENEW(lakon,char,8*(*ne+*nintpoint));
		  
		  if(*norien>0){
		      RENEW(ielorien,ITG,mi[2]*(*ne+*nintpoint));
		      for(k=mi[2]**ne;k<mi[2]*(*ne+*nintpoint);k++) ielorien[k]=0;
		  }
		  RENEW(ielmat,ITG,mi[2]*(*ne+*nintpoint));
		  for(k=mi[2]**ne;k<mi[2]*(*ne+*nintpoint);k++) ielmat[k]=1;

                  /* interpolating the state variables */

		  if(*nstate_!=0){
		      if(maxprevcontel!=0){
			  RENEW(xstateini,double,
                                *nstate_*mi[0]*(ne0+maxprevcontel));
			  for(k=*nstate_*mi[0]*ne0;
                                k<*nstate_*mi[0]*(ne0+maxprevcontel);++k){
			      xstateini[k]=xstate[k];
			  }
		      }
		      
		      RENEW(xstate,double,*nstate_*mi[0]*(ne0+*nintpoint));
		      for(k=*nstate_*mi[0]*ne0;k<*nstate_*mi[0]*(ne0+*nintpoint);k++){
			  xstate[k]=0.;
		      }
		      
		      if((*nintpoint>0)&&(maxprevcontel>0)){
			  iex=2;
			  
			  /* interpolation of xstate */
			  
			  FORTRAN(interpolatestate,(ne,ipkon,kon,lakon,
                               &ne0,mi,xstate,pslavsurf,nstate_,
                               xstateini,islavsurf,islavsurfold,
			       pslavsurfold,tieset,ntie,itiefac));
			  
		      }

		      if(maxprevcontel!=0){
			  SFREE(islavsurfold);SFREE(pslavsurfold);
		      }

		      maxprevcontel=*nintpoint;

		      RENEW(xstateini,double,*nstate_*mi[0]*(ne0+*nintpoint));
		      for(k=0;k<*nstate_*mi[0]*(ne0+*nintpoint);++k){
			  xstateini[k]=xstate[k];
		      }
		  }

	      }
	  }

	  contact(&ncont,ntie,tieset,nset,set,istartset,iendset,
		  ialset,itietri,lakon,ipkon,kon,koncont,ne,cg,straight,nkon,
		  co,vold,ielmat,cs,elcon,istep,&iinc,&iit,ncmat_,ntmat_,
		  &ne0,vini,nmethod,
		  iperturb,ikboun,nboun,mi,imastop,nslavnode,islavnode,
                  islavsurf,
		  itiefac,areaslav,iponoels,inoels,springarea,tietol,&reltime,
		  imastnode,nmastnode,xmastnor,filab,mcs,ics,&nasym,
		  xnoels,mortar,pslavsurf,pmastsurf,clearini,&theta,
	          xstateini,xstate,nstate_,&icutb,&ialeatoric,jobnamef,
                  &alea);
   
	  /* check whether, for a dynamic calculation, damping is involved */

	  if(*nmethod==4){
	      if(*iexpl<=1){
		  if(idampingwithoutcontact==0){
		      for(i=0;i<*ne;i++){
			  if(ipkon[i]<0) continue;
			  if(*ncmat_>=5){
			      if(strcmp1(&lakon[i*8],"ES")==0){
				  if(strcmp1(&lakon[i*8+6],"C")==0){
				      imat=ielmat[i*mi[2]];
				      if(elcon[(*ncmat_+1)**ntmat_*(imat-1)+4]>0.){
					  idamping=1;break;
				      }
				  }
			      }
			  }
		      }
		  }
	      }
	  }
	  
	  printf(" Number of contact spring elements=%" ITGFORMAT "\n\n",*ne-ne0);
            
	  /* carlo start */

	  if((*iexpl>1)){
	      
	      if((*ne-ne0)<ncontacts){
		  ncontacts=*ne-ne0;
		  inccontact=0;
	      }  
	      else if((*ne-ne0)>ncontacts)  {			
		  
		  FORTRAN(calcstabletimeinccont,(ne,lakon,kon,ipkon,mi,
			  ielmat,elcon,mortar,adb,alpha,nactdof,springarea,
			  &ne0,ntmat_,ncmat_,&dtcont));

		  if(dtcont<*tinc)*tinc=dtcont;
		  dtheta=(*tinc)/(*tper);
		  dthetaref=dtheta;

		  ncontacts=*ne-ne0; 
		  inccontact=0;
	      }else if((inccontact==500)&&(ncontacts==0)){
		  *tinc=dtvol;
		  dtheta=(*tinc)/(*tper);
		  dthetaref=dtheta;

		  dtcont=1.e30;
	      } 
	      inccontact++;
	  }
	  
          /* carlo end */

      }
      
      /*  updating the nonlinear mpc's (also affects the boundary
	  conditions through the nonhomogeneous part of the mpc's) */
      
      FORTRAN(nonlinmpc,(co,vold,ipompc,nodempc,coefmpc,labmpc,
			 nmpc,ikboun,ilboun,nboun,xbounact,aux,iaux,
			 &maxlenmpc,ikmpc,ilmpc,&icascade,
			 kon,ipkon,lakon,ne,&reltime,&newstep,xboun,fmpc,
			 &iit,&idiscon,&ncont,trab,ntrans,ithermal,mi));
      
      if(icascade==2){
	  for(k=0;k<3*memmpc_;k++){nodempcref[k]=nodempc[k];}
	  for(k=0;k<memmpc_;k++){coefmpcref[k]=coefmpc[k];}
      }
      
      if((icascade>0)||(ncont!=0)) remastruct(ipompc,&coefmpc,&nodempc,nmpc,
	  &mpcfree,nodeboun,ndirboun,nboun,ikmpc,ilmpc,ikboun,ilboun,
	  labmpc,nk,&memmpc_,&icascade,&maxlenmpc,
	  kon,ipkon,lakon,ne,nactdof,icol,jq,&irow,isolver,
	  neq,nzs,nmethod,&f,&fext,&b,&aux2,&fini,&fextini,
	  &adb,&aub,ithermal,iperturb,mass,mi,iexpl,mortar,
	  typeboun,&cv,&cvini,&iit,network);
      
      /* invert nactdof */
      
      SFREE(nactdofinv);
      NNEW(nactdofinv,ITG,mt**nk);
      NNEW(nodorig,ITG,*nk);
      FORTRAN(gennactdofinv,(nactdof,nactdofinv,nk,mi,nodorig,
			     ipkon,lakon,kon,ne));
      SFREE(nodorig);
      
      /* check whether the forced displacements changed; if so, and
	 if the procedure is static, the first iteration has to be
	 purely linear elastic, in order to get an equilibrium
	 displacement field; otherwise huge (maybe nonelastic)
	 stresses may occur, jeopardizing convergence */
      
      ilin=0;
      
      /* only for iinc=1 a linearized calculation is performed, since
	 for iinc>1 a reasonable displacement field is predicted by using the
	 initial velocity field at the end of the last increment */
      
      if((iinc==1)&&(*ithermal<2)){
	  dev=0.;
	  for(k=0;k<*nboun;++k){
	      err=fabs(xbounact[k]-xbounini[k]);
	      if(err>dev){dev=err;}
	  }
	  if(dev>1.e-5) ilin=1;
      }
      
      /* prediction of the kinematic vectors  */
      
      NNEW(v,double,mt**nk);
      
      prediction(uam,nmethod,&bet,&gam,&dtime,ithermal,nk,veold,accold,v,
		 &iinc,&idiscon,vold,nactdof,mi);
      
      NNEW(fn,double,mt**nk);
      NNEW(stx,double,6*mi[0]**ne);
      
      /* determining the internal forces at the start of the increment
	 
	 for a static calculation with increased forced displacements
	 the linear strains are calculated corresponding to
	 
	 the displacements at the end of the previous increment, extrapolated
	 if appropriate (for nondispersive media) +
	 the forced displacements at the end of the present increment +
	 the temperatures at the end of the present increment (this sum is
	 v) -
	 the displacements at the end of the previous increment (this is vold)
	 
	 these linear strains are converted in stresses by multiplication
	 with the tangent element stiffness matrix and converted into nodal
	 forces. 
	 
	 this boils down to the fact that the effect of forced displacements
	 should be handled in a purely linear way at the
	 start of a new increment, in order to speed up the convergence and
	 (for dissipative media) guarantee smooth loading within the increment.
	 
	 for all other cases the nodal force calculation is based on
	 the true stresses derived from the appropriate strain tensor taking
	 into account the extrapolated displacements at the end of the 
	 previous increment + the forced displacements and the temperatures
	 at the end of the present increment */
      
      iout=-1;
      if(istrainfree==1) iout=-2;
      iperturb_sav[0]=iperturb[0];
      iperturb_sav[1]=iperturb[1];
      
      /* first iteration in first increment: elastic tangent */
      
      if((*nmethod!=4)&&(ilin==1)){
	  
	  ielas=1;
	  
	  iperturb[0]=-1;
	  iperturb[1]=0;
	  
	  for(k=0;k<neq[1];++k){b[k]=f[k];}
	  NNEW(inum,ITG,*nk);
	  results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
		  ielorien,norien,orab,ntmat_,t1ini,t1act,ithermal,
		  prestr,iprestr,filab,eme,emn,een,iperturb,
		  f,fn,nactdof,&iout,qa,vold,b,nodeboun,
		  ndirboun,xbounact,nboun,ipompc,
		  nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
		  &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
		  xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
		  &icmd, ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
		  emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
		  iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
		  fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
		  &reltime,&ne0,thicke,shcon,nshcon,
		  sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
		  mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
                  islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
                  inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
	  iperturb[0]=0;SFREE(inum);
	  
	  /* check whether any displacements or temperatures are changed
	     in the new increment */
	  
	  for(k=0;k<neq[1];++k){f[k]=f[k]+b[k];}
	  
      }
      else{
	  
	  NNEW(inum,ITG,*nk);
	  results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
		  ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
		  prestr,iprestr,filab,eme,emn,een,iperturb,
		  f,fn,nactdof,&iout,qa,vold,b,nodeboun,
		  ndirboun,xbounact,nboun,ipompc,
		  nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
		  &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
		  xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
		  &icmd,ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
		  emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
		  iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
		  fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
		  &reltime,&ne0,thicke,shcon,nshcon,
		  sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
		  mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
                  islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
                  inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
	  SFREE(inum);
	  
	  memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);
	  
	  if(*ithermal!=2){
	      for(k=0;k<6*mi[0]*ne0;++k){
		  sti[k]=stx[k];
	      }
	  }
	  
      }
      
      ielas=0;
      iout=0;
      
      SFREE(fn);SFREE(v);
      if((*ithermal!=3)||(ncont==0)||(*mortar!=1)||(*ncmat_<11)) SFREE(stx);
      
      /***************************************************************/
      /* iteration counter and start of the loop over the iterations */
      /***************************************************************/

    iit=1;
//    icntrl=0;

    /* change due to previous checkdivergence routine */

    if(icntrl!=0) icutb++;

    ctrl[0]=i0ref;ctrl[1]=irref;ctrl[3]=icref;
    if(*nmethod!=4)NNEW(resold,double,neq[1]);
    if(uncoupled){
	*ithermal=2;
	NNEW(iruc,ITG,nzs[1]-nzs[0]);
	for(k=0;k<nzs[1]-nzs[0];k++) iruc[k]=irow[k+nzs[0]]-neq[0];
    }

    while(icntrl==0){

#ifdef COMPANY
	  FORTRAN(uiter,(&iit));
#endif	  

    /*  updating the nonlinear mpc's (also affects the boundary
	conditions through the nonhomogeneous part of the mpc's) */

      if((iit!=1)||((uncoupled)&&(*ithermal==1))){

	  printf(" iteration %" ITGFORMAT "\n\n",iit);

	  /* restoring the distributed loading before adding the
	     friction heating */
	  
	  if((*ithermal==3)&&(ncont!=0)&&(*mortar==1)&&(*ncmat_>=11)){
	      *nload=nloadref;
	      memcpy(&nelemload[0],&nelemloadref[0],sizeof(ITG)*2**nload);
	      if(*nam>0){
		  memcpy(&iamload[0],&iamloadref[0],sizeof(ITG)*2**nload);
	      }
	      memcpy(&sideload[0],&sideloadref[0],sizeof(char)*20**nload);
	  }
	  
          FORTRAN(tempload,(xforcold,xforc,xforcact,iamforc,nforc,
              xloadold,xload,
	      xloadact,iamload,nload,ibody,xbody,nbody,xbodyold,xbodyact,
	      t1old,t1,t1act,iamt1,nk,amta,
	      namta,nam,ampli,&time,&reltime,ttime,&dtime,ithermal,nmethod,
              xbounold,xboun,xbounact,iamboun,nboun,
              nodeboun,ndirboun,nodeforc,ndirforc,istep,&iinc,
	      co,vold,itg,&ntg,amname,ikboun,ilboun,nelemload,sideload,mi,
              ntrans,trab,inotr,veold,integerglob,doubleglob,tieset,istartset,
              iendset,ialset,ntie,nmpc,ipompc,ikmpc,ilmpc,nodempc,coefmpc,
              ipobody,iponoel,inoel,ipkon,kon,ielprop,prop,ielmat,
              shcon,nshcon,rhcon,nrhcon,cocon,ncocon,ntmat_,lakon));

	  for(i=0;i<3;i++){cam[i]=0.;}for(i=3;i<5;i++){cam[i]=0.5;}
	  if(*ithermal>1){
	      radflowload(itg,ieg,&ntg,&ntr,adrad,aurad,bcr,ipivr,
	        ac,bc,nload,sideload,nelemload,xloadact,lakon,ipiv,
                ntmat_,vold,shcon,nshcon,ipkon,kon,co,
	        kontri,&ntri,nloadtr,tarea,tenv,physcon,erad,&adview,&auview,
	        nflow,ikboun,xbounact,nboun,ithermal,&iinc,&iit,
                cs,mcs,inocs,&ntrit,nk,fenv,istep,&dtime,ttime,&time,ilboun,
	        ikforc,ilforc,xforcact,nforc,cam,ielmat,&nteq,prop,ielprop,
	        nactdog,nacteq,nodeboun,ndirboun,network,
                rhcon,nrhcon,ipobody,ibody,xbodyact,nbody,iviewfile,jobnamef,
	        ctrl,xloadold,&reltime,nmethod,set,mi,istartset,iendset,ialset,
	        nset,ineighe,nmpc,nodempc,ipompc,coefmpc,labmpc,&iemchange,nam,
	        iamload,jqrad,irowrad,&nzsrad,icolrad,ne,iaxial,qa,cocon,ncocon,
		iponoel,inoel,nprop,amname,namta,amta);
             
              /* check whether network iterations converged */

	      if(qa[2]>0){
		  checkdivergence(co,nk,kon,ipkon,lakon,ne,stn,nmethod, 
   	           kode,filab,een,t1act,&time,epn,ielmat,matname,enern, 
	           xstaten,nstate_,istep,&iinc,iperturb,ener,mi,output,
                   ithermal,qfn,&mode,&noddiam,trab,inotr,ntrans,orab,
	           ielorien,norien,description,sti,&icutb,&iit,&dtime,qa,
	           vold,qam,ram1,ram2,ram,cam,uam,&ntg,ttime,&icntrl,
	           &theta,&dtheta,veold,vini,idrct,tper,&istab,tmax, 
	           nactdof,b,tmin,ctrl,amta,namta,itpamp,inext,&dthetaref,
                   &itp,&jprint,jout,&uncoupled,t1,&iitterm,nelemload,
                   nload,nodeboun,nboun,itg,ndirboun,&deltmx,&iflagact,
	           set,nset,istartset,iendset,ialset,emn,thicke,jobnamec,
	           mortar,nmat,ielprop,prop,&ialeatoric,&kscale,
                   energy, &allwk,&energyref,&emax,&r_abs,&enetoll,energyini,
	           &allwkini,&temax,&sizemaxinc,&ne0,&neini,&dampwk,
	           &dampwkini,energystartstep);
		  continue;
	      }
	  }

	  if(icascade==2){
	      memmpc_=memmpcref_;mpcfree=mpcfreeref;maxlenmpc=maxlenmpcref;
	      RENEW(nodempc,ITG,3*memmpcref_);
	      for(k=0;k<3*memmpcref_;k++){nodempc[k]=nodempcref[k];}
	      RENEW(coefmpc,double,memmpcref_);
	      for(k=0;k<memmpcref_;k++){coefmpc[k]=coefmpcref[k];}
	  }

	  if((ncont!=0)&&(*mortar<=1)&&(ismallsliding==0)&&
/*           for node-to-face contact: freeze contact elements for
             iterations 8 and higher */
             ((iit<=8)||(*mortar==1))&&
/*           for purely thermal calculations: freeze contact elements
             during complete step */
             ((*ithermal!=2)||(iit==-1))){

	      neold=*ne;
	      *ne=ne0;*nkon=nkon0;
	      contact(&ncont,ntie,tieset,nset,set,istartset,iendset,
		      ialset,itietri,lakon,ipkon,kon,koncont,ne,cg,
                      straight,nkon,co,vold,ielmat,cs,elcon,istep,
                      &iinc,&iit,ncmat_,ntmat_,&ne0,
                      vini,nmethod,iperturb,
                      ikboun,nboun,mi,imastop,nslavnode,islavnode,islavsurf,
                      itiefac,areaslav,iponoels,inoels,springarea,tietol,
                      &reltime,imastnode,nmastnode,xmastnor,
                      filab,mcs,ics,&nasym,xnoels,mortar,pslavsurf,pmastsurf,
                      clearini,&theta,xstateini,xstate,nstate_,&icutb,
                      &ialeatoric,jobnamef,&alea);

	      /* check whether, for a dynamic calculation, damping is involved */
	      
	      if(*nmethod==4){
		  if(*iexpl<=1){
		      if(idampingwithoutcontact==0){
			  for(i=0;i<*ne;i++){
			      if(ipkon[i]<0) continue;
			      if(*ncmat_>=5){
				  if(strcmp1(&lakon[i*8],"ES")==0){
				      if(strcmp1(&lakon[i*8+6],"C")==0){
					  imat=ielmat[i*mi[2]];
					  if(elcon[(*ncmat_+1)**ntmat_*(imat-1)+4]>0.){
					      idamping=1;break;
					  }
				      }
				  }
			      }
			  }
		      }
		  }
	      }
	      
	      if(*mortar==0){
	         if(*ne!=neold){iflagact=1;}
	      }else if(*mortar==1){
/*	         if(((*ne-ne0)<(neold-ne0)*0.999)||
		 ((*ne-ne0)>(neold-ne0)*1.001)){iflagact=1;}*/
		  if(((*ne-ne0)<(neold-ne0)*(1.-delcon))||
		     ((*ne-ne0)>(neold-ne0)*(1.+delcon))){iflagact=1;}
              }

	      printf(" Number of contact spring elements=%" ITGFORMAT "\n\n",*ne-ne0);

	  }
	  
	  if(*ithermal==3){
	      for(k=0;k<*nk;++k){t1act[k]=vold[mt*k];}
	  }

	  FORTRAN(nonlinmpc,(co,vold,ipompc,nodempc,coefmpc,labmpc,
		nmpc,ikboun,ilboun,nboun,xbounact,aux,iaux,
	        &maxlenmpc,ikmpc,ilmpc,&icascade,
	        kon,ipkon,lakon,ne,&reltime,&newstep,xboun,fmpc,&iit,
		&idiscon,&ncont,trab,ntrans,ithermal,mi));

	  if(icascade==2){
	      for(k=0;k<3*memmpc_;k++){nodempcref[k]=nodempc[k];}
	      for(k=0;k<memmpc_;k++){coefmpcref[k]=coefmpc[k];}
	  }

	  if((icascade>0)||(ncont!=0)){
	      remastruct(ipompc,&coefmpc,&nodempc,nmpc,
		&mpcfree,nodeboun,ndirboun,nboun,ikmpc,ilmpc,ikboun,ilboun,
		labmpc,nk,&memmpc_,&icascade,&maxlenmpc,
		kon,ipkon,lakon,ne,nactdof,icol,jq,&irow,isolver,
		neq,nzs,nmethod,&f,&fext,&b,&aux2,&fini,&fextini,
		&adb,&aub,ithermal,iperturb,mass,mi,iexpl,mortar,
		typeboun,&cv,&cvini,&iit,network);

	      /* invert nactdof */
	      
	      SFREE(nactdofinv);
	      NNEW(nactdofinv,ITG,mt**nk);
	      NNEW(nodorig,ITG,*nk);
	      FORTRAN(gennactdofinv,(nactdof,nactdofinv,nk,mi,nodorig,
				     ipkon,lakon,kon,ne));
	      SFREE(nodorig);
	      
	      NNEW(v,double,mt**nk);
	      NNEW(stx,double,6*mi[0]**ne);
	      NNEW(fn,double,mt**nk);
      
	      memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
	      iout=-1;
	      
	      NNEW(inum,ITG,*nk);
	      results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	        elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
		ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
		prestr,iprestr,filab,eme,emn,een,iperturb,
		f,fn,nactdof,&iout,qa,vold,b,nodeboun,
		ndirboun,xbounact,nboun,ipompc,
		nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
		&bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
		xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,&icmd,
		ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,emeini,
		xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,iendset,
	        ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,fmpc,
		nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
		&reltime,&ne0,thicke,shcon,nshcon,
                sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
		mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
		islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
		inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
	  
	      memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);
	      
	      if(*ithermal!=2){
		  for(k=0;k<6*mi[0]*ne0;++k){
		      sti[k]=stx[k];
		  }
	      }

	      /*for(k=0;k<neq[1];++k){printf("f=%" ITGFORMAT ",%f\n",k,f[k]);}*/
	      
	      SFREE(v);SFREE(fn);SFREE(inum);
	      if((*ithermal!=3)||(ncont==0)||(*mortar!=1)||(*ncmat_<11)) SFREE(stx);
	      iout=0;
	      
	  }else{

	      /*for(k=0;k<neq[1];++k){printf("f=%" ITGFORMAT ",%f\n",k,f[k]);}*/
	  }
      }
	  
      /* add friction heating  */
      
      if((*ithermal==3)&&(ncont!=0)&&(*mortar==1)&&(*ncmat_>=11)){
	  nload_=*nload+2*(*ne-ne0);

	  RENEW(nelemload,ITG,2*nload_);
	  DMEMSET(nelemload,2**nload,2*nload_,0);
	  if(*nam>0){
	      RENEW(iamload,ITG,2*nload_);
	      DMEMSET(iamload,2**nload,2*nload_,0);
	  }
	  RENEW(xloadact,double,2*nload_);
	  DMEMSET(xloadact,2**nload,2*nload_,0.);
	  RENEW(sideload,char,20*nload_);
	  DMEMSET(sideload,20**nload,20*nload_,'\0');

	  NNEW(idefload,ITG,nload_);
	  DMEMSET(idefload,0,nload_,1);
	  FORTRAN(frictionheating,(&ne0,ne,ipkon,lakon,ielmat,mi,elcon,
		  ncmat_,ntmat_,kon,islavsurf,pmastsurf,springarea,co,vold,
                  veold,pslavsurf,xloadact,nload,&nload_,nelemload,iamload,
		  idefload,sideload,stx,nam,&time,ttime,matname,istep,&iinc));
	  SFREE(idefload);SFREE(stx);
      }
      
      if(*iexpl<=1){

	/* calculating the local stiffness matrix and external loading */

	NNEW(ad,double,neq[1]);
	NNEW(au,double,nzs[1]);

	if(*nmethod==4) DMEMSET(fnext,0,mt**nk,0.);

	mafillsmmain(co,nk,kon,ipkon,lakon,ne,nodeboun,ndirboun,xbounact,nboun,
		  ipompc,nodempc,coefmpc,nmpc,nodeforc,ndirforc,xforcact,
		  nforc,nelemload,sideload,xloadact,nload,xbodyact,ipobody,
		  nbody,cgr,ad,au,fext,nactdof,icol,jq,irow,neq,nzl,
		  nmethod,ikmpc,ilmpc,ikboun,ilboun,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,
		  ielmat,ielorien,norien,orab,ntmat_,
		  t0,t1act,ithermal,prestr,iprestr,vold,iperturb,sti,
		  nzs,stx,adb,aub,iexpl,plicon,nplicon,plkcon,nplkcon,
		  xstiff,npmat_,&dtime,matname,mi,
                  ncmat_,mass,&stiffness,&buckling,&rhsi,&intscheme,
                  physcon,shcon,nshcon,cocon,ncocon,ttime,&time,istep,&iinc,
		  &coriolis,ibody,xloadold,&reltime,veold,springarea,nstate_,
                  xstateini,xstate,thicke,integerglob,doubleglob,
		  tieset,istartset,iendset,ialset,ntie,&nasym,pslavsurf,
		  pmastsurf,mortar,clearini,ielprop,prop,&ne0,fnext,&kscale,
		  iponoel,inoel,network,ntrans,inotr,trab,design,penal);

	if(nasym==1){
	    RENEW(au,double,2*nzs[1]);
	    if(*nmethod==4) RENEW(aub,double,2*nzs[1]);
	    symmetryflag=2;
	    inputformat=1;

	    FORTRAN(mafillsmas,(co,nk,kon,ipkon,lakon,ne,nodeboun,
                  ndirboun,xbounact,nboun,
		  ipompc,nodempc,coefmpc,nmpc,nodeforc,ndirforc,xforcact,
		  nforc,nelemload,sideload,xloadact,nload,xbodyact,ipobody,
		  nbody,cgr,ad,au,fext,nactdof,icol,jq,irow,neq,nzl,
		  nmethod,ikmpc,ilmpc,ikboun,ilboun,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,
		  ielmat,ielorien,norien,orab,ntmat_,
		  t0,t1act,ithermal,prestr,iprestr,vold,iperturb,sti,
		  nzs,stx,adb,aub,iexpl,plicon,nplicon,plkcon,nplkcon,
		  xstiff,npmat_,&dtime,matname,mi,
                  ncmat_,mass,&stiffness,&buckling,&rhsi,&intscheme,
                  physcon,shcon,nshcon,cocon,ncocon,ttime,&time,istep,&iinc,
                  &coriolis,ibody,xloadold,&reltime,veold,springarea,nstate_,
                  xstateini,xstate,thicke,
                  integerglob,doubleglob,tieset,istartset,iendset,
		  ialset,ntie,&nasym,pslavsurf,pmastsurf,mortar,clearini,
		  ielprop,prop,&ne0,&kscale,iponoel,inoel,network));
	}
	    

	iperturb[0]=iperturb_sav[0];
	iperturb[1]=iperturb_sav[1];

      }else{

	/* calculating the external loading 

	   This is only done once per increment. In reality, the
           external loading is a function of vold (specifically,
           the body forces and surface loading). This effect is
           neglected, since the increment size in dynamic explicit
           calculations is usually small */

	  FORTRAN(rhs,(co,nk,kon,ipkon,lakon,ne,
		  ipompc,nodempc,coefmpc,nmpc,nodeforc,ndirforc,xforcact,
		  nforc,nelemload,sideload,xloadact,nload,xbodyact,ipobody,
		  nbody,cgr,fext,nactdof,&neq[1],
		  nmethod,ikmpc,ilmpc,
		  elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,
		  ielmat,ielorien,norien,orab,ntmat_,
		  t0,t1act,ithermal,iprestr,vold,iperturb,
		  iexpl,plicon,nplicon,plkcon,nplkcon,
		  npmat_,ttime,&time,istep,&iinc,&dtime,physcon,ibody,
		  xbodyold,&reltime,veold,matname,mi,ikactmech,
		  &nactmech,ielprop,prop,sti,xstateini,xstate,nstate_,
                  ntrans,inotr,trab));
      }
      
/*      for(k=0;k<neq[1];++k){printf("f=%" ITGFORMAT ",%f\n",k,f[k]);}
      for(k=0;k<neq[1];++k){printf("fext=%" ITGFORMAT ",%f\n",k,fext[k]);}
      for(k=0;k<neq[1];++k){printf("ad=%" ITGFORMAT ",%f\n",k,ad[k]);}
      for(k=0;k<nzs[1];++k){printf("au=%" ITGFORMAT ",%f\n",k,au[k]);}*/

      /* calculating the damping matrix for implicit dynamic
         calculations */

      if(idamping==1){

          /* Rayleigh damping */

	  NNEW(adc,double,neq[1]);
	  for(k=0;k<neq[0];k++){adc[k]=alpham*adb[k]+betam*ad[k];}
	  if(nasym==0){
	      NNEW(auc,double,nzs[1]);
	      for(k=0;k<nzs[0];k++){auc[k]=alpham*aub[k]+betam*au[k];}
	  }else{
	      NNEW(auc,double,2*nzs[1]);
	      for(k=0;k<2*nzs[0];k++){auc[k]=alpham*aub[k]+betam*au[k];}
	  }
 
          /* dashpots and contact damping */

	  FORTRAN(mafilldm,(co,nk,kon,ipkon,lakon,ne,nodeboun,
              ndirboun,xbounact,nboun,
	      ipompc,nodempc,coefmpc,nmpc,nodeforc,ndirforc,xforcact,
	      nforc,nelemload,sideload,xloadact,nload,xbodyact,
              ipobody,nbody,cgr,
	      adc,auc,nactdof,icol,jq,irow,neq,nzl,nmethod,
	      ikmpc,ilmpc,ikboun,ilboun,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,
	      t0,t1act,ithermal,prestr,iprestr,vold,iperturb,sti,
	      nzs,stx,adb,aub,iexpl,plicon,nplicon,plkcon,nplkcon,
	      xstiff,npmat_,&dtime,matname,mi,ncmat_,
	      ttime,&time,istep,&iinc,ibody,clearini,mortar,springarea,
              pslavsurf,pmastsurf,&reltime,&nasym));
      }

      /* calculating the residual */

      calcresidual(nmethod,neq,b,fext,f,iexpl,nactdof,aux2,vold,
	 vini,&dtime,accold,nk,adb,aub,jq,irow,nzl,alpha,fextini,fini,
	 islavnode,nslavnode,mortar,ntie,f_cm,f_cs,mi,
	 nzs,&nasym,&idamping,veold,adc,auc,cvini,cv);

      /* storing the residuum in resold (for line search) */

      if((*mortar==1)&&(iit!=1)&&(*ne-ne0>0)&(*nmethod!=4)){memcpy(&resold[0],&b[0],sizeof(double)*neq[1]);}
	  
      newstep=0;
      
      if(*nmethod==0){
	  
	  /* error occurred in mafill: storing the geometry in frd format */
	  
	  *nmethod=0;
	  ++*kode;
	  NNEW(inum,ITG,*nk);for(k=0;k<*nk;k++) inum[k]=1;
	  if(strcmp1(&filab[1044],"ZZS")==0){
	      NNEW(neigh,ITG,40**ne);
	      NNEW(ipneigh,ITG,*nk);
	  }
	  
	  ptime=*ttime+time;
	  frd(co,nk,kon,ipkon,lakon,&ne0,v,stn,inum,nmethod,
	      kode,filab,een,t1,fn,&ptime,epn,ielmat,matname,enern,xstaten,
	      nstate_,istep,&iinc,ithermal,qfn,&mode,&noddiam,trab,inotr,
	      ntrans,orab,ielorien,norien,description,ipneigh,neigh,
	      mi,sti,vr,vi,stnr,stni,vmax,stnmax,&ngraph,veold,ener,ne,
	      cs,set,nset,istartset,iendset,ialset,eenmax,fnr,fni,emn,
	      thicke,jobnamec,output,qfx,cdn,mortar,cdnr,cdni,nmat,
	      ielprop,prop);

	  if(strcmp1(&filab[1044],"ZZS")==0){SFREE(ipneigh);SFREE(neigh);} 
     #ifdef COMPANY
	  FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif
	  SFREE(inum);FORTRAN(stop,());
	  
      }
      
      /* implicit step (static or dynamic) */
      
      if(*iexpl<=1){
	  if((*nmethod==4)&&(*mortar<2)){
	      
	      /* mechanical part */
	      
	      if(*ithermal!=2){
		  scal1=bet*dtime*dtime*(1.+*alpha);
		  for(k=0;k<neq[0];++k){
		      ad[k]=adb[k]+scal1*ad[k];
		  }
		  for(k=0;k<nzs[0];++k){
		      au[k]=aub[k]+scal1*au[k];
		  }
		  
		  /* upper triangle of asymmetric matrix */
		  
		  if(nasym>0){
		      for(k=nzs[2];k<nzs[2]+nzs[0];++k){
			  au[k]=aub[k]+scal1*au[k];
		      }
		  }

                  /* damping */
		  
		  if(idamping==1){
		      scal1=gam*dtime*(1.+*alpha);
		      for(k=0;k<neq[0];++k){
			  ad[k]+=scal1*adc[k];
		      }
		      for(k=0;k<nzs[0];++k){
			  au[k]+=scal1*auc[k];
		      }
		      
		      /* upper triangle of asymmetric matrix */
		      
		      if(nasym>0){
			  for(k=nzs[2];k<nzs[2]+nzs[0];++k){
			      au[k]+=scal1*auc[k];
			  }
		      }
		  }

	      }
	      
	      /* thermal part */
	      
	      if(*ithermal>1){
		  for(k=neq[0];k<neq[1];++k){
		      ad[k]=adb[k]/dtime+ad[k];
		  }
		  for(k=nzs[0];k<nzs[1];++k){
		      au[k]=aub[k]/dtime+au[k];
		  }
		  
		  /* upper triangle of asymmetric matrix */
		  
		  if(nasym>0){
		      for(k=nzs[2]+nzs[0];k<nzs[2]+nzs[1];++k){
			  au[k]=aub[k]/dtime+au[k];
		      }
		  }
	      }
	  }
	  
	  if(*isolver==0){
#ifdef SPOOLES
	      if(*ithermal<2){
//		  predgmres_struct_mt(ad,&au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],
//			       &symmetryflag,&inputformat,jq,&nzs[2],&nrhs);
		  spooles(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],
		  			  &symmetryflag,&inputformat,&nzs[2]);
	      }else if((*ithermal==2)&&(uncoupled)){
		  n1=neq[1]-neq[0];
		  n2=nzs[1]-nzs[0];
		  spooles(&ad[neq[0]],&au[nzs[0]],&adb[neq[0]],&aub[nzs[0]],
			  &sigma,&b[neq[0]],&icol[neq[0]],iruc,
			  &n1,&n2,&symmetryflag,&inputformat,&nzs[2]);
	      }else{
		  spooles(ad,au,adb,aub,&sigma,b,icol,irow,&neq[1],&nzs[1],
			  &symmetryflag,&inputformat,&nzs[2]);
	      }
#else
	      printf(" *ERROR in nonlingeo: the SPOOLES library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if((*isolver==2)||(*isolver==3)){
	      if(nasym>0){
		  if(*isolver==3){
		      printf(" *WARNING in nonlingeo: the iterative Cholesky solver cannot be used for asymmetric matrices.\nThe iterative scaling solver will be used instead\n\n");
		  }
		  NNEW(rwork,double,neq[1]);
		  NNEW(sol,double,neq[1]);
		  RENEW(au,double,2*nzs[1]+neq[1]);
		  memcpy(&au[2*nzs[1]],ad,sizeof(double)*neq[1]);
		  nelt=2*nzs[1]+neq[1];
		  lrgw=131+16*neq[1];
		  isym=0;
		  NNEW(rgwk,double,lrgw);
		  NNEW(igwk,ITG,20);
		  for(i=0;i<neq[1];i++){rwork[i]=1./ad[i];}
		  FORTRAN(predgmres_struct,(&neq[1],b,sol,&nelt,irow,jq,au,
				     &isym,&itol,&tol,&itmax,&iter,
				     &err,&ierr,&iunit,sb,sx,rgwk,&lrgw,igwk,
				     &ligw,rwork,iwork));
		  memcpy(b,sol,sizeof(double)*neq[1]);
		  SFREE(rgwk);SFREE(igwk);SFREE(rwork);SFREE(sol);
	      }else{
		  preiter(ad,&au,b,&icol,&irow,&neq[1],&nzs[1],isolver,iperturb);
	      }
	  }
	  else if(*isolver==4){
#ifdef SGI
	      if(nasym>0){
		  printf(" *ERROR in nonlingeo: the SGI solver cannot be used for asymmetric matrices\n\n");
		  FORTRAN(stop,());
	      }
	      token=1;
	      if(*ithermal<2){
		  sgi_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],token);
	      }else if((*ithermal==2)&&(uncoupled)){
		  n1=neq[1]-neq[0];
		  n2=nzs[1]-nzs[0];
		  sgi_main(&ad[neq[0]],&au[nzs[0]],&adb[neq[0]],&aub[nzs[0]],
			   &sigma,&b[neq[0]],&icol[neq[0]],iruc,
			   &n1,&n2,token);
	      }else{
		  sgi_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[1],&nzs[1],token);
	      }
#else
	      printf(" *ERROR in nonlingeo: the SGI library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if(*isolver==5){
#ifdef TAUCS
	      if(nasym>0){
		  printf(" *ERROR in nonlingeo: the TAUCS solver cannot be used for asymmetric matrices\n\n");
		  FORTRAN(stop,());
	      }
	      tau(ad,&au,adb,aub,&sigma,b,icol,&irow,&neq[1],&nzs[1]);
#else
	      printf(" *ERROR in nonlingeo: the TAUCS library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if(*isolver==6){
#ifdef MATRIXSTORAGE
	      matrixstorage(ad,&au,adb,aub,&sigma,icol,&irow,&neq[1],&nzs[1],
			    ntrans,inotr,trab,co,nk,nactdof,jobnamec,mi,ipkon,
			    lakon,kon,ne,mei,nboun,nmpc,cs,mcs,ithermal,nmethod);
#else
	      printf("*ERROR in arpack: the MATRIXSTORAGE library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  else if(*isolver==7){
#ifdef PARDISO
	      if(*ithermal<2){
		  pardiso_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[0],&nzs[0],
			       &symmetryflag,&inputformat,jq,&nzs[2],&nrhs);
	      }else if((*ithermal==2)&&(uncoupled)){
		  n1=neq[1]-neq[0];
		  n2=nzs[1]-nzs[0];
		  pardiso_main(&ad[neq[0]],&au[nzs[0]],&adb[neq[0]],&aub[nzs[0]],
			       &sigma,&b[neq[0]],&icol[neq[0]],iruc,
			       &n1,&n2,&symmetryflag,&inputformat,jq,&nzs[2],&nrhs);
	      }else{
		  pardiso_main(ad,au,adb,aub,&sigma,b,icol,irow,&neq[1],&nzs[1],
			       &symmetryflag,&inputformat,jq,&nzs[2],&nrhs);
	      }
#else
	      printf(" *ERROR in nonlingeo: the PARDISO library is not linked\n\n");
	      FORTRAN(stop,());
#endif
	  }
	  
	  if(*mortar<=1){
	      if(isensitivity){
/*		  if(nasym!=0){
		      printf("*ERROR in nonlingeo: a sensitivity analysis \n is not allowed in combination with frictional contact \n\n");
		      FORTRAN(stop,());
		      }*/
		  SFREE(adcpy);NNEW(adcpy,double,neq[1]);
		  SFREE(aucpy);NNEW(aucpy,double,(nasym+1)*nzs[1]);
		  memcpy(&adcpy[0],&ad[0],sizeof(double)*neq[1]);
		  memcpy(&aucpy[0],&au[0],sizeof(double)*(nasym+1)*nzs[1]);
	      }
	      SFREE(ad);SFREE(au);
	  } 
      }
      
      /* explicit dynamic step */
      
      else{
	  if(*ithermal!=2){
	      for(k=0;k<neq[0];++k){
		  b[k]=b[k]/adb[k];
	      }
	  }
	  if(*ithermal>1){
	      for(k=neq[0];k<neq[1];++k){
		  b[k]=b[k]*dtime/adb[k];
	      }
	  }
      }

      /* calculating the displacements, stresses and forces */
      
      NNEW(v,double,mt**nk);
      memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
      
      NNEW(stx,double,6*mi[0]**ne);
      NNEW(fn,double,mt**nk);
      
      NNEW(inum,ITG,*nk);
      results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounact,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
	      &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
	      &icmd,ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
	      emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
	      iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
	      fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
	      &reltime,&ne0,thicke,shcon,nshcon,
	      sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
	      mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
              islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
              inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
      SFREE(inum);

      /* implicit dynamics (Matteo Pacher) */

      if((*ne!=ne0)&&(*nmethod==4)&&(*ithermal<2)&&(*iexpl<=1)){
         FORTRAN(storecontactprop,(ne,&ne0,lakon,kon,ipkon,mi,
                         ielmat,elcon,mortar,adblump,nactdof,springarea,
                         ncmat_,ntmat_,stx,&temax));
      }

      /* updating the external work (only for dynamic calculations) */

      if((*nmethod==4)&&(*ithermal<2)){
	  allwk=allwkini;
	  for(i=0;i<*nk;i++){
	      for(k=1;k<4;k++){
		  allwk+=(fnext[i*mt+k]+fnextini[i*mt+k])*
		      (v[i*mt+k]-vini[i*mt+k])/2.;
	      }
	  }

        /* Work due to damping forces (cv and cvini) --> MPADD */

	  if(idamping==1){
	      dampwk=dampwkini;
	      for(k=0;k<*nk;++k){
		  for(j=1;j<mt;++j){
		      if(nactdof[mt*k+j]>0){
			  aux2[nactdof[mt*k+j]-1]=v[mt*k+j]-vini[mt*k+j];
		      }
		  }
	      }
	      for(k=0;k<neq[0];k++){
		  dampwk+=-(cv[k]+cvini[k])*aux2[k]/2.;
	      }
	  }
        /* Damping forces --> MPADD */
      }

      /* line search (only for static surface-to-surface penalty contact)
         and not in the first iteration */

      if((*mortar==1)&&(iit!=1)&&(*ne-ne0>0)&&(*nmethod!=4)){

	  SFREE(v);SFREE(stx);SFREE(fn);
      
	  /* calculating the residual */
      
	  NNEW(res,double,neq[1]);
	  calcresidual(nmethod,neq,res,fext,f,iexpl,nactdof,aux2,vold,
	     vini,&dtime,accold,nk,adb,aub,jq,irow,nzl,alpha,fextini,fini,
	     islavnode,nslavnode,mortar,ntie,f_cm,f_cs,mi,
	     nzs,&nasym,&idamping,veold,adc,auc,cvini,cv);

          /* calculating the line search factor */

	  sum1=0.;sum2=0.;
	  for(i=0;i<neq[1];i++){
	      sum1+=b[i]*resold[i];
	      sum2+=b[i]*res[i];
	  }
	  SFREE(res);

	  if(fabs(sum1-sum2)<1.e-30){
	      flinesearch=1.;
	  }else{
	      flinesearch=sum1/(sum1-sum2);
	      if(flinesearch>smaxls){
		  flinesearch=smaxls;
	      }else if(flinesearch<sminls){
		  flinesearch=sminls;
	      }
	  }
	  printf("line search factor=%f\n\n",flinesearch);

          /* update the solution */

	  for(i=0;i<neq[1];i++){b[i]*=flinesearch;}
      
	  NNEW(v,double,mt**nk);
	  memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
	  
	  NNEW(stx,double,6*mi[0]**ne);
	  NNEW(fn,double,mt**nk);
	  
	  NNEW(inum,ITG,*nk);
	  results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounact,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
	      &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
	      &icmd,ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
	      emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
	      iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
	      fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
	      &reltime,&ne0,thicke,shcon,nshcon,
	      sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
	      mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
	      islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
	      inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
	  SFREE(inum);
      }
      
      /* calculating the residual */
      
      calcresidual(nmethod,neq,b,fext,f,iexpl,nactdof,aux2,vold,
	 vini,&dtime,accold,nk,adb,aub,jq,irow,nzl,alpha,fextini,fini,
	 islavnode,nslavnode,mortar,ntie,f_cm,f_cs,mi,
	 nzs,&nasym,&idamping,veold,adc,auc,cvini,cv);

      memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);
      if(*ithermal!=2){
	  for(k=0;k<6*mi[0]*ne0;++k){
	      sti[k]=stx[k];
	  }
      }
      
      /* calculating the ratio of the smallest to largest pressure
         for face-to-face contact
         only done at the end of a step */

      if((*mortar==1)&&(1.-theta-dtheta<=1.e-6)){
	  FORTRAN(negativepressure,(&ne0,ne,mi,stx,&pressureratio));
      }else{pressureratio=0.;}

      SFREE(v);SFREE(stx);SFREE(fn);

      if(idamping==1){SFREE(adc);SFREE(auc);}

      if(*iexpl<=1){
	  
	  /* store the residual forces for the next iteration */

	  if(*ithermal!=2){
	      if(cam[0]>uam[0]){uam[0]=cam[0];}      
	      if(qau<1.e-10){
		  if(qa[0]>ea*qam[0]){qam[0]=(qamold[0]*jnz+qa[0])/(jnz+1);}
		  else {qam[0]=qamold[0];}
	      }
	  }
	  if(*ithermal>1){
	      if(cam[1]>uam[1]){uam[1]=cam[1];}      
	      if(qau<1.e-10){
		  if(qa[1]>ea*qam[1]){qam[1]=(qamold[1]*jnz+qa[1])/(jnz+1);}
		  else {qam[1]=qamold[1];}
	      }
	  }
      
	  /* calculating the maximum residual */

	  for(k=0;k<2;++k){
	      ram2[k]=ram1[k];
	      ram1[k]=ram[k];
	      ram[k]=0.;
	  }
	  if(*ithermal!=2){
	      for(k=0;k<neq[0];++k){
		  err=fabs(b[k]);
		  if(err>ram[0]){ram[0]=err;ram[2]=k+0.5;}
	      }
	  }
	  if(*ithermal>1){
	      for(k=neq[0];k<neq[1];++k){
		  err=fabs(b[k]);
		  if(err>ram[1]){ram[1]=err;ram[3]=k+0.5;}
	      }
	  }
	  
	  /*   Divergence criteria for face-to-face penalty is different */
	  
	  if(*mortar==1){
	      for(k=4;k<6;++k){
		  ram2[k]=ram1[k];
		  ram1[k]=ram[k];
	      } 
	      ram[4]=ram[0]+ram1[0];
/*	      if((iflagact==0)&&(iit>1)){
		  ram[5]=1.5;
		  }else{ram[5]=0.5;}*/
	      ram[5]=(*ne-ne0)-(neold-ne0)+0.5;
/*	      if(iit>3){
		  if((fabs(ram[6])>=fabs(ram1[6]))&&(fabs(ram[6])>=fabs(ram2[6]))){
		      ram[7]=1.5;
		  }else{ram[7]=0.5;}
		  }*/
	      
	  }
	  
	  /* next line is inserted to cope with stress-less
	     temperature calculations */
	  
	  if(*ithermal!=2){
	      if(ram[0]<1.e-6) ram[0]=0.;      
	      printf(" average force= %f\n",qa[0]);
	      printf(" time avg. forc= %f\n",qam[0]);
	      if((ITG)((double)nactdofinv[(ITG)ram[2]]/mt)+1==0){
		  printf(" largest residual force= %f\n",
			 ram[0]);
	      }else{
		  inode=(ITG)((double)nactdofinv[(ITG)ram[2]]/mt)+1;
		  idir=nactdofinv[(ITG)ram[2]]-mt*(inode-1);
		  printf(" largest residual force= %f in node %" ITGFORMAT " and dof %" ITGFORMAT "\n",
			 ram[0],inode,idir);
	      }
	      printf(" largest increment of disp= %e\n",uam[0]);
	      if((ITG)cam[3]==0){
		  printf(" largest correction to disp= %e\n\n",
			 cam[0]);
	      }else{
		  inode=(ITG)((double)nactdofinv[(ITG)cam[3]]/mt)+1;
		  idir=nactdofinv[(ITG)cam[3]]-mt*(inode-1);
		  printf(" largest correction to disp= %e in node %" ITGFORMAT " and dof %" ITGFORMAT "\n\n",cam[0],inode,idir);
	      }
	  }
	  if(*ithermal>1){
	      if(ram[1]<1.e-6) ram[1]=0.;      
	      printf(" average flux= %f\n",qa[1]);
	      printf(" time avg. flux= %f\n",qam[1]);
	      if((ITG)((double)nactdofinv[(ITG)ram[3]]/mt)+1==0){
		  printf(" largest residual flux= %f\n",
			 ram[1]);
	      }else{
		  inode=(ITG)((double)nactdofinv[(ITG)ram[3]]/mt)+1;
		  idir=nactdofinv[(ITG)ram[3]]-mt*(inode-1);
		  printf(" largest residual flux= %f in node %" ITGFORMAT " and dof %" ITGFORMAT "\n",ram[1],inode,idir);
	      }
	      printf(" largest increment of temp= %e\n",uam[1]);
	      if((ITG)cam[4]==0){
		  printf(" largest correction to temp= %e\n\n",
			 cam[1]);
	      }else{
		  inode=(ITG)((double)nactdofinv[(ITG)cam[4]]/mt)+1;
		  idir=nactdofinv[(ITG)cam[4]]-mt*(inode-1);
		  printf(" largest correction to temp= %e in node %" ITGFORMAT " and dof %" ITGFORMAT "\n\n",cam[1],inode,idir);
	      }
	  }
	  fflush(stdout);
	  
	  FORTRAN(writecvg,(istep,&iinc,&icutb,&iit,ne,&ne0,ram,qam,cam,uam,
			ithermal));

	  checkconvergence(co,nk,kon,ipkon,lakon,ne,stn,nmethod, 
   	     kode,filab,een,t1act,&time,epn,ielmat,matname,enern, 
	     xstaten,nstate_,istep,&iinc,iperturb,ener,mi,output,
             ithermal,qfn,&mode,&noddiam,trab,inotr,ntrans,orab,
	     ielorien,norien,description,sti,&icutb,&iit,&dtime,qa,
	     vold,qam,ram1,ram2,ram,cam,uam,&ntg,ttime,&icntrl,
	     &theta,&dtheta,veold,vini,idrct,tper,&istab,tmax, 
	     nactdof,b,tmin,ctrl,amta,namta,itpamp,inext,&dthetaref,
             &itp,&jprint,jout,&uncoupled,t1,&iitterm,nelemload,
             nload,nodeboun,nboun,itg,ndirboun,&deltmx,&iflagact,
	     set,nset,istartset,iendset,ialset,emn,thicke,jobnamec,
	     mortar,nmat,ielprop,prop,&ialeatoric,&kscale,
             energy,&allwk,&energyref,&emax,&r_abs,&enetoll,energyini,
	     &allwkini,&temax,&sizemaxinc,&ne0,&neini,&dampwk,
	     &dampwkini,energystartstep);
	  
      }else{

          /* explicit dynamics */

	  icntrl=1;
	  icutb=0;   

          /* recalculation of the time increment every 500 icrements
             (may have changed due to deformation) */

	  if((iinc/500)*500==iinc){
	      FORTRAN(calcstabletimeincvol,(&ne0,lakon,co,kon,ipkon,mi,
			      ielmat,&dtvol,alpha,wavespeed));

	      if(dtvol<*tinc)*tinc=dtvol;
	      dtheta=(*tinc)/(*tper);
	      dthetaref=dtheta;
	  }

	  theta=theta+dtheta;  
	  if(dtheta>=1.-theta){
	      if(dtheta>1.-theta){
		  printf(" the increment size exceeds the remainder of the step and is decreased to %e\n\n",
		      dtheta**tper);
	      }
	      dtheta=1.-theta;
	      dthetaref=dtheta;
	  }
	  iflagact=0;
      }
      
    }

    if(*nmethod!=4)SFREE(resold);

    /*********************************************************/
    /*   end of the iteration loop                          */
    /*********************************************************/

    /* icutb=0 means that the iterations in the increment converged,
       icutb!=0 indicates that the increment has to be reiterated with
                another increment size (dtheta) */

    /* Adapter: Perform coupling related actions, only if solver iterations converged (icutb == 0) */
    if( icutb == 0 )
    {
        /* Adapter: Write coupling data */

    	NNEW(v,double,mt**nk);
    	NNEW(fn,double,mt**nk);
    	NNEW(stn,double,6**nk);
	    NNEW(stx,double,6*mi[0]**ne);
    	NNEW(inum,ITG,*nk);
  
    	memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
    	
		// iout=-1 means that the displacements and temperatures are assumed to be known 			and used to calculate strains, stresses...., with no result output
		iout=-1;
    	icmd=3;// calculate only stress (not stiffness)

      results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounact,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
	      &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,
	      &icmd,ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,
	      emeini,xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,
	      iendset,ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,
	      fmpc,nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
	      &reltime,&ne0,thicke,shcon,nshcon,
	      sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
	      mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
              islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
              inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);

	    simulationData.fn = fn;
        memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);

        Precice_WriteCouplingData( &simulationData );
        /* Adapter: Advance the coupling */
			/* gkdas2: add displacement to coordinates*/
		//printf("UPDATING COORDINATES\n");
		//NLupdateCO(coUpdated, v, *nk, mt);


        Precice_Advance( &simulationData );

        /* Adapter: If the coupling does not converge, read the checkpoint */
        if( Precice_IsReadCheckpointRequired() )
        {

			printf("READING ITERATION CHECKPOINT");
            if( *nmethod == 4 )
            {
                Precice_ReadIterationCheckpoint( &simulationData, vold );
                icutb++;
            }
            Precice_FulfilledReadCheckpoint();
        }

	

		SFREE(v);SFREE(stn);SFREE(stx);SFREE(fn);SFREE(inum);
    }

	

	printf("AERO-ELASTIC CONVERGED! \n");

	printf("UPDATING COORDINATES\n");
	NLupdateCO(coUpdated, vold, *nk, mt);
   
    /* printing the energies (only for dynamic calculations) */

    if((icutb==0)&&(*nmethod==4)&&(*ithermal<2)){
	printf(" initial energy (at start of step) = %e\n\n",energyref);

	printf(" since start of the step: \n");
	printf(" external work = %e\n",allwk);
	printf(" work performed by the damping forces = %e\n",dampwk);
	printf(" netto work = %e\n\n",allwk+dampwk);

        printf(" actual energy: \n");
	printf(" internal energy = %e\n",energy[0]);
	printf(" kinetic energy = %e\n",energy[1]);
	printf(" elastic contact energy = %e\n",energy[2]);
	printf(" energy lost due to friction = %e\n",energy[3]);
	printf(" total energy  = %e\n\n",energy[0]+energy[1]+energy[2]+energy[3]);

	printf(" energy increase = %e\n\n",energy[0]+energy[1]+energy[2]+energy[3]-energyref);

	printf(" energy balance (absolute) = %e \n",energy[0]+energy[1]+energy[2]+energy[3]-energyref-allwk-dampwk);

	/* Belytschko criterion */

	denergymax=energy[0];
	if(denergymax<energy[1]) denergymax=energy[1];
	if(denergymax<fabs(allwk)) denergymax=fabs(allwk);

	if(denergymax>1.e-30){
	    printf(" energy balance (relative) = %f %% \n\n",
            fabs((energy[0]+energy[1]+energy[2]+energy[3]-energyref-allwk-dampwk)/
            denergymax*100.));
	}else{
	    printf(" energy balance (relative) =0 %% \n\n");
	}
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
//    MPADD start
//	printf(" work done by the damping forces = %e\n", dampwk);
//	neini=*ne; 
//	printf(" contact elements end of increment = %"ITGFORMAT"\n\n", *ne - ne0);
//    MPADD end
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

    }

    if(uncoupled){
	SFREE(iruc);
    }

    if(((qa[0]>ea*qam[0])||(qa[1]>ea*qam[1]))&&(icutb==0)){jnz++;}
    iit=0;

    if(icutb!=0){
      memcpy(&vold[0],&vini[0],sizeof(double)*mt**nk);

      for(k=0;k<*nboun;++k){xbounact[k]=xbounini[k];}
      if((*ithermal==1)||(*ithermal>=3)){
	for(k=0;k<*nk;++k){t1act[k]=t1ini[k];}
      }
      for(k=0;k<neq[1];++k){
	  f[k]=fini[k];
      }
      if(*nmethod==4){
	for(k=0;k<mt**nk;++k){
	  veold[k]=veini[k];
	  accold[k]=accini[k];
	}
	for(k=0;k<neq[1];++k){
//	  f[k]=fini[k];
	  fext[k]=fextini[k];
	  cv[k]=cvini[k];
	}
      }
      if(*ithermal!=2){
	  for(k=0;k<6*mi[0]*ne0;++k){
	      sti[k]=stiini[k];
	      eme[k]=emeini[k];
	  }
      }
      if(*nener==1)
	  for(k=0;k<mi[0]*ne0;++k){ener[k]=enerini[k];}

      for(k=0;k<*nstate_*mi[0]*(ne0+maxprevcontel);++k){
	  xstate[k]=xstateini[k];
      }	  

      qam[0]=qamold[0];
      qam[1]=qamold[1];
    }
    
    /* face-to-face penalty */

    if((*mortar==1)&&(icutb==0)){
	
	ntrimax=0;
	for(i=0;i<*ntie;i++){	    
	    if(itietri[2*i+1]-itietri[2*i]+1>ntrimax)		
		ntrimax=itietri[2*i+1]-itietri[2*i]+1;  	
	}
	NNEW(xo,double,ntrimax);	    
	NNEW(yo,double,ntrimax);	    
	NNEW(zo,double,ntrimax);	    
	NNEW(x,double,ntrimax);	    
	NNEW(y,double,ntrimax);	    
	NNEW(z,double,ntrimax);	   
	NNEW(nx,ITG,ntrimax);	   
	NNEW(ny,ITG,ntrimax);	    
	NNEW(nz,ITG,ntrimax);
      
	/*  Determination of active nodes (islavact) */
      
	FORTRAN(islavactive,(tieset,ntie,itietri,cg,straight,
		   co,vold,xo,yo,zo,x,y,z,nx,ny,nz,mi,
		   imastop,nslavnode,islavnode,islavact));

	SFREE(xo);SFREE(yo);SFREE(zo);SFREE(x);SFREE(y);SFREE(z);SFREE(nx);	    
	SFREE(ny);SFREE(nz);

	if(negpres==0){
	    if((*mortar==1)&&(1.-theta-dtheta<=1.e-6)&&(itruecontact==1)){
		printf(" pressure ratio (smallest/largest pressure over all contact areas) =%e\n\n",pressureratio);
		if(pressureratio<-0.05){
		    printf(" zero-size increment is appended\n\n");
		    negpres=1;theta=1.-1.e-6;dtheta=1.e-6;
		}
	    }
	}else{negpres=0;}

    }

    /* output */

    if((jout[0]==jprint)&&(icutb==0)){

      jprint=0;

      /* calculating the displacements and the stresses and storing */
      /* the results in frd format  */
	
      NNEW(v,double,mt**nk);
      NNEW(fn,double,mt**nk);
      NNEW(stn,double,6**nk);
      if(*ithermal>1) NNEW(qfn,double,3**nk);
      NNEW(inum,ITG,*nk);
      NNEW(stx,double,6*mi[0]**ne);
      
      if(strcmp1(&filab[261],"E   ")==0) NNEW(een,double,6**nk);
      if(strcmp1(&filab[435],"PEEQ")==0) NNEW(epn,double,*nk);
      if(strcmp1(&filab[522],"ENER")==0) NNEW(enern,double,*nk);
      if(strcmp1(&filab[609],"SDV ")==0) NNEW(xstaten,double,*nstate_**nk);
      if(strcmp1(&filab[2175],"CONT")==0) NNEW(cdn,double,6**nk);
      if(strcmp1(&filab[2697],"ME  ")==0) NNEW(emn,double,6**nk);

      memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);

      iout=2;
      icmd=3;
      
#ifdef COMPANY
      FORTRAN(uinit,());
#endif
      results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	      elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	      ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	      prestr,iprestr,filab,eme,emn,een,iperturb,
	      f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	      ndirboun,xbounact,nboun,ipompc,
	      nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
              &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	      xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,&icmd,
	      ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,emeini,
              xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,iendset,
              ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,fmpc,
	      nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
              &reltime,&ne0,thicke,shcon,nshcon,
              sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
              mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
	      islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
              inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
      
      memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);

      iout=0;
      if(*iexpl<=1) icmd=0;
//      FORTRAN(networkinum,(ipkon,inum,kon,lakon,ne,itg,&ntg));
//      for(k=0;k<ntg;k++)if(inum[itg[k]-1]>0){inum[itg[k]-1]*=-1;}
      
      ++*kode;
      if(*mcs!=0){
	ptime=*ttime+time;
	frdcyc(co,nk,kon,ipkon,lakon,ne,v,stn,inum,nmethod,kode,filab,een,
	       t1act,fn,&ptime,epn,ielmat,matname,cs,mcs,nkon,enern,xstaten,
               nstate_,istep,&iinc,iperturb,ener,mi,output,ithermal,qfn,
               ialset,istartset,iendset,trab,inotr,ntrans,orab,ielorien,
	       norien,stx,veold,&noddiam,set,nset,emn,thicke,jobnamec,&ne0,
               cdn,mortar,nmat,qfx,ielprop,prop);
#ifdef COMPANY
	FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif
      }
      else{
	  if(strcmp1(&filab[1044],"ZZS")==0){
	      NNEW(neigh,ITG,40**ne);
	      NNEW(ipneigh,ITG,*nk);
	  }

	  ptime=*ttime+time;
	  frd(co,nk,kon,ipkon,lakon,&ne0,v,stn,inum,nmethod,
	    kode,filab,een,t1act,fn,&ptime,epn,ielmat,matname,enern,xstaten,
	    nstate_,istep,&iinc,ithermal,qfn,&mode,&noddiam,trab,inotr,
	    ntrans,orab,ielorien,norien,description,ipneigh,neigh,
	    mi,stx,vr,vi,stnr,stni,vmax,stnmax,&ngraph,veold,ener,ne,
	    cs,set,nset,istartset,iendset,ialset,eenmax,fnr,fni,emn,
	    thicke,jobnamec,output,qfx,cdn,mortar,cdnr,cdni,nmat,ielprop,prop);

	  if(strcmp1(&filab[1044],"ZZS")==0){SFREE(ipneigh);SFREE(neigh);}
#ifdef COMPANY
	  FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif
      }


      
      SFREE(v);SFREE(fn);SFREE(stn);SFREE(inum);SFREE(stx);
      if(*ithermal>1){SFREE(qfn);}
      
      if(strcmp1(&filab[261],"E   ")==0) SFREE(een);
      if(strcmp1(&filab[435],"PEEQ")==0) SFREE(epn);
      if(strcmp1(&filab[522],"ENER")==0) SFREE(enern);
      if(strcmp1(&filab[609],"SDV ")==0) SFREE(xstaten);
      if(strcmp1(&filab[2175],"CONT")==0) SFREE(cdn);
      if(strcmp1(&filab[2697],"ME  ")==0) SFREE(emn);
   }
    
  }

  /*********************************************************/
  /*   end of the increment loop                          */
  /*********************************************************/

  if(jprint!=0){

  /* calculating the displacements and the stresses and storing  
     the results in frd format */
  
    NNEW(v,double,mt**nk);
    NNEW(fn,double,mt**nk);
    NNEW(stn,double,6**nk);
    if(*ithermal>1) NNEW(qfn,double,3**nk);
    NNEW(inum,ITG,*nk);
    NNEW(stx,double,6*mi[0]**ne);
  
    if(strcmp1(&filab[261],"E   ")==0) NNEW(een,double,6**nk);
    if(strcmp1(&filab[435],"PEEQ")==0) NNEW(epn,double,*nk);
    if(strcmp1(&filab[522],"ENER")==0) NNEW(enern,double,*nk);
    if(strcmp1(&filab[609],"SDV ")==0) NNEW(xstaten,double,*nstate_**nk);
    if(strcmp1(&filab[2175],"CONT")==0) NNEW(cdn,double,6**nk);
    if(strcmp1(&filab[2697],"ME  ")==0) NNEW(emn,double,6**nk);
    
    memcpy(&v[0],&vold[0],sizeof(double)*mt**nk);
    iout=2;
    icmd=3;

#ifdef COMPANY
    FORTRAN(uinit,());
#endif
    results(co,nk,kon,ipkon,lakon,ne,v,stn,inum,stx,
	    elcon,nelcon,rhcon,nrhcon,alcon,nalcon,alzero,ielmat,
	    ielorien,norien,orab,ntmat_,t0,t1act,ithermal,
	    prestr,iprestr,filab,eme,emn,een,iperturb,
	    f,fn,nactdof,&iout,qa,vold,b,nodeboun,
	    ndirboun,xbounact,nboun,ipompc,
	    nodempc,coefmpc,labmpc,nmpc,nmethod,cam,&neq[1],veold,accold,
            &bet,&gam,&dtime,&time,ttime,plicon,nplicon,plkcon,nplkcon,
	    xstateini,xstiff,xstate,npmat_,epn,matname,mi,&ielas,&icmd,
            ncmat_,nstate_,stiini,vini,ikboun,ilboun,ener,enern,emeini,
            xstaten,eei,enerini,cocon,ncocon,set,nset,istartset,iendset,
            ialset,nprint,prlab,prset,qfx,qfn,trab,inotr,ntrans,fmpc,
	    nelemload,nload,ikmpc,ilmpc,istep,&iinc,springarea,
            &reltime,&ne0,thicke,shcon,nshcon,
            sideload,xloadact,xloadold,&icfd,inomat,pslavsurf,pmastsurf,
            mortar,islavact,cdn,islavnode,nslavnode,ntie,clearini,
	    islavsurf,ielprop,prop,energyini,energy,&kscale,iponoel,
            inoel,nener,orname,network,ipobody,xbodyact,ibody,typeboun);
    
    memcpy(&vold[0],&v[0],sizeof(double)*mt**nk);

    iout=0;
    if(*iexpl<=1) icmd=0;
//    FORTRAN(networkinum,(ipkon,inum,kon,lakon,ne,itg,&ntg));
//    for(k=0;k<ntg;k++)if(inum[itg[k]-1]>0){inum[itg[k]-1]*=-1;}
    
    ++*kode;
    if(*mcs>0){
	ptime=*ttime+time;
      frdcyc(co,nk,kon,ipkon,lakon,ne,v,stn,inum,nmethod,kode,filab,een,
	     t1act,fn,&ptime,epn,ielmat,matname,cs,mcs,nkon,enern,xstaten,
             nstate_,istep,&iinc,iperturb,ener,mi,output,ithermal,qfn,
             ialset,istartset,iendset,trab,inotr,ntrans,orab,ielorien,
	     norien,stx,veold,&noddiam,set,nset,emn,thicke,jobnamec,&ne0,
             cdn,mortar,nmat,qfx,ielprop,prop);
#ifdef COMPANY
      FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif

    }
    else{
	if(strcmp1(&filab[1044],"ZZS")==0){
	    NNEW(neigh,ITG,40**ne);
	    NNEW(ipneigh,ITG,*nk);
	}

	ptime=*ttime+time;
	frd(co,nk,kon,ipkon,lakon,&ne0,v,stn,inum,nmethod,
	    kode,filab,een,t1act,fn,&ptime,epn,ielmat,matname,enern,xstaten,
	    nstate_,istep,&iinc,ithermal,qfn,&mode,&noddiam,trab,inotr,
	    ntrans,orab,ielorien,norien,description,ipneigh,neigh,
	    mi,stx,vr,vi,stnr,stni,vmax,stnmax,&ngraph,veold,ener,ne,
	    cs,set,nset,istartset,iendset,ialset,eenmax,fnr,fni,emn,
	    thicke,jobnamec,output,qfx,cdn,mortar,cdnr,cdni,nmat,ielprop,prop);

	if(strcmp1(&filab[1044],"ZZS")==0){SFREE(ipneigh);SFREE(neigh);}
#ifdef COMPANY
	FORTRAN(uout,(v,mi,ithermal,filab,kode));
#endif
    }

    SFREE(v);SFREE(fn);SFREE(stn);SFREE(inum);SFREE(stx);
    if(*ithermal>1){SFREE(qfn);}
    
    if(strcmp1(&filab[261],"E   ")==0) SFREE(een);
    if(strcmp1(&filab[435],"PEEQ")==0) SFREE(epn);
    if(strcmp1(&filab[522],"ENER")==0) SFREE(enern);
    if(strcmp1(&filab[609],"SDV ")==0) SFREE(xstaten);
    if(strcmp1(&filab[2175],"CONT")==0) SFREE(cdn);
    if(strcmp1(&filab[2697],"ME  ")==0) SFREE(emn);

  }
    
  /* writing out the latest stiffness matrix for a subsequent
     sensitivity analysis */

  if(isensitivity){
      
      strcpy(stiffmatrix,jobnamec);
      strcat(stiffmatrix,".stm");
      
      if((f1=fopen(stiffmatrix,"wb"))==NULL){
	  printf("*ERROR in linstatic: cannot open stiffness matrix file for writing...");
	  exit(0);
      }
      
      /* storing the stiffness matrix */

      /* nzs,irow,jq and icol have to be stored too, since the static analysis
	 can involve contact, whereas in the sensitivity analysis contact is not
	 taken into account while determining the structure of the stiffness
	 matrix (in mastruct.c)
      */
      
      if(fwrite(&nasym,sizeof(ITG),1,f1)!=1){
	  printf("*ERROR saving the symmetry flag to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(nzs,sizeof(ITG),3,f1)!=3){
	  printf("*ERROR saving the number of subdiagonal nonzeros to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(irow,sizeof(ITG),nzs[2],f1)!=nzs[2]){
	  printf("*ERROR saving irow to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(jq,sizeof(ITG),neq[1]+1,f1)!=neq[1]+1){
	  printf("*ERROR saving jq to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(icol,sizeof(ITG),neq[1],f1)!=neq[1]){
	  printf("*ERROR saving icol to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(adcpy,sizeof(double),neq[1],f1)!=neq[1]){
	  printf("*ERROR saving the diagonal of the stiffness matrix to the stiffness matrix file...");
	  exit(0);
      }
      if(fwrite(aucpy,sizeof(double),(nasym+1)*nzs[2],f1)!=(nasym+1)*nzs[2]){
	  printf("*ERROR saving the off-diagonal terms of the stiffness matrix to the stiffness matrix file...");
	  exit(0);
      }
      fclose(f1);
      SFREE(adcpy);SFREE(aucpy);
  }
  
  /* restoring the distributed loading  */

  if((*ithermal==3)&&(ncont!=0)&&(*mortar==1)&&(*ncmat_>=11)){
      *nload=nloadref;
      RENEW(nelemload,ITG,2**nload);memcpy(&nelemload[0],&nelemloadref[0],sizeof(ITG)*2**nload);
      if(*nam>0){
	  RENEW(iamload,ITG,2**nload);
	  memcpy(&iamload[0],&iamloadref[0],sizeof(ITG)*2**nload);
      }
      RENEW(sideload,char,20**nload);memcpy(&sideload[0],&sideloadref[0],sizeof(char)*20**nload);
      
      /* freeing the temporary fields */
      
      SFREE(nelemloadref);if(*nam>0){SFREE(iamloadref);};
      SFREE(sideloadref);
  }

  /* setting the velocity to zero at the end of a quasistatic or stationary
     step */

  if(abs(*nmethod)==1){
    for(k=0;k<mt**nk;++k){veold[k]=0.;}
  }

  /* updating the loading at the end of the step; 
     important in case the amplitude at the end of the step
     is not equal to one */

  for(k=0;k<*nboun;++k){

      /* thermal boundary conditions are updated only if the
         step was thermal or thermomechanical */

      if(ndirboun[k]==0){
	  if(*ithermal<2) continue;

	  /* mechanical boundary conditions are updated only
             if the step was not thermal or the node is a
             network node */

      }else if((ndirboun[k]>0)&&(ndirboun[k]<4)){
	  node=nodeboun[k];
	  FORTRAN(nident,(itg,&node,&ntg,&id));
	  networknode=0;
	  if(id>0){
	      if(itg[id-1]==node) networknode=1;
	  }
	  if((*ithermal==2)&&(networknode==0)) continue;
      }
      xbounold[k]=xbounact[k];
  }
  for(k=0;k<*nforc;++k){xforcold[k]=xforcact[k];}
  for(k=0;k<2**nload;++k){xloadold[k]=xloadact[k];}
  for(k=0;k<7**nbody;k=k+7){xbodyold[k]=xbodyact[k];}
  if(*ithermal==1){
    for(k=0;k<*nk;++k){t1old[k]=t1act[k];}
    for(k=0;k<*nk;++k){vold[mt*k]=t1act[k];}
  }
  else if(*ithermal>1){
    for(k=0;k<*nk;++k){t1[k]=vold[mt*k];}
    if(*ithermal>=3){
	for(k=0;k<*nk;++k){t1old[k]=t1act[k];}
    }
  }

  qaold[0]=qa[0];
  qaold[1]=qa[1];

  SFREE(f);SFREE(b);
  SFREE(xbounact);SFREE(xforcact);SFREE(xloadact);SFREE(xbodyact);
  if(*nbody>0) SFREE(ipobody);if(inewton==1){SFREE(cgr);}
  SFREE(fext);SFREE(ampli);SFREE(xbounini);SFREE(xstiff);
  if((*ithermal==1)||(*ithermal>=3)){SFREE(t1act);SFREE(t1ini);}

  if(*ithermal>1){
      SFREE(itg);SFREE(ieg);SFREE(kontri);SFREE(nloadtr);
      SFREE(nactdog);SFREE(nacteq);SFREE(ineighe);
      SFREE(tarea);SFREE(tenv);SFREE(fenv);SFREE(qfx);
      SFREE(erad);SFREE(ac);SFREE(bc);SFREE(ipiv);
      SFREE(bcr);SFREE(ipivr);SFREE(adview);SFREE(auview);SFREE(adrad);
      SFREE(aurad);SFREE(irowrad);SFREE(jqrad);SFREE(icolrad);
      if((*mcs>0)&&(ntr>0)){SFREE(inocs);}
      if((*network>0)||(ntg>0)){SFREE(iponoel);SFREE(inoel);}
  }

  if(icfd==1){
      SFREE(neifa);SFREE(neiel);SFREE(neij);SFREE(ielfa);SFREE(ifaext);
      SFREE(vfa);SFREE(nactdoh);SFREE(nactdohinv);SFREE(konf);
      SFREE(ipkonf);SFREE(lakonf);SFREE(ielmatf);free(ifatie);
      SFREE(ipnei);SFREE(isolidsurf);
      if(*norien>0) SFREE(ielorienf);
/*      if(nblk!=0){SFREE(istartblk);SFREE(iendblk);
	SFREE(nblket);SFREE(nblkze);SFREE(ielblk);}*/
  }

  SFREE(fini);
  if(*nmethod==4){
    SFREE(aux2);SFREE(fextini);SFREE(veini);SFREE(accini);
    SFREE(adb);SFREE(aub);SFREE(cvini);SFREE(cv);SFREE(fnext);
    SFREE(fnextini);
  }
  SFREE(eei);SFREE(stiini);SFREE(emeini);
  if(*nener==1)SFREE(enerini);
  if(*nstate_!=0){SFREE(xstateini);}

  SFREE(aux);SFREE(iaux);SFREE(vini);

  if(icascade==2){
      memmpc_=memmpcref_;mpcfree=mpcfreeref;maxlenmpc=maxlenmpcref;
      RENEW(nodempc,ITG,3*memmpcref_);
      for(k=0;k<3*memmpcref_;k++){nodempc[k]=nodempcref[k];}
      RENEW(coefmpc,double,memmpcref_);
      for(k=0;k<memmpcref_;k++){coefmpc[k]=coefmpcref[k];}
      SFREE(nodempcref);SFREE(coefmpcref);
  }

  if(ncont!=0){
      *ne=ne0;*nkon=nkon0;
      if(*nener==1){
	RENEW(ener,double,mi[0]**ne*2);
      }
      RENEW(ipkon,ITG,*ne);
      RENEW(lakon,char,8**ne);
      RENEW(kon,ITG,*nkon);
      if(*norien>0){
	  RENEW(ielorien,ITG,mi[2]**ne);
      }
      RENEW(ielmat,ITG,mi[2]**ne);

      SFREE(cg);SFREE(straight);
      SFREE(imastop);SFREE(itiefac);SFREE(islavnode);
      SFREE(nslavnode);SFREE(iponoels);SFREE(inoels);SFREE(imastnode);
      SFREE(nmastnode);SFREE(itietri);SFREE(koncont);SFREE(xnoels);
      SFREE(springarea);SFREE(xmastnor);

      if(*mortar==0){
	  SFREE(areaslav);
      }else if(*mortar==1){
	  SFREE(pmastsurf);SFREE(ipe);SFREE(ime);
          SFREE(islavact);
      }
  }

  /* reset icascade */

  if(icascade==1){icascade=0;}

  mpcinfo[0]=memmpc_;mpcinfo[1]=mpcfree;mpcinfo[2]=icascade;
  mpcinfo[3]=maxlenmpc;

  if(iglob==1){SFREE(integerglob);SFREE(doubleglob);}

  *icolp=icol;*irowp=irow;*cop=co;*voldp=vold;

  *ipompcp=ipompc;*labmpcp=labmpc;*ikmpcp=ikmpc;*ilmpcp=ilmpc;
  *fmpcp=fmpc;*nodempcp=nodempc;*coefmpcp=coefmpc;*nelemloadp=nelemload;
  *iamloadp=iamload;*sideloadp=sideload;

  *ipkonp=ipkon;*lakonp=lakon;*konp=kon;*ielorienp=ielorien;
  *ielmatp=ielmat;*enerp=ener;*xstatep=xstate;

  *islavsurfp=islavsurf;*pslavsurfp=pslavsurf;*clearinip=clearini;

  (*tmin)*=(*tper);
  (*tmax)*=(*tper);

  SFREE(nactdofinv);
  // MPADD start
  if((*nmethod==4)&&(*ithermal!=2)&&(*iexpl<=1)&&(icfd!=1)){ SFREE(adblump);}
  // MPADD end
  
  (*ttime)+=(*tper);
  
  /* Adapter: Free the memory */
  Precice_FreeData( &simulationData );

/* gkdas2: write final updated coordinates and free memory */
NLwriteUpdatedCO(coUpdated, *nk, mt); 
SFREE(coUpdated);	


  return;
}