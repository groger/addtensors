cmake_minimum_required(VERSION 2.6)
 
PROJECT(addtensors)
#PROJECT(testAffichage)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/Library)
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(Library)


FIND_PACKAGE(GenerateCLP REQUIRED)
IF(GenerateCLP_FOUND)
  INCLUDE(${GenerateCLP_USE_FILE})
ELSE(GenerateCLP_FOUND)
  MESSAGE(FATAL_ERROR
          "GenerateCLP not found. Please set GenerateCLP_DIR.")
ENDIF(GenerateCLP_FOUND)

SET(src addtensors.cpp)
GENERATECLP(src addtensors.xml)

add_executable(addtensors  ${src}  )
target_link_libraries( addtensors library ${ITK_LIBRARIES} vtkHybrid vtkCommon ${VTK_LIBRARIES})
