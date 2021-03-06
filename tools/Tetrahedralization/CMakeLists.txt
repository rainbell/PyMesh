# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")
SET(DEPENDENT_INCLUDES "")

# Inlclude CGAL
IF(CGAL_FOUND)
    ADD_DEFINITIONS(-DWITH_CGAL)
    ADD_DEFINITIONS(-DCGAL_CONCURRENT_MESH_3)
    LIST(APPEND DEPENDENT_LIBS cgal_wrapper)
    ADD_SUBDIRECTORY(CGAL)
ELSE(CGAL_FOUND)
    MESSAGE(WARNING "CGAL missing.  Skipping CGAL tetrahedralization support")
ENDIF(CGAL_FOUND)

IF(LIBIGL_FOUND)
    LIST(APPEND DEPENDENT_INCLUDES ${LIBIGL_INCLUDE_DIRS})
    ADD_DEFINITIONS(-DWITH_IGL)
ENDIF(LIBIGL_FOUND)

# Include TetGen
IF(TETGEN_FOUND)
    LIST(APPEND DEPENDENT_LIBS lib_tetgen_wrapper)
    ADD_DEFINITIONS(-DWITH_TETGEN)
    ADD_SUBDIRECTORY(TetGen)
ELSE(TETGEN_FOUND)
    MESSAGE(WARNING "Tetgen missing.  Skipping TetGen tetrahedralization support")
ENDIF(TETGEN_FOUND)

# Include Geogram
IF (GEOGRAM_FOUND)
    LIST(APPEND DEPENDENT_LIBS geogram)
    ADD_DEFINITIONS(-DWITH_GEOGRAM)
    ADD_SUBDIRECTORY(Geogram)
ENDIF (GEOGRAM_FOUND)

# Include Quartet
IF (QUARTET_FOUND)
    LIST(APPEND DEPENDENT_INCLUDES ${QUARTET_INCLUDE_DIRS})
    LIST(APPEND DEPENDENT_LIBS ${QUARTET_LIBRARIES})
    ADD_DEFINITIONS(-DWITH_QUARTET)
    ADD_SUBDIRECTORY(Quartet)
ENDIF (QUARTET_FOUND)

IF (MMG_FOUND AND TETGEN_FOUND)
    LIST(APPEND DEPENDENT_INCLUDES ${MMG_INCLUDE_DIRS})
    LIST(APPEND DEPENDENT_LIBS ${MMG_LIBRARY})
    ADD_DEFINITIONS(-DWITH_MMG)
    ADD_SUBDIRECTORY(MMG)
ENDIF (MMG_FOUND AND TETGEN_FOUND)

IF (TETWILD_FOUND)
    LIST(APPEND DEPENDENT_INCLUDES ${TETWILD_INCLUDE_DIRS})
    LIST(APPEND DEPENDENT_LIBS ${TETWILD_LIBRARY})
    ADD_DEFINITIONS(-DWITH_TETWILD)
    ADD_SUBDIRECTORY(TetWild)
ENDIF (TETWILD_FOUND)

ADD_LIBRARY(lib_tetrahedralization SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_tetrahedralization
    ${DEPENDENT_LIBS} Mesh lib_MeshUtils)
TARGET_INCLUDE_DIRECTORIES(lib_tetrahedralization SYSTEM PRIVATE
    ${DEPENDENT_INCLUDES})
#TARGET_COMPILE_DEFINITIONS(lib_tetrahedralization PRIVATE CGAL_MESH_3_PROTECTION_DEBUG)
SET_TARGET_PROPERTIES(lib_tetrahedralization PROPERTIES OUTPUT_NAME
    "tetrahedronization")

ADD_CUSTOM_TARGET(tetrahedralization DEPENDS lib_tetrahedralization)

