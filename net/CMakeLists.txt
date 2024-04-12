cmake_minimum_required(VERSION 3.16)

set(NET_SRC
	include/net/socket.h
	src/net/socket.cpp
)

add_library(Net
	STATIC
		${NET_SRC}
)

target_link_libraries(Net
	PUBLIC
		Ws2_32.lib
)

target_include_directories(Net
 PUBLIC
	include
	include/net/
)

include(GenerateExportHeader)
generate_export_header(Net
		EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/net/Exports.h
		EXPORT_MACRO_NAME NET_EXPORT
)
