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
#http://stackoverflow.com/questions/15175318/cmake-how-to-build-external-projects-and-include-their-targets
#http://www.cmake.org/Wiki/CMake:Install_Commands
cmake_minimum_required(VERSION 2.8)

project(openssi-event)

#add_subdirectory(test)
#add_dependencies(ssievent_test ssi)

include_directories(include ../../core/include/ioput/socket ../../core/include/ioput ../../core/include ../)

set(COMMON_SRC_FILES

source/EventBoardWorker.cpp
source/EventMonitor.cpp
source/EventQueue.cpp

source/ThresClassEventSender.cpp
source/XMLEventHelper.cpp
source/XMLEventSender.cpp
source/FixationEventSender.cpp
source/FloatsEventSender.cpp
source/IESelect.cpp
source/ZeroEventSender.cpp
source/TheEventBoard.cpp
source/ThresEventSender.cpp
source/TupleEventSender.cpp
source/StringEventSender.cpp
source/ClockEventSender.cpp

source/ExportMain.cpp


)

 IF(WIN32)
 set(P_SRC_FILES
source/EventSlider.cpp
	)
 ELSE(WIN32)
 set(P_SRC_FILES

	)
 ENDIF(WIN32)
 

 
 set(SRC_FILES ${COMMON_SRC_FILES} ${P_SRC_FILES})




#find_library(OPENSSI_CORE_LIBRARY NAMES openssi-core HINTS ../../cmake-core/corelib/Debug/ ../../cmake-core/corelib/Release/)

IF(MINGW)
  find_library (MINGW_WSOCKET "wsock32" HINTS ${MINGWLIB} )
  find_library (MINGW_WMM "winmm" HINTS ${MINGWLIB})
  find_library (MINGW_WSOCKET2 "ws2_32" HINTS ${MINGWLIB} )
  find_library (MINGW_PTHREAD "pthread" HINTS ${MINGWLIB} )
  	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb" )
  ELSEIF(UNIX)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb" )
  find_library (MINGW_PTHREAD "pthread" HINTS "/usr/lib" )
ELSE(MINGW)
  SET(MINGW_WSOCKET "")
  SET(MINGW_WSOCKET2 "")
  SET(MINGW_WMM "")
   SET(MINGW_PTHREAD "")
ENDIF(MINGW)

add_library(ssievent SHARED ${SRC_FILES} )
target_link_libraries(ssievent ssi ${MINGW_WSOCKET} ${MINGW_WMM} ${MINGW_PTHREAD} )
#add_executable(myapp main.c)

#rename targets if debug
set_target_properties(ssievent PROPERTIES DEBUG_POSTFIX "d" PREFIX "")

#install target to ssi install path set in base directory
install(TARGETS ssievent DESTINATION ${SSI_INSTALL}/${SSI_PLATFORM}/${SSI_COMPILER})
