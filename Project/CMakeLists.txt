cmake_minimum_required(VERSION 2.4)
PROJECT(NetGrid)
IF(WIN32)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
	LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/windows)
	set(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zm2000 /Zi /Ob0 /Od /RTC1") 
	SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "/MT /Zm2000 /O1 /Ob1 /D NDEBUG")
	SET(CMAKE_C_FLAGS_RELEASE_INIT "/MT /Zm2000 /O2 /Ob2 /D NDEBUG")
	SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zm2000 /Zi /O2 /Ob1 /D NDEBUG")
	SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/windows/")
	MESSAGE(STATUS "WINDOWS")
	IF(CMAKE_CL_64)
		MESSAGE(STATUS "x64")
	ELSE()
		MESSAGE(STATUS "x86")
	ENDIF(CMAKE_CL_64)
ENDIF(WIN32)

IF(LINUX)
	SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/linux")
	#SET(CMAKE_BUILD_TYPE "Debug")
	LINK_DIRECTORIES("${PROJECT_BINARY_DIR}/linux")
	#SET(CMAKE_C_FLAGS	"-std=c++0x -Wall -lpthread -rdynamic -ldl -Dlinux -lcurl -lmysqlclient -lTNavigation")
        SET(CMAKE_CXX_FLAGS " -std=c++11 -Wall -DLINUX -pthread -rdynamic -ldl")
	#SET(CMAKE_C_FLAGS_DEBUG	"-g -D_DEBUG -o0 -pg -L${PROJECT_SOURCE_DIR}/Thridlibrary/debug/linux")
        SET(CMAKE_C_FLAGS_DEBUG "-g -D_DEBUG -O -pg -L${PROJECT_SOURCE_DIR}/Thridlibrary/debug/linux")
	#SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 _L${PROJECT_SOURCE_DIR}/Thridlibrary/release/linux")
	SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O2 _L${PROJECT_SOURCE_DIR}/Thridlibrary/release/linux")
	#SET(CMAKE_C_FLAGS_RELEASE "-O3 _L${PROJECT_SOURCE_DIR}/Thridlibrary/release/linux")
	SET(CMAKE_C_FLAGS_RELEASE "-O3 _L${PROJECT_SOURCE_DIR}/Thridlibrary/release/linux")
	MESSAGE(STATUS "Linux")
ENDIF(LINUX)

SET(API_DIR
	"${PROJECT_SOURCE_DIR}/API/"
)
SET(API
	"${PROJECT_SOURCE_DIR}/API/IKernel.h"
	"${PROJECT_SOURCE_DIR}/API/IModule.h"	
)

SET(PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/Public"
)
SET(PUBLIC
	"${PUBLIC_DIR}/MultiSys.h"
	"${PUBLIC_DIR}/LinSys.h"
	"${PUBLIC_DIR}/WinSys.h"
	"${PUBLIC_DIR}/Tools.h"
	"${PUBLIC_DIR}/Tools.cpp"
	"${PUBLIC_DIR}/Singleton.h"
	"${PUBLIC_DIR}/CDumper.h"
	"${PUBLIC_DIR}/CDumper.cpp"
	"${PUBLIC_DIR}/IStream.h"
	"${PUBLIC_DIR}/TString.h"
	"${PUBLIC_DIR}/CircularBuffer.h"
	"${PUBLIC_DIR}/CircularQueue.h"
	"${PUBLIC_DIR}/IReader.h"
	"${PUBLIC_DIR}/TDynArray.h"
	"${PUBLIC_DIR}/TList.h"
	"${PUBLIC_DIR}/TDynPool.h"
	"${PUBLIC_DIR}/XmlReader.cpp"
	"${PUBLIC_DIR}/XmlReader.h"
    "${PUBLIC_DIR}/Convert.h"
    "${PUBLIC_DIR}/Tools_time.h"
    "${PUBLIC_DIR}/Tools_time.cpp"
    "${PUBLIC_DIR}/BitMark.h"	
    "${PUBLIC_DIR}/Tools_file.h"
    "${PUBLIC_DIR}/Tools_file.cpp"
    "${PUBLIC_DIR}/TCallBack.h"	
)
SET(TINYXML_DIR
	"${PUBLIC_DIR}/tinyxml2"
)
SET(TINYXML
	"${TINYXML_DIR}/tinyxml2.h"
	"${TINYXML_DIR}/tinyxml2.cpp"
)
SET(2D_DIR
	"${PUBLIC_DIR}/2D"
)
SET(2D
	"${2D_DIR}/Zone.h"
	"${2D_DIR}/ZoneAOI.h"
    "${2D_DIR}/TZoneAOI.h"
	"${2D_DIR}/ZoneAOI.cpp"
	"${2D_DIR}/Vector.h"
    "${2D_DIR}/RectTree.h"
	"${2D_DIR}/RectTree.cpp"
    "${2D_DIR}/RTree.h"
    "${2D_DIR}/TRectTree.h"
    "${2D_DIR}/SearchTree.cpp"
    "${2D_DIR}/SearchTree.h"
)

SET(COMMON_TIME_DIR
	"${PUBLIC_DIR}/time/"
)
SET(COMMON_TIME
	"${PUBLIC_DIR}/time/StopWatch.h"
)


MESSAGE(STATUS ${PUBLIC})
IF(WIN32)
SET(THIRD_LIBRARIES_DEBUG_DIR
	"${PROJECT_SOURCE_DIR}/Thridlibrary/windows/debug"
)
SET(THIRD_LIBRARIES_RELEASE_DIR
	"${PROJECT_SOURCE_DIR}/Thridlibrary/windows/release"
)

SET(OBJECT_LIBD
	debug "${PROJECT_BINARY_DIR}/windows/debug/ObjectMgr.lib"
)
SET(OBJECT_LIB
	optimized "${PROJECT_BINARY_DIR}/windows/Release/ObjectMgr.lib"
)


ENDIF(WIN32)

SET(THIRD_LIBRARIES_INCLUDE_DIR
	"${PROJECT_SOURCE_DIR}/Thridlibrary/include"
)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/Kernel/)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/)