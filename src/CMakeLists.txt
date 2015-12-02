#
# Copyright (c) 2015 CNRS
# Authors: Mathieu Geisert
#
#

SET(LIBRARY_NAME ${PROJECT_NAME})

SET( SOURCES
  ProjectSupaero
  )

FOREACH( source ${SOURCES} )
  ADD_EXECUTABLE( ${source} ${source}.cpp)
  PKG_CONFIG_USE_DEPENDENCY(${source} acado)
  INSTALL(TARGETS ${source} DESTINATION bin)
ENDFOREACH(source)



