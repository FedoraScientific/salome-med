//  MED MedMem : MED idl descriptions implementation based on the classes of MEDMEM
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : convert.hxx
//  Author : EDF
//  Module : MED
//  $Header: /export/home/CVS/SALOME_ROOT/MED/src/MedMem/convert.hxx

# if ! defined ( __CONVERT_H__ )
# define __CONVERT_H__

#include <SALOMEconfig.h> 

#include CORBA_SERVER_HEADER(MED)

#include "MEDMEM_define.hxx"

using namespace MED_EN;

#include "MEDMEM_GenDriver.hxx"

SALOME_MED::medGeometryElement convertMedEltToIdlElt(medGeometryElement element) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medEntityMesh      convertMedEntToIdlEnt(medEntityMesh entity) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medModeSwitch      convertMedModeToIdlMode(medModeSwitch mode) 
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medDriverTypes     convertMedDriverToIdlDriver(driverTypes driverType)
			       throw (SALOME::SALOME_Exception);
SALOME_MED::medConnectivity    convertMedConnToIdlConn(medConnectivity connectivite)
			       throw (SALOME::SALOME_Exception);

medGeometryElement convertIdlEltToMedElt (SALOME_MED::medGeometryElement element) 
		      throw (SALOME::SALOME_Exception);
medEntityMesh   convertIdlEntToMedEnt(SALOME_MED::medEntityMesh entity) 
		      throw (SALOME::SALOME_Exception);
medModeSwitch       convertIdlModeToMedMode(SALOME_MED::medModeSwitch mode) 
		      throw (SALOME::SALOME_Exception);
driverTypes 	      convertIdlDriverToMedDriver(SALOME_MED::medDriverTypes driverType)
		      throw (SALOME::SALOME_Exception);
medConnectivity      convertIdlConnToMedConn(SALOME_MED::medConnectivity connectivite)
		      throw (SALOME::SALOME_Exception);


bool verifieParam (SALOME_MED::medEntityMesh entity, SALOME_MED::medGeometryElement geomElement)
     throw (SALOME::SALOME_Exception);

# endif 	/* # if ! defined ( __CONVERT_H__ ) */
