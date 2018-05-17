cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(sre-smtp C CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(
        smtp-lib

        src/smtp-lib/mail_parser.hpp
        src/smtp-lib/mail_parser.cpp
        src/smtp-lib/raw_socket.hpp
        src/smtp-lib/raw_socket.cpp
        src/smtp-lib/socket.cpp
        src/smtp-lib/socket.hpp
        src/smtp-lib/mail_receiver.cpp
        src/smtp-lib/mail_receiver.hpp
        src/smtp-lib/smtp_server.cpp
        src/smtp-lib/smtp_server.hpp
        src/smtp-lib/connection.cpp
        src/smtp-lib/connection.hpp
        src/smtp-lib/utils.hpp)

set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET smtp-lib PROPERTY CXX_STANDARD 17)
target_include_directories(smtp-lib PRIVATE src/smtp-lib)

if(MSVC)
	target_link_libraries(smtp-lib wsock32 ws2_32)
else()
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(smtp-lib Threads::Threads)

    set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -pedantic -Werror -Wno-unused-parameter")

    # Make sure clang actually uses libc++ and noch gnu libstd++
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
    endif()
endif()

add_executable(
		smtp-lib-test 
		
		src/smtp-lib-test/acceptance_test.cpp
		src/smtp-lib-test/connection_test.cpp
		src/smtp-lib-test/mail_parser_test.cpp
		src/smtp-lib-test/raw_socket_test.cpp
		src/smtp-lib-test/server_test.cpp
		src/smtp-lib-test/socket_listener_test.cpp
		src/smtp-lib-test/test.cpp)
set_property(TARGET smtp-lib-test PROPERTY CXX_STANDARD 17)
target_include_directories(smtp-lib-test PRIVATE catch src)
target_link_libraries(smtp-lib-test smtp-lib)


add_executable(smtp-server src/smtp-server/main.cpp)
set_property(TARGET smtp-server PROPERTY CXX_STANDARD 17)
target_include_directories(smtp-server PRIVATE src)
target_link_libraries(smtp-server smtp-lib)
