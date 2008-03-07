
#include <vector>
#include <string>
#include <map>
#include <set>

#include <iostream>
#include <fstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <sys/time.h>
//Debug macros
#include "MEDMEM_Utilities.hxx"

//MEDMEM includes
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"

//MEDSPLITTER includes
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParallelTopology.hxx"
#include "MEDSPLITTER_SequentialTopology.hxx"
#include "MEDSPLITTER_MESHCollectionDriver.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_MESHCollectionMedAsciiDriver.hxx"

using namespace MEDSPLITTER;

//template inclusion
#include "MEDSPLITTER_MESHCollectionMedAsciiDriver.H"


MESHCollectionMedAsciiDriver::MESHCollectionMedAsciiDriver(MESHCollection* collection):MESHCollectionDriver(collection)
{
}

/*!reads a MED File v>=2.3
 * and mounts the corresponding meshes in memory
 * the connect zones are created from the joints
 * 
 *\param filename ascii file containing the list of MED v2.3 files
 * */
 
int MESHCollectionMedAsciiDriver::read(char* filename)
{
  
  BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::read()")
  
    //ditributed meshes
    vector<int*> cellglobal;
  vector<int*> nodeglobal;
  vector<int*> faceglobal;
  
  int nbdomain;

  // reading ascii master file
  try{
    MESSAGE("Start reading");
    ifstream asciiinput(filename);
    
    if (!asciiinput)     
      throw MEDEXCEPTION("MEDSPLITTER read - Master File does not exist");
    
    char charbuffer[512];
    asciiinput.getline(charbuffer,512);
    
    while (charbuffer[0]=='#')
      {
	asciiinput.getline(charbuffer,512);
      }

    //reading number of domains
    nbdomain=atoi(charbuffer);
    cout << "nb domain"<<nbdomain<<endl;
    //    asciiinput>>nbdomain;
    m_filename.resize(nbdomain);
    (m_collection->getMesh()).resize(nbdomain);
    cellglobal.resize(nbdomain);
    nodeglobal.resize(nbdomain);
    faceglobal.resize(nbdomain);
    
    if (nbdomain == 0)
      throw MEDEXCEPTION("Empty ASCII master file");
    for (int i=0; i<nbdomain;i++)
      {
        
    	//reading information about the domain
    	string mesh;
    	int idomain;
    	string host;
      string meshstring;
    	cellglobal[i]=0;
      faceglobal[i]=0;
      nodeglobal[i]=0;
      
    	asciiinput >> mesh >> idomain >> meshstring >> host >> m_filename[i];
    		
    	//Setting the name of the global mesh (which is the same
    	//for all the subdomains)
    	if (i==0)
    	  m_collection->setName(mesh);
    		
    	if (idomain!=i+1)
    	  {
    	    cerr<<"Error : domain must be written from 1 to N in asciifile descriptor"<<endl;
    	    return 1;
    	  }
    	readSubdomain(meshstring, cellglobal,faceglobal,nodeglobal, i);
      
  
      }//loop on domains
    MESSAGE("end of read");
  }//of try
  catch(...)
    {
      cerr << "I/O error reading parallel MED file"<<endl;
      throw;
    }
    
  //creation of topology from mesh and connect zones
  ParallelTopology* aPT = new ParallelTopology
    ((m_collection->getMesh()), (m_collection->getCZ()), cellglobal, nodeglobal, faceglobal);
  m_collection->setTopology(aPT);
    
  for (int i=0; i<nbdomain; i++)
    {
      if (cellglobal[i]!=0) delete[] cellglobal[i];
      if (nodeglobal[i]!=0) delete[] nodeglobal[i];
      if (faceglobal[i]!=0) delete[] faceglobal[i];
    }
  
  END_OF("MEDSPLITTER::MESHCollectionDriver::read()")
    return 0;
}


/*! writes the collection of meshes in a 
 * MED v2.3 file
 * with the connect zones being written as joints
 * \param filename name of the ascii file containing the meshes description
 */
void MESHCollectionMedAsciiDriver::write(char* filename)
{
	
  BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::write()")
 
    ofstream file(filename);

  file <<"#MED Fichier V 2.3"<<" "<<endl;
  file <<"#"<<" "<<endl;
  file<<m_collection->getMesh().size()<<" "<<endl;
	 
  int nbdomains= m_collection->getMesh().size();
  m_filename.resize(nbdomains);

  //loop on the domains
  for (int idomain=0; idomain<nbdomains;idomain++)
    {
      char distfilename[256];
	
      ostringstream suffix;
      suffix << filename<< idomain+1 <<".med";
		
      strcpy(distfilename,suffix.str().c_str());

      m_filename[idomain]=string(distfilename);
		
      MESSAGE("File name "<<string(distfilename));
		
      int id=(m_collection->getMesh())[idomain]->addDriver(MEDMEM::MED_DRIVER,distfilename,(m_collection->getMesh())[idomain]->getName(),MED_EN::MED_CREATE);
		
      MESSAGE("Start writing");
      (m_collection->getMesh())[idomain]->write(id);
		
      //updating the ascii description file
      file << m_collection->getName() <<" "<< idomain+1 << " "<< (m_collection->getMesh())[idomain]->getName() << " localhost " << distfilename << " "<<endl;
	
      writeSubdomain(idomain, nbdomains, distfilename);
    }
	
  END_OF("MEDSPLITTER::MESHCollectionDriver::write()");

}