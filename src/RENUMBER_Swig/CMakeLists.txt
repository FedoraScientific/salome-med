# Copyright (C) 2012-2014  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

INCLUDE(${SWIG_USE_FILE})

ADD_DEFINITIONS(${PYTHON_DEFINITIONS})

SET_SOURCE_FILES_PROPERTIES(MEDRenumber.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(MEDRenumber.i PROPERTIES SWIG_DEFINITIONS "-shadow")
SET(SWIG_MODULE_MEDRenumber_EXTRA_FLAGS "${SWIG_EXTRA_FLAGS_FOR_NUMPYANDSCIPY};-DWITHOUT_AUTOFIELD")

IF(Boost_FOUND)
  SET(SWIG_MODULE_MEDRenumber_EXTRA_FLAGS -DHAS_BOOST ${SWIG_MODULE_MEDRenumber_EXTRA_FLAGS})
ENDIF(Boost_FOUND)

IF(METIS_FOUND)
  SET(SWIG_MODULE_MEDRenumber_EXTRA_FLAGS -DHAS_METIS ${SWIG_MODULE_MEDRenumber_EXTRA_FLAGS})
ENDIF(METIS_FOUND)

SET (MEDRenumber_SWIG_DPYS_FILES
    MEDRenumberCommon.i)   

INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR} # pthread dependancy due to python2.7 library
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../RENUMBER
  ${CMAKE_CURRENT_SOURCE_DIR}/../MEDCoupling_Swig
  ${CMAKE_CURRENT_SOURCE_DIR}/../MEDCoupling
  ${CMAKE_CURRENT_SOURCE_DIR}/../INTERP_KERNEL
  ${CMAKE_CURRENT_SOURCE_DIR}/../INTERP_KERNEL/Bases
  ${CMAKE_CURRENT_SOURCE_DIR}/../INTERP_KERNEL/Geometric2D
  ${CMAKE_CURRENT_SOURCE_DIR}/../INTERP_KERNEL/ExprEval
  ${CMAKE_CURRENT_SOURCE_DIR}/../INTERP_KERNEL/GaussPoints
  )

SWIG_ADD_MODULE(MEDRenumber python MEDRenumber.i)
SWIG_LINK_LIBRARIES(MEDRenumber ${PYTHON_LIBRARIES} ${PLATFORM_LIBS} renumbercpp)

IF(WIN32)
  SET_TARGET_PROPERTIES(_MEDRenumber PROPERTIES DEBUG_OUTPUT_NAME _MEDRenumber_d)
ENDIF(WIN32)
INSTALL(TARGETS ${SWIG_MODULE_MEDRenumber_REAL_NAME} DESTINATION ${SALOME_INSTALL_PYTHON})

SET(PYFILES_TO_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/MEDRenumber.py)
INSTALL_AND_COMPILE_PYTHON_FILE("${PYFILES_TO_INSTALL}" ${SALOME_INSTALL_SCRIPT_PYTHON})

INSTALL(FILES MEDRenumber.i MEDRenumberCommon.i DESTINATION ${SALOME_INSTALL_HEADERS})
INSTALL(FILES MEDRenumberTest.py DESTINATION ${SALOME_INSTALL_SCRIPT_PYTHON})

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
ADD_TEST(MEDRenumberTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/MEDRenumberTest.py)
SET_TESTS_PROPERTIES(MEDRenumberTest PROPERTIES ENVIRONMENT "${tests_env}")
