#
# Copyright (c) 2015 CNRS
# Authors: Mathieu Geisert
#
#

SET(LIBRARY_NAME ${PROJECT_NAME})

SET( SOURCES
  ProjectSupaero
  thetered_aerial
  )

FOREACH( source ${SOURCES} )
  ADD_EXECUTABLE( ${source} ${source}.cpp)
  PKG_CONFIG_USE_DEPENDENCY(${source} gepetto-viewer-corba)
  INSTALL(TARGETS ${source} DESTINATION bin)
ENDFOREACH(source)



