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

cmake_minimum_required(VERSION 2.8)

project(ssigraphic_test)

IF(UNIX)
INCLUDE(FindPkgConfig)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(SDL2 REQUIRED sdl2)
endif(UNIX)

include_directories (
	.
	../../../core/include
	../../../core/include/ioput/socket
	../../../core/include/ioput
	
	../../../plugins/
	../../../plugins/graphic/include
	
	${CAIRO_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}
)

set(COMMON_SRC_FILES

 Main_.cpp

 
)

IF(MINGW)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
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
  find_library (X11 "X11" HINTS "/usr/X11R6/lib")
  find_library (Xi "Xi" HINTS "/usr/X11R6/lib")
ELSE(MINGW)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
  SET(MINGW_PTHREAD "")
ENDIF(MINGW)


set(SRC_FILES ${COMMON_SRC_FILES} )


add_executable(ssigraphic_test ${SRC_FILES})
if(UNIX)
target_link_libraries(ssigraphic_test ssi ${MINGW_WSOCKET} ${MINGW_WSOCKET2} ${MINGW_WMM} ${MINGW_PTHREAD} ${X11} ${Xi} ${CAIRO_LIBRARIES} ${SDL2_LIBRARIES})
else(UNIX)
target_link_libraries(ssigraphic_test ssi ${MINGW_WSOCKET} ${MINGW_WSOCKET2} ${MINGW_WMM} ${MINGW_PTHREAD})
endif(UNIX)


#rename targets if debug
set_target_properties(ssigraphic_test PROPERTIES DEBUG_POSTFIX "d" )

#install target to ssi install path set in base directory
install(TARGETS ssigraphic_test DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER}/)
