cmake_minimum_required(VERSION 3.16)

add_executable(EchoServer
	main.cpp
)

target_link_libraries(EchoServer
	PRIVATE
		Net
)
