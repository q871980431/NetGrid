#cmake file for module AI
#author:xuping
#created:2019/3/22 11:59:34

SET(AI
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/AI.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/AI.h"
)

SET(TYPES
	"${CMAKE_CURRENT_SOURCE_DIR}/types/behaviac_types.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/behaviac_agent_member_visitor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/behaviac_agent_meta.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/behaviac_agent_meta.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/behaviac_agent_meta.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/behaviac_headers.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/TestAgent.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/types/internal/TestAgent.cpp"	
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
		${THIRD_LIBRARIES_INCLUDE_DIR}/behaviac/include
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
		${THIRD_LIBRARIES_INCLUDE_DIR}/behaviac/include
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
SOURCE_GROUP( Types FILES ${TYPES})
SOURCE_GROUP( \\ FILES ${AI})

IF(WIN32)
	SET(THIRD_LIB_DEBUG
		"${THIRD_LIBRARIES_DEBUG_DIR}/DbgHelp.Lib"
		"${THIRD_LIBRARIES_DEBUG_DIR}/libbehaviac_msvc_debug.lib"

	)
	SET(THIRD_LIB_RELEASE
	)
ENDIF(WIN32)

IF(LINUX)

ENDIF(LINUX)

ADD_LIBRARY( AI SHARED
	${PUBLIC}
	${API}
	${LOGIC_INTERFACE}
	${LOGIC_DEFINE}
	${TINYXML}
	${COMMON_TIME}
	${AI}
	${TYPES}
)

SET_PROPERTY(TARGET AI PROPERTY FOLDER "Logic")

ADD_DEPENDENCIES(AI NetGrid)

TARGET_LINK_LIBRARIES( AI
	${THIRD_LIB_DEBUG}
	${THIRD_LIB_RELEASE}
	${KERNEL_LIBD}
	${KERNEL_LIB}
)