#-----------------------------------------------------------------------------
# Initialize project.

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	if(NOT DEFINED WITH_IN_SOURCE_BUILD)
		message(FATAL_ERROR
			"CMake generation for openssi is not allowed within the source directory!"
			"\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
			"\n "
			"\n rm CMakeCache.txt"
			"\n cd .."
			"\n mkdir cmake-make"
			"\n cd cmake-make"
			"\n cmake ../core"
			"\n "
			"\n Alternately define WITH_IN_SOURCE_BUILD to force this option (not recommended!)"
		)		
	endif()
endif()

#http://stackoverflow.com/questions/25609692/how-to-add-source-files-in-another-folder
#file(GLOB Dir1_Sources RELATIVE "Dir1" "*.cpp")
#http://www.opentissue.org/mediawiki/index.php/Using_CMake
cmake_minimum_required(VERSION 2.8)

project(ssigraphic)

IF(UNIX)
INCLUDE(FindPkgConfig)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(SDL2 REQUIRED sdl2)
endif(UNIX)

add_subdirectory(test)
add_dependencies(ssigraphic_test ssi)

include_directories(		include
							include/ioput
							../../core/include/ioput/socket
							../../core/include/ioput
							../../core/include
							../
							
							${CAIRO_INCLUDE_DIRS}
							${SDL2_INCLUDE_DIRS}
							)

set(COMMON_SRC_FILES

source/PaintSignal.cpp
#source/PaintVideo.cpp
source/SignalPainter.cpp
source/ThePainter.cpp
#source/VideoPainter.cpp
source/ExportMain.cpp

)


set(SRC_FILES ${COMMON_SRC_FILES} )

#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)

IF(MINGW)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
  find_library (MINGW_GDIPLUS "gdiplus" HINTS ${MINGWLIB} )
  find_library (MINGW_GDI32 "gdi32" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET "wsock32" HINTS ${MINGWLIB} )
  find_library (MINGW_WMM "winmm" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET2 "ws2_32" HINTS ${MINGWLIB} )
  find_library (MINGW_PTHREAD "pthread" HINTS ${MINGWLIB} )
ELSEIF(UNIX)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb" )
  find_library (MINGW_PTHREAD "pthread" HINTS "/usr/lib" )
  link_directories(${CAIRO_LIBRARY_DIRS})
  link_directories(${SDL2_LIBRARY_DIRS})
ELSE(MINGW)
  

  SET(MINGW_GDIPLUS "")
  SET(MINGW_GDI32 "")
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
  SET(MINGW_PTHREAD "")

ENDIF(MINGW)



add_library(ssigraphic SHARED ${SRC_FILES})
IF(UNIX)
target_link_libraries(ssigraphic ${MINGW_PTHREAD} ${CAIRO_LIBRARIES} ${SDL2_LIBRARIES} -ldl)
ELSE(UNIX)
target_link_libraries(ssigraphic ssi ${MINGW_GDIPLUS} ${MINGW_GDI32} ${MINGW_WSOCKET} ${MINGW_WSOCKET2} ${MINGW_PTHREAD} ${MINGW_WMM})
ENDIF(UNIX)
#add_executable(myapp main.c)

#rename targets if debug
set_target_properties(ssigraphic PROPERTIES DEBUG_POSTFIX "d" PREFIX "")

#install target to ssi install path set in base directory
install(TARGETS ssigraphic DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
