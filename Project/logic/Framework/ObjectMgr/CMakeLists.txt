#cmake file for module ObjectMgr
#author:xuping
#created:2017/9/28 16:21:23

SET(OBJECTMGR
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectMgr.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectMgr.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectDes.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectDes.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectMember.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectMember.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/CommonObject.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/CommonObject.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/TableObject.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TableObject.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PODObject.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PODObject.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/RowObject.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/RowObject.h"	
)
IF(WIN32)
	INCLUDE_DIRECTORIES(
		${API_DIR}
		${PUBLIC_DIR}
		${THIRD_LIBRARIES_INCLUDE_DIR}/DbgHelp.h
        ${2D_DIR}
		${COMMON_TIME_DIR}
		${TINYXML_DIR}
		${LOGIC_INTERFACE_DIR}
		${LOGIC_DEFINE_DIR}
	)
ENDIF(WIN32)

IF(LINUX)
	INCLUDE_DIRECTORIES(
        ${API_DIR}
        ${PUBLIC_DIR}
        ${2D_DIR}
		${COMMON_TIME_DIR}
		${TINYXML_DIR}
		${LOGIC_INTERFACE_DIR}
		${LOGIC_DEFINE_DIR}
	)
ENDIF(LINUX)
MESSAGE(STATUS ${LOGIC_INTERFACE})
SOURCE_GROUP( Api	FILES ${API})
SOURCE_GROUP( Define FILES ${LOGIC_DEFINE})
SOURCE_GROUP( Public\\Tinyxml FILES ${TINYXML})
SOURCE_GROUP( Public\\2D FILES ${2D})
SOURCE_GROUP( Public\\time FILES ${COMMON_TIME})
SOURCE_GROUP( Public FILES ${PUBLIC})
SOURCE_GROUP( Interface FILES ${LOGIC_INTERFACE})
SOURCE_GROUP( \\ FILES ${OBJECTMGR})

IF(WIN32)
	SET(THIRD_LIB_DEBUG
		"${THIRD_LIBRARIES_DEBUG_DIR}/DbgHelp.Lib"
	)
	SET(THIRD_LIB_RELEASE
	)
ENDIF(WIN32)

IF(LINUX)

ENDIF(LINUX)

ADD_LIBRARY( ObjectMgr SHARED
	${PUBLIC}
	${API}
	${LOGIC_INTERFACE}
	${LOGIC_DEFINE}
	${TINYXML}
	${COMMON_TIME}
	${OBJECTMGR}
)

#ADD_DEPENDENCIES( ObjectMgr Test)

SET_PROPERTY(TARGET ObjectMgr PROPERTY FOLDER "Framework")

TARGET_LINK_LIBRARIES( ObjectMgr
	${KERNEL_LIBD}
	${KERNEL_LIB}
)
