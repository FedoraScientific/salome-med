
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
#include "MEDSPLITTER_MESHCollectionMedXMLDriver.hxx"

using namespace MEDSPLITTER;

//template inclusion
#include "MEDSPLITTER_MESHCollectionMedXMLDriver.H"

/*!\class MESHCollectionMedXMLDriver
 *
 *\brief Driver for MED 3.2 files having XML master files
 *
 * Driver for reading and writing distributed files
 * for which the master file is written in an XML format compliant with
 * the MED 3.2 specification.
 * The reading and writing of the meshes and fields are apart :
 * the meshes must always be written/read before the fields. Reading/Writing fields
 * is optional and is done field after field. API for reading/writing fields
 * is written with a template so that MEDMEM::FIELD<int> and MEDMEM::FIELD<double>
 * can be conveniently handled.
*/

MESHCollectionMedXMLDriver::MESHCollectionMedXMLDriver(MESHCollection* collection):MESHCollectionDriver(collection)
{
}

/*!reads a MED File XML Master File v>=2.3
 * and mounts the corresponding meshes in memory
 * the connect zones are created from the joints
 * 
 *\param filename XML file containing the list of MED v2.3 files
 * */
 
int MESHCollectionMedXMLDriver::read(char* filename)
{
  
  BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::read()");
  
		//ditributed meshes
	vector<int*> cellglobal;
  vector<int*> nodeglobal;
  vector<int*> faceglobal;
  
  int nbdomain;

	m_master_filename=filename;

  // reading ascii master file
  try{
    MESSAGE("Start reading");

    // Setting up the XML tree corresponding to filename
    xmlDocPtr master_doc=xmlParseFile(filename);

    if (!master_doc)    
      throw MEDEXCEPTION("MEDSPLITTER XML read - Master File does not exist o r is not compliant with XML scheme");

    ////////////////////
    //number of domains
    ////////////////////
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(master_doc);
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST "//splitting/subdomain", xpathCtx);
    if (xpathObj==0 || xpathObj->nodesetval->nodeNr ==0)
      throw MEDEXCEPTION("MEDSPLITTER read - XML Master File does not contain /MED/splitting/subdomain node");

    /* as subdomain has only one property which is "number"
     * it suffices to take the content of its first child */
    const char* mystring = (const char*)xpathObj->nodesetval->nodeTab[0]->properties->children->content;
    sscanf(mystring, "%d", &nbdomain);

    //////////////////
    //mesh name
    //////////////////
    xpathObj = xmlXPathEvalExpression(BAD_CAST "//content/mesh", xpathCtx);
    if (xpathObj==0 || xpathObj->nodesetval->nodeNr ==0)
      throw MEDEXCEPTION("MEDSPLITTER read - XML Master File does not contain /MED/content/mesh node");
    m_collection->setName( (const char*)xpathObj->nodesetval->nodeTab[0]->properties->children->content);

    cout << "nb domain" << nbdomain << endl;
    m_filename.resize(nbdomain);
    (m_collection->getMesh()).resize(nbdomain);
    cellglobal.resize(nbdomain);
    nodeglobal.resize(nbdomain);
    faceglobal.resize(nbdomain);


    // retrieving the node which contains the file names
    const char filechar[]="//files/subfile";
    xpathObj = xmlXPathEvalExpression(BAD_CAST filechar, xpathCtx);
    if (xpathObj==0 || xpathObj->nodesetval->nodeNr ==0)
      throw MEDEXCEPTION("MEDSPLITTER read - XML Master File does not contain /MED/files/subfile nodes");
    int nbfiles = xpathObj->nodesetval ->nodeNr;

    for (int i=0; i<nbfiles;i++)
      {
        //reading information about the domain

        string host;
        string meshname;
        //char meshname[MED_TAILLE_NOM];

        cellglobal[i]=0;
        faceglobal[i]=0;
        nodeglobal[i]=0;
        
        ////////////////////////////
        //reading file names 
        ////////////////////////////
        ostringstream name_search_string;
        name_search_string<<"//files/subfile[@id=\""<<i+1<<"\"]/name";
        cout <<name_search_string.str()<<endl;
        xmlXPathObjectPtr xpathObjfilename =
          xmlXPathEvalExpression(BAD_CAST name_search_string.str().c_str(),xpathCtx);
        if (xpathObjfilename->nodesetval ==0)
          throw MEDEXCEPTION("MED XML reader : Error retrieving file name ");
        //ostringstream filename(m_filename[i]);
        m_filename[i]=(const char*)xpathObjfilename->nodesetval->nodeTab[0]->children->content;
        //filename.flush();
        //cout<<m_filename[i]<<endl;
//				xmlNodePtr current =  xpathObj->nodesetval->nodeTab[i];
//				xmlAttrPtr properties = current->properties;
//        
//        
//				//browsing XML attributes of subfile node until "id" attribute is found
//				while(strcmp((const char*)(properties->name),"id")!=0)
//					properties=properties->next;
//				sscanf((const char*)properties->children->content,"%d",&idomain);
//				properties = current->properties;
//    
//				//browsing XML attributes of subfile node until "name" attribute is found
//				while(strcmp((const char*)(properties->name),"name")!=0)
//					properties=properties->next;
//				m_filename[i]=(const char*)properties->children->content;

				////////////////////////////////
				//reading the local mesh names
				////////////////////////////////
        ostringstream mesh_search_string;
        mesh_search_string<<"//mapping/mesh/chunk[@subdomain=\""<<i+1<<"\"]/name";
        
        xmlXPathObjectPtr xpathMeshObj = xmlXPathEvalExpression(BAD_CAST mesh_search_string.str().c_str(),xpathCtx);
        if (xpathMeshObj->nodesetval ==0)
          throw MEDEXCEPTION("MED XML reader : Error retrieving mesh name ");
        meshname=(const char*)xpathMeshObj->nodesetval->nodeTab[0]->children->content;

         
//         
//				xmlXPathObjectPtr xpathMeshObj = xmlXPathEvalExpression(BAD_CAST "//mesh/chunk" , xpathCtx);
//				int nbchunks = xpathMeshObj->nodesetval ->nodeNr;
//				for (int ichunk=0; ichunk<nbchunks; ichunk++)
//					{
//						xmlNodePtr currentchunk =  xpathMeshObj->nodesetval->nodeTab[ichunk];
//						xmlAttrPtr propertieschunk = currentchunk->properties;
//						int id;
//						sscanf((const char*)propertieschunk->children->content, "%d",&id);
//						if (strcmp((const char*)propertieschunk->name, "subdomain")==0 && id==idomain)
//							{
//								propertieschunk=propertieschunk->next;
//								meshstring=(const char*)propertieschunk->children->content;
//							}
//					}
				
//				if (idomain!=i+1)
//					{
//						cerr<<"Error : domain must be written from 1 to N in master file descriptor"<<endl;
//						return 1;
//					}
        readSubdomain(meshname, cellglobal, faceglobal, nodeglobal, i);
        xmlXPathFreeObject(xpathObjfilename);
        
        xmlXPathFreeObject(xpathMeshObj);
	    }//loop on domains
        
        // LIBXML cleanup
    xmlXPathFreeObject(xpathObj); 
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(master_doc); 
  
		MESSAGE("end of read");
  }//of try
  catch(...)
    {
      throw MEDEXCEPTION("I/O error reading parallel MED file");
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
 * MED v2.3 XML file
 * with the connect zones being written as joints
 * \param filename name of the XML file containing the meshes description
 */
void MESHCollectionMedXMLDriver::write(char* filename)
{
	
	BEGIN_OF("MEDSPLITTER::MESHCollectionDriver::writeXML()");
 
	xmlDocPtr master_doc = 0;
	xmlNodePtr root_node = 0, node, node2;
	//	xmlDTDPtr dtd = 0;

	char buff[256];

	//Creating the XML document

	master_doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(0, BAD_CAST "root");
	xmlDocSetRootElement(master_doc,root_node);
	
	//Creating child nodes

	// Version tag
	node = xmlNewChild(root_node, 0, BAD_CAST "version",0);
	xmlNewProp(node, BAD_CAST "maj", BAD_CAST "2");
	xmlNewProp(node, BAD_CAST "min", BAD_CAST "3");
	xmlNewProp(node, BAD_CAST "ver", BAD_CAST "1");

	//Description tag

	time_t present;
	time( &present);
	struct tm *time_asc = localtime(&present);
	char date[6];
	sprintf(date,"%02d%02d%02d",time_asc->tm_year
					                ,time_asc->tm_mon+1
					                ,time_asc->tm_mday);

	node = xmlNewChild(root_node,0, BAD_CAST "description",0);

	xmlNewProp(node, BAD_CAST "what", BAD_CAST m_collection->getDescription().c_str());
	xmlNewProp(node, BAD_CAST "when", BAD_CAST date);
	
	//Content tag
	node =xmlNewChild(root_node,0, BAD_CAST "content",0);
	node2 = xmlNewChild(node, 0, BAD_CAST "mesh",0);
	xmlNewProp(node2, BAD_CAST "name", BAD_CAST m_collection->getName().c_str());
	
	//Splitting tag
	node=xmlNewChild(root_node,0,BAD_CAST "splitting",0);
	node2=xmlNewChild(node,0,BAD_CAST "subdomain",0);
	sprintf(buff, "%d", m_collection->getMesh().size());
	xmlNewProp(node2, BAD_CAST "number", BAD_CAST buff);
	node2=xmlNewChild(node,0,BAD_CAST "global_numbering",0);
	xmlNewProp(node2, BAD_CAST "present", BAD_CAST "yes");

	//Files tag
	xmlNodePtr file_node=xmlNewChild(root_node,0,BAD_CAST "files",0);

	//Mapping tag
	node = xmlNewChild(root_node,0,BAD_CAST "mapping",0);
	xmlNodePtr mesh_node = xmlNewChild(node, 0, BAD_CAST "mesh",0);
	xmlNewProp(mesh_node, BAD_CAST "name", BAD_CAST m_collection->getName().c_str());

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
			node = xmlNewChild(file_node, 0, BAD_CAST "subfile",0);
			sprintf (buff,"%d",idomain+1);
			xmlNewProp(node, BAD_CAST "id", BAD_CAST buff);
      xmlNewChild(node,0,BAD_CAST "name",BAD_CAST distfilename);
      xmlNewChild(node,0,BAD_CAST "machine",BAD_CAST "localhost");

			node = xmlNewChild(mesh_node,0, BAD_CAST "chunk",0);
			xmlNewProp(node, BAD_CAST "subdomain", BAD_CAST buff);
      xmlNewChild(node,0,BAD_CAST "name", BAD_CAST (m_collection->getMesh())[idomain]->getName().c_str());
		
      writeSubdomain(idomain, nbdomains, distfilename);
			
		}
	strcat(filename,".xml");
	m_master_filename=filename;
	xmlSaveFormatFileEnc(filename, master_doc, "UTF-8", 1);
 	xmlFreeDoc(master_doc);
	xmlCleanupParser();

	END_OF("MEDSPLITTER::MESHCollectionDriver::writeXML()");

}