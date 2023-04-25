#****************************************************************************
#* CMakeLists.txt
#****************************************************************************
cmake_minimum_required (VERSION 3.11)


include_directories(
    ${CMAKE_BINARY_DIR}/zsp_ast/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${debug_mgr_INCDIR}"
    )

link_directories(
	${CMAKE_BINARY_DIR}/antlr4/lib
	${CMAKE_BINARY_DIR}/antlr4/lib64
	)

file(GLOB json_rpc_SRC
	"*.h"
	"*.cpp")
add_library(json-rpc SHARED
	${json_rpc_SRC}
	)

install(TARGETS json-rpc
    DESTINATION lib
    EXPORT json-rpc-targets)
    
set_target_properties(json-rpc PROPERTIES SKIP_BUILD_RPATH TRUE)

