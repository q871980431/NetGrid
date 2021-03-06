SET(TEST
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Test.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Test.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/hmac.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/hmac_sha1.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/sha.h"	
	"${CMAKE_CURRENT_SOURCE_DIR}/sha1.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestMatch.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestMatch.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectTest.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ObjectTest.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestRtree.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestRtree.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestSort.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestSort.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestAlgorithm.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestAlgorithm.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestScriptEngine.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/TestScriptEngine.h"
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
MESSAGE("Testxxxxxx")
MESSAGE(STATUS ${LOGIC_INTERFACE})
SOURCE_GROUP( Api	FILES ${API})
SOURCE_GROUP( Define FILES ${LOGIC_DEFINE})
SOURCE_GROUP( Public\\Tinyxml FILES ${TINYXML})
SOURCE_GROUP( Public\\2D FILES ${2D})
SOURCE_GROUP( Public\\time FILES ${COMMON_TIME})
SOURCE_GROUP( Public FILES ${PUBLIC})
SOURCE_GROUP( Interface FILES ${LOGIC_INTERFACE})
SOURCE_GROUP( \\ FILES ${TEST})


IF(WIN32)
	SET(THIRD_LIB_DEBUG
	"${THIRD_LIBRARIES_DEBUG_DIR}/DbgHelp.Lib"
	)
	SET(THIRD_LIB_RELEASE
	)
ENDIF(WIN32)

IF(LINUX)

ENDIF(LINUX)

ADD_LIBRARY( Test SHARED
	${TEST}
	${PUBLIC}
	${API}
	${LOGIC_INTERFACE}
	${LOGIC_DEFINE}
	${TINYXML}
	${2D}
	${COMMON_TIME}
)

SET_PROPERTY(TARGET Test PROPERTY FOLDER "Logic")

ADD_DEPENDENCIES(Test ObjectMgr)
ADD_DEPENDENCIES(Test NetGrid)
ADD_DEPENDENCIES(Cluster NodeProto)

TARGET_LINK_LIBRARIES( Test
    ${NODEPROTO_LIBD}
    ${NODEPROTO_LIB}
    ${OBJECT_LIBD}
    ${OBJECT_LIB}
	${KERNEL_LIBD}
	${KERNEL_LIB}
)

