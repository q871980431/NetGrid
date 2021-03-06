SET(KERNEL_DIR
	"${PROJECT_SOURCE_DIR}/Kernel"
)
SET( KERNEL
	"${KERNEL_DIR}/main.cpp"
	"${KERNEL_DIR}/Kernel.h"
	"${KERNEL_DIR}/Kernel.cpp"
)

SET(KERNEL_INTERFACE_DIR
	"${KERNEL_DIR}/Interface"
)

FILE(GLOB_RECURSE KERNEL_INTERFACE ${KERNEL_INTERFACE_DIR}/*.h)

SET(KERNEL_LOG_DIR	"${KERNEL_DIR}/Log")
SET(KERNEL_LOG
	"${KERNEL_LOG_DIR}/LogFile.h"
	"${KERNEL_LOG_DIR}/LogFile.cpp"
	"${KERNEL_LOG_DIR}/Logger.h"
	"${KERNEL_LOG_DIR}/Logger.cpp"
)

SET(KERNEL_CONFIG_DIR	"${KERNEL_DIR}/Config")
SET(KERNEL_CONFIG
	"${KERNEL_CONFIG_DIR}/Configmgr.cpp"
	"${KERNEL_CONFIG_DIR}/Configmgr.h"
)

SET(KERNEL_MODULE_DIR	"${KERNEL_DIR}/Module")
SET(KERNEL_MODULE
	"${KERNEL_MODULE_DIR}/Modulemgr.cpp"
	"${KERNEL_MODULE_DIR}/Modulemgr.h"
)

SET(KERNEL_NET_DIR	"${KERNEL_DIR}/Net")
SET(KERNEL_NET
	"${KERNEL_NET_DIR}/TcpConnection.cpp"
	"${KERNEL_NET_DIR}/TcpConnection.h"
    "${KERNEL_NET_DIR}/NetService.cpp"
	"${KERNEL_NET_DIR}/NetService.h"
    "${KERNEL_NET_DIR}/NetListenerMgr.cpp"
	"${KERNEL_NET_DIR}/NetListenerMgr.h"
    "${KERNEL_NET_DIR}/NetIOEngine.cpp"
	"${KERNEL_NET_DIR}/NetIOEngine.h"
    "${KERNEL_NET_DIR}/IOEngine_IOCP.cpp"
	"${KERNEL_NET_DIR}/IOEngine_IOCP.h"
    "${KERNEL_NET_DIR}/IOEngine_Epoll.cpp"
	"${KERNEL_NET_DIR}/IOEngine_Epoll.h"
)

SET( KERNEL_TIMER_DIR "${KERNEL_DIR}/Timer")
SET( KERNEL_TIMER
	"${KERNEL_TIMER_DIR}/TimerMgr.cpp"
	"${KERNEL_TIMER_DIR}/TimerMgr.h"
   	"${KERNEL_TIMER_DIR}/TimerWheel.cpp"
	"${KERNEL_TIMER_DIR}/TimerWheel.h"
)

SET( KERNEL_FRAME_DIR "${KERNEL_DIR}/Frame")
SET( KERNEL_FRAME
	"${KERNEL_FRAME_DIR}/FrameMgr.cpp"
	"${KERNEL_FRAME_DIR}/FrameMgr.h"
)

SET( KERNEL_ASYNC_DIR "${KERNEL_DIR}/Async")
SET( KERNEL_ASYNC
	"${KERNEL_ASYNC_DIR}/AsyncQueue.cpp"
	"${KERNEL_ASYNC_DIR}/AsyncQueue.h"
	"${KERNEL_ASYNC_DIR}/AsyncThread.cpp"
	"${KERNEL_ASYNC_DIR}/AsyncThread.h"	
)

SET( KERNEL_SEH_DIR "${KERNEL_DIR}/Exception")
SET( KERNEL_SEH
	"${KERNEL_SEH_DIR}/ExceptionMgr.h"
	"${KERNEL_SEH_DIR}/ExceptionMgr.cpp"
	"${KERNEL_SEH_DIR}/i386-signal.h"
	"${KERNEL_SEH_DIR}/x86_64-signal.h"	
	"${KERNEL_SEH_DIR}/segvcatch.cpp"
	"${KERNEL_SEH_DIR}/segvcatch.h"	
)
SET( KERNEL_PROFILE_DIR "${KERNEL_DIR}/Proflie")
FILE(GLOB_RECURSE KERNEL_PROFILE ${KERNEL_PROFILE_DIR}/*.h ${KERNEL_PROFILE_DIR}/*.cpp)
MESSAGE("Profile:${KERNEL_PROFILE}")
MESSAGE("SEH:${KERNEL_SEH}")

IF(WIN32)
	INCLUDE_DIRECTORIES(
		${API_DIR}
		${PUBLIC_DIR}
		${KERNEL_INTERFACE_DIR}
		${KERNEL_LOG_DIR}
		${THIRD_LIBRARIES_INCLUDE_DIR}/DbgHelp.h		
		${KERNEL_CONFIG_DIR}
        ${2D_DIR}
		${COMMON_TIME_DIR}
		${TINYXML_DIR}
        ${KERNEL_MODULE_DIR}
		${KERNEL_NET_DIR}
		${KERNEL_TIMER_DIR}
		${KERNEL_FRAME_DIR}
		${KERNEL_ASYNC_DIR}
		${KERNEL_SEH_DIR}
		${KERNEL_PROFILE_DIR}
		${THIRD_LIBRARIES_INCLUDE_DIR}/gperftools
		${THIRD_LIBRARIES_INCLUDE_DIR}/gperftools/windows	
	)
ENDIF(WIN32)

IF(LINUX)
	INCLUDE_DIRECTORIES(
                ${API_DIR}
                ${PUBLIC_DIR}
                ${KERNEL_INTERFACE_DIR}
                ${KERNEL_LOG_DIR}
				${THIRD_LIBRARIES_INCLUDE_DIR}/unwind
                ${KERNEL_CONFIG_DIR}
				${KERNEL_MODULE_DIR}
				${COMMON_TIME_DIR}
				${TINYXML_DIR}
				${KERNEL_MODULE_DIR}
				${KERNEL_NET_DIR}
				${KERNEL_TIMER_DIR}
				${KERNEL_FRAME_DIR}
			    ${KERNEL_ASYNC_DIR}		
				${KERNEL_SEH_DIR}
				${KERNEL_PROFILE_DIR}
				${THIRD_LIBRARIES_INCLUDE_DIR}/gperftools
        )
ENDIF(LINUX)
MESSAGE("test")
MESSAGE(STATUS ${COMMON_TIME})
SOURCE_GROUP( Api	FILES ${API})
SOURCE_GROUP( Public\\Tinyxml FILES ${TINYXML})
SOURCE_GROUP(Public\\2D FILES ${2D})
SOURCE_GROUP(Public\\time FILES ${COMMON_TIME})
SOURCE_GROUP( Public FILES ${PUBLIC})
SOURCE_GROUP( Src\\Interface FILES ${KERNEL_INTERFACE})
SOURCE_GROUP( Src\\Log FILES ${KERNEL_LOG})
SOURCE_GROUP( Src\\Config FILES ${KERNEL_CONFIG})
SOURCE_GROUP( Src\\Module FILES ${KERNEL_MODULE})
SOURCE_GROUP( Src\\Net FILES ${KERNEL_NET})
SOURCE_GROUP( Src\\Timer FILES ${KERNEL_TIMER})
SOURCE_GROUP( Src\\Frame FILES ${KERNEL_FRAME})
SOURCE_GROUP( Src\\Async FILES ${KERNEL_ASYNC})
SOURCE_GROUP( Src\\Proflie FILES ${KERNEL_PROFILE})
SOURCE_GROUP( Src\\SEH 	 FILES ${KERNEL_SEH})

SOURCE_GROUP( Src FILES ${KERNEL})


IF(WIN32)
	SET(THIRD_LIB_DEBUG
	debug "${THIRD_LIBRARIES_DEBUG_DIR}/DbgHelp.Lib"
	"ws2_32.lib"
	"wsock32.lib"
	debug "${THIRD_LIBRARIES_DEBUG_DIR}/libtcmalloc_minimal.lib"
	)
	SET(THIRD_LIB_RELEASE
	    optimized "ws2_32.lib"
	    optimized "wsock32.lib"
	)
ENDIF(WIN32)

IF(LINUX)
	SET(THIRD_LIB_DEBUG
	)
	SET(THIRD_LIB_RELEASE
#		optimized "${THIRD_LIBRARIES_RELEASE_DIR}/libunwind.a"
#		optimized "${THIRD_LIBRARIES_RELEASE_DIR}/libunwind-setjmp.a"
#		optimized "${THIRD_LIBRARIES_RELEASE_DIR}/libunwind-x86_64.a"
#		optimized "${THIRD_LIBRARIES_RELEASE_DIR}/libunwind-generic.a"

	)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lunwind -lunwind-x86_64 -lunwind-setjmp -ltcmalloc_and_profiler")
ENDIF(LINUX)

ADD_EXECUTABLE( NetGrid
				${PUBLIC}
				${API}
				${KERNEL_INTERFACE}
				${KERNEL_LOG}
                ${KERNEL_CONFIG}
				${KERNEL_MODULE}
				${KERNEL_NET}
				${KERNEL_TIMER}
				${KERNEL_FRAME}
				${KERNEL_ASYNC}
				${KERNEL_SEH}
				${KERNEL}
				${TINYXML}
                ${2D}
				${COMMON_TIME}
				${KERNEL_PROFILE}
)

IF(WIN32)
	SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/windows/")
ENDIF(WIN32)
IF(LINUX)
	SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/linux/")
ENDIF(LINUX)

TARGET_LINK_LIBRARIES(	NetGrid
	${THIRD_LIB_DEBUG}
	${THIRD_LIB_RELEASE}
)
