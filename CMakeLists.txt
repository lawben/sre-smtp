cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(sre-smtp C CXX)

add_library(
        smtp-lib
        src/smtp-lib/smtpserver.h
        src/smtp-lib/smtpserver.cpp
        src/smtp-lib/socket.cpp
        src/smtp-lib/socket.hpp
        src/smtp-lib/mailparser.h
        src/smtp-lib/mailparser.cpp
		src/smtp-lib/rawsocket.hpp
		src/smtp-lib/rawsocket.cpp
)
set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET smtp-lib PROPERTY CXX_STANDARD 17)
if(WIN32)
  target_link_libraries(smtp-lib wsock32 ws2_32)
endif()

add_executable(smtp-server src/smtp-server/main.cpp)
set_property(TARGET smtp-server PROPERTY CXX_STANDARD 17)
target_include_directories(smtp-server PRIVATE src)
target_link_libraries(smtp-server smtp-lib)


