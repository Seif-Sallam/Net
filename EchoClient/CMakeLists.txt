cmake_minimum_required(VERSION 3.16)

add_executable(EchoClient
	main.cpp
)

target_link_libraries(EchoClient
	PRIVATE
		Net
)
