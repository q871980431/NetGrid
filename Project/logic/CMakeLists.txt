SET(LOGIC_INTERFACE_DIR
	"${PROJECT_SOURCE_DIR}/logic/Interface"
)

SET(LOGIC_DEFINE_DDIR
	"${PROJECT_SOURCE_DIR}/logic/Define"
)

SET(LOGIC_DEFINE_DIR
	"${PROJECT_SOURCE_DIR}/logic/Define"
	"${PROJECT_SOURCE_DIR}/logic/Define/Proto"
	"${THIRD_LIBRARIES_INCLUDE_DIR}/google"
)


SET(LOGIC_DEFINE
	"${LOGIC_DEFINE_DDIR}/FrameworkDefine.h"
	"${LOGIC_DEFINE_DDIR}/MemberDefine.h"
	"${LOGIC_DEFINE_DDIR}/MemberDef.h"
	"${LOGIC_DEFINE_DDIR}/Game_tools.h"	
	"${LOGIC_DEFINE_DDIR}/CommonMsgDefine.h"
	"${LOGIC_DEFINE_DDIR}/Proto/test.pb.h"
)

FILE(GLOB LOGIC_INTERFACE "${PROJECT_SOURCE_DIR}/logic/Interface/*.*")

FILE(GLOB LOGIC_DIRS "${PROJECT_SOURCE_DIR}/logic/*")
FOREACH(LOGIC_DIR ${LOGIC_DIRS})
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/AttrGenerator.exe" NOT_1)
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/AttrGenerator.rar" NOT_2)
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/CMakeLists.txt" NOT_3)
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/Define" NOT_4)
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/Interface" NOT_5)
	string( COMPARE NOTEQUAL ${LOGIC_DIR} "${PROJECT_SOURCE_DIR}/logic/MemberGenerator.exe" NOT_6)
	IF(NOT_1 AND NOT_2 AND NOT_3 AND NOT_4 AND NOT_5 AND NOT_6)
		MESSAGE("添加子目录:${LOGIC_DIR}")
		ADD_SUBDIRECTORY(${LOGIC_DIR})
	ENDIF()
ENDFOREACH()
