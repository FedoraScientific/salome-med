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
//  File   : readCoordinate.cxx
//  Module : MED

using namespace std;
#include<string>
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
using namespace MEDMEM;

void usage(char * name)
{
  cout << "    " << name <<" <file name>"<< " <mesh name> " << " <interlace mode>" << endl;
  cout << "    " << "displays all Nodes Coordinates in mdump mode" << endl;
  cout << endl;
  cout << "    " << "mesh name is mandatory. Try mdump if necessary" << endl;
  cout << "    " << "values for interlace mode are  : " << endl;
  cout << "    " << "MED_FULL_INTERLACE (default value) or MED_NO_INTERLACE" << endl;
  exit(-1);
}

int main (int argc, char ** argv) {
  
  if (argc < 3) usage(argv[0]);
  if (argc > 4) usage(argv[0]);

  string fileName = argv[1];
  string meshName = argv[2];

  medModeSwitch Mode = MED_FULL_INTERLACE;
  if (argc==4)
  {
        string comp=argv[3];
  	if ( comp == "MED_NO_INTERLACE" ) Mode = MED_NO_INTERLACE;
	else if ( comp != "MED_FULL_INTERLACE") usage(argv[0]);
  }


  MESH * myMesh= new MESH() ;
  myMesh->setName(meshName);
  MED_MESH_RDONLY_DRIVER myMeshDriver(fileName,myMesh);
  try
  {
       myMeshDriver.setMeshName(meshName);
       myMeshDriver.open();
  }
  catch (const exception & ex)
  {
         MESSAGE("Catch Exception : ");
         SCRUTE(ex.what());
  };
  MESSAGE("Open done");

  try
  {
       myMeshDriver.read();
  }
  catch (const exception & ex)
  {
         MESSAGE("Catch Exception : ");
         SCRUTE(ex.what());
  };
  MESSAGE("Read done");
  
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int MeshDimension  = myMesh->getMeshDimension() ;
  int NumberOfNodes  = myMesh->getNumberOfNodes() ;

  cout << "(************************)"<<endl;
  cout << "(* NOEUDS DU MAILLAGE : *)"<<endl;
  cout << "(************************)"<<endl;

  string typeRepere  = myMesh->getCoordinatesSystem();
  if ( typeRepere.size() == 0 ) typeRepere ="0";
  cout << "- Type de repere des coordonnees : " << typeRepere << endl;

  string chainevide(MED_TAILLE_PNOM+2,' ');
  cout << "- Nom des coordonnees : " << endl ;
  const string * CoordinatesNames = myMesh->getCoordinatesNames() ;
  for (int i=0; i < SpaceDimension ; i++) 
  {
	string bonnelongueur=chainevide;
	bonnelongueur.replace(1,CoordinatesNames[i].size(),CoordinatesNames[i]);
  	cout << bonnelongueur;
  }
  cout <<endl;

  cout << "- Unites des coordonnees : " << endl ;
  const string * CoordinatesUnits = myMesh->getCoordinatesUnits() ;
  for ( int i=0; i < SpaceDimension ; i++) 
  {
	string bonnelongueur=chainevide;
	bonnelongueur.replace(1,CoordinatesUnits[i].size(),CoordinatesUnits[i]);
  	cout << bonnelongueur;
  }
  cout <<endl;

  const double * Coordinates = myMesh->getCoordinates(Mode) ;
  cout << "- Coordonnees des noeuds : " << endl;
  for (int i=0; i < SpaceDimension*NumberOfNodes; i++)
  {
	fprintf(stdout," %f ",Coordinates[i]);
  }
  cout <<endl;

  delete myMesh;

}