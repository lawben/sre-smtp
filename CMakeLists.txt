cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
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
        src/smtp-lib/smtp_handler.cpp
        src/smtp-lib/smtp_handler.hpp
        src/smtp-lib/smtp_server.cpp
        src/smtp-lib/smtp_server.hpp
        src/smtp-lib/connection.cpp
        src/smtp-lib/connection.hpp
        src/smtp-lib/utils.hpp)

# Global flags and include directories
add_compile_options(-std=c++17 -pthread -Wall -Wextra -pedantic -Werror -Wno-unused-parameter)

set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET smtp-lib PROPERTY CXX_STANDARD 17)
if(WIN32)
  target_link_libraries(smtp-lib wsock32 ws2_32)
endif()

add_executable(smtp-server src/smtp-server/main.cpp)
set_property(TARGET smtp-server PROPERTY CXX_STANDARD 17)
target_include_directories(smtp-server PRIVATE src)
target_link_libraries(smtp-server smtp-lib)


