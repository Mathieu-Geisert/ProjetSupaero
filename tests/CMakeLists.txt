#
#  Copyright 2015 CNRS
#

# --- MACROS ------------------------------------------------------------------
# --- MACROS ------------------------------------------------------------------
# --- MACROS ------------------------------------------------------------------
MACRO(ADD_EXEC NAME PKGS)
    ADD_EXECUTABLE(${NAME} ${NAME})
  FOREACH(PKG ${PKGS})
    PKG_CONFIG_USE_DEPENDENCY(${NAME} ${PKG})
  ENDFOREACH(PKG)
  TARGET_LINK_LIBRARIES (${NAME} ${PROJECT_NAME})
ENDMACRO(ADD_EXEC)


ADD_EXEC(test "gepetto-viewer-corba")
