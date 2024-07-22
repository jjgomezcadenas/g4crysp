cmake_minimum_required(VERSION 3.16...3.21)
project(Geant4WithCSV)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Find Apache Arrow and Parquet packages
#find_package(Arrow REQUIRED)
#find_package(Parquet REQUIRED)
find_package(GSL REQUIRED)

# Include Geant4's use file
include(${Geant4_USE_FILE})

include_directories(${GSL_INCLUDE_DIRS})
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

message(STATUS "G4L = ${Geant4_LIBRARIES}")

add_executable(crystalApp crystal.cc ${sources} ${headers})
target_link_libraries(crystalApp ${Geant4_LIBRARIES})

# Link the executable with GSL libraries
target_link_libraries(crystalApp ${GSL_LIBRARIES})


# Link against Geant4, Arrow, and Parquet
#target_link_libraries(crystalApp ${Geant4_LIBRARIES} Arrow::arrow_bundled_dependencies Parquet::parquet_shared)
