# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

SALOME_CONFIGURE_FILE(Doxyfile_med_user.in Doxyfile_med_user)
SALOME_CONFIGURE_FILE(static/header.html.in static/header.html)

  
IF(SALOME_MED_ENABLE_PYTHON)
  FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/tmp/medcouplingexamples.in" input)
  FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/BuildPyExamplesFromCPP.py" pythondocexamplesgenerator)
  FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" output)
  
  # :TRICKY: For ease of maintenance, documentation for code examples is
  # splitted in several files. We here splice to a single file before running
  # Doxygen.

  SET(EXAMPLE_FILES # files to concatenate: order is important!
    doxfiles/examples/medcouplingexamplesheader.doxy
    doxfiles/examples/medcouplingexamplesfields.doxy
    doxfiles/examples/medcouplingexamplesmeshes.doxy
    doxfiles/examples/medcouplingexamplesarrays.doxy
    doxfiles/examples/medcouplingexamplesother.doxy
    doxfiles/examples/medcouplingexamplesfooter.doxy
  )

  # This function adds IN_FILE contents to the end of OUT_FILE
  FUNCTION(concat IN_FILE OUT_FILE)
    FILE(READ ${IN_FILE} CONTENTS)
    FILE(APPEND ${OUT_FILE} ${CONTENTS})
  ENDFUNCTION()

  # Prepare a temporary file to "concat" to:
  FILE(WRITE ${input} "")
  # Call the "concat" function for each example file
  FOREACH(EXAMPLE_FILE ${EXAMPLE_FILES})
    concat(${EXAMPLE_FILE} ${input})
  ENDFOREACH()
  # Note: The reason for writing to a temporary is so the real target file only
  # gets updated if its content has changed.

  # Here is the "standard" procedure, as if ${input} was hand-written.
  ADD_CUSTOM_TARGET(usr_docs ALL
    COMMAND ${PYTHON_EXECUTABLE} ${pythondocexamplesgenerator} ${input} ${output}
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile_med_user
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  
  SET(doxyfile_med_user ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_med_user)
  FILE(STRINGS ${doxyfile_med_user} enabled_sections REGEX "ENABLED_SECTIONS")
  IF(enabled_sections)
    FILE(READ ${doxyfile_med_user} doxy_file)
    STRING(REPLACE ${enabled_sections} "${enabled_sections} ENABLE_EXAMPLES" new_doxy_file ${doxy_file})
    FILE(WRITE ${doxyfile_med_user} ${new_doxy_file})
  ELSE()
    FILE(APPEND ${doxyfile_med_user} 
      "#Temporary variable to enable python documentation sections\nENABLED_SECTIONS = ENABLE_EXAMPLES")
  ENDIF()
  
ELSE()
  ADD_CUSTOM_TARGET(usr_docs ALL
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile_med_user
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
ENDIF()
 
INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target usr_docs)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc_ref_user/html/ DESTINATION ${SALOME_INSTALL_DOC}/gui/MED)
INSTALL(FILES images/head.png DESTINATION ${SALOME_INSTALL_DOC}/gui/MED) 

SET(MAKE_CLEAN_FILES doc_ref_user tmp)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${MAKE_CLEAN_FILES}")
