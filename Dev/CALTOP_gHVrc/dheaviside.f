!
!     CalculiX - A 3-dimensional finite element program
!              Copyright (C) 1998-2018 Guido Dhondt
!
!     This program is free software; you can redistribute it and/or
!     modify it under the terms of the GNU General Public License as
!     published by the Free Software Foundation(version 2);
!
!
!     This program is distributed in the hope that it will be useful,
!     but WITHOUT ANY WARRANTY; without even the implied warranty of
!     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
!     GNU General Public License for more details.
!
!     You should have received a copy of the GNU General Public License
!     along with this program; if not, write to the Free Software
!     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
!
      subroutine dheaviside(ne,nea, neb, designFiltered,
     &  designFiltered2, dHSVector)
!
!     defiltering with Heaviside projection
!
         implicit none
!
c      integer 
!
!
         integer ne,
     &  nea,neb,i,j
!
         real*8 designFiltered(*),designFiltered2(*),dHSVector(*) 
    
!
         intent(in) ne,nea,neb, dHSVector,designFiltered   
         intent(inout) designFiltered2
   

!      loop over all elements
!         
         do i=nea,neb
           designFiltered2(i)= designFiltered(i)*dHSVector(i)    
         enddo
!
         return
         end
   