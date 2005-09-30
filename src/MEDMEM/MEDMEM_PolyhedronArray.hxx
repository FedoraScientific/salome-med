# ifndef __POLYHEDRONARRAY_H__
# define __POLYHEDRONARRAY_H__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {
class POLYHEDRONARRAY
{
private :
  MED_EN::med_int   _numberOfPolyhedron;
  MED_EN::med_int   _numberOfFaces;
  MED_EN::med_int   _numberOfNodes;
  PointerOf <MED_EN::med_int>   _polyhedronIndex;
  PointerOf <MED_EN::med_int>   _facesIndex;
  PointerOf <MED_EN::med_int>   _nodes;

public :
  POLYHEDRONARRAY();
  POLYHEDRONARRAY(MED_EN::med_int numberofpolyhedron,MED_EN::med_int numberoffaces,MED_EN::med_int numberofnodes);
  POLYHEDRONARRAY(const POLYHEDRONARRAY& m);

  inline  MED_EN::med_int   getNumberOfPolyhedron()     const;
  inline  MED_EN::med_int   getNumberOfFaces()          const;
  inline  MED_EN::med_int   getNumberOfNodes()          const;
  inline  const MED_EN::med_int*   getPolyhedronIndex()    const;
  inline  const MED_EN::med_int*   getFacesIndex()         const;
  inline  const MED_EN::med_int*   getNodes()              const;

  inline  void      setPolyhedronIndex(const MED_EN::med_int* polyhedronindex);
  inline  void      setFacesIndex(const MED_EN::med_int* facesindex);
  inline  void      setNodes(const MED_EN::med_int* nodes);
};

// ----------------------------------------------------------
//                       Methodes Inline
// ----------------------------------------------------------
inline MED_EN::med_int POLYHEDRONARRAY::getNumberOfPolyhedron() const
{
  return _numberOfPolyhedron;
};
inline MED_EN::med_int POLYHEDRONARRAY::getNumberOfFaces() const
{
  return _numberOfFaces;
};
inline MED_EN::med_int POLYHEDRONARRAY::getNumberOfNodes() const
{
  return _numberOfNodes;
};
inline const MED_EN::med_int* POLYHEDRONARRAY::getPolyhedronIndex() const
{
  return (const MED_EN::med_int*) _polyhedronIndex;
};
inline const MED_EN::med_int* POLYHEDRONARRAY::getFacesIndex() const
{
  return (const MED_EN::med_int*) _facesIndex;
};
inline const MED_EN::med_int* POLYHEDRONARRAY::getNodes() const
{
  return (const MED_EN::med_int*) _nodes;
};


inline void POLYHEDRONARRAY::setPolyhedronIndex(const MED_EN::med_int* polyhedronindex)
{
  _polyhedronIndex.set(_numberOfPolyhedron+1, polyhedronindex);
}
inline void POLYHEDRONARRAY::setFacesIndex(const MED_EN::med_int* facesindex)
{
  _facesIndex.set(_numberOfFaces+1, facesindex);
}
inline void POLYHEDRONARRAY::setNodes(const MED_EN::med_int* nodes)
{
  _nodes.set(_numberOfNodes, nodes);
}

}

# endif