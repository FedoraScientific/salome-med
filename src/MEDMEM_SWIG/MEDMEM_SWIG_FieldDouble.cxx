//  MED MEDMEM_SWIG : binding of C++ implementation and Python
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
//  File   : MEDMEM_SWIG_FieldDouble.cxx
//  Module : MED

using namespace std;
#include "MEDMEM_SWIG_FieldDouble.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE() : FIELD<double>()
{
  BEGIN_OF("Default Constructor (for Python API) FIELDDOUBLE");

  END_OF("Default Constructor (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Constructor with arguments
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const SUPPORT * Support, const int NumberOfComponents) : FIELD<double>(Support,NumberOfComponents)
{
  BEGIN_OF("Constructor with arguments (for Python API) FIELDDOUBLE");

  med_type_champ type = MED_REEL64;

  setValueType(type);

  END_OF("Constructor with arguments (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Another constructor with arguments
 */
//=============================================================================
FIELDDOUBLE::FIELDDOUBLE(const SUPPORT * Support, driverTypes driverType,
			 const string & fileName,
			 const string & fieldName) :
  FIELD<double>(Support, driverType, fileName, fieldName)
{
  BEGIN_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");

  END_OF("Another constructor with arguments (for Python API) FIELDDOUBLE");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
FIELDDOUBLE::~FIELDDOUBLE()
{
  BEGIN_OF("Default Destructor (for Python API) FIELDDOUBLE");

  END_OF("Default Destructor (for Python API) FIELDDOUBLE");
}
