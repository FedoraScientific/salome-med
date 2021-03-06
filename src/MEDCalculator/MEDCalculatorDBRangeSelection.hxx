// Copyright (C) 2007-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCALCULATORDBRANGESELECTION_HXX__
#define __MEDCALCULATORDBRANGESELECTION_HXX__

#include "MedCalculatorDefines.hxx"
#include "InterpKernelException.hxx"

#include <vector>

namespace ParaMEDMEM
{
  class MEDCALCULATOR_EXPORT MEDCalculatorDBRangeSelection
  {
  public:
    MEDCalculatorDBRangeSelection(const char *v) throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBRangeSelection(int v);
    MEDCalculatorDBRangeSelection();
    void setPyStart(int val);
    void setPyEnd(int val);
    MEDCalculatorDBRangeSelection& operator=(const char *v) throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBRangeSelection& operator=(int v) throw(INTERP_KERNEL::Exception);
    std::vector<int> getIds(int lgth) const throw(INTERP_KERNEL::Exception);
    int getSize(int lgth) const throw(INTERP_KERNEL::Exception);
    bool isAll() const;
    void setAll();
  private:
    void setValue(const char *v) throw(INTERP_KERNEL::Exception);
    void setValue(int v) throw(INTERP_KERNEL::Exception);
    static int TraducePyVal(int val);
  private:
    static const char SEPARATOR=':';
    static const char ALL_ELTS[];
  private:
    int _start;
    int _end;
  };
}

#endif
