cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

project(
	Net
	VERSION
	0.1
	LANGUAGES
	CXX
)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(Net)
add_subdirectory(EchoServer)
add_subdirectory(EchoClient)
