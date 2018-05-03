cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sre-smtp C CXX)

add_library(smtp-lib src/smtp-lib/smtpserver.h src/smtp-lib/smtpserver.cpp src/smtp-lib/socket.cpp src/smtp-lib/socket.hpp)
set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(smtp-server src/smtp-server/main.cpp)
target_include_directories(smtp-server PRIVATE src)

if(WIN32)
  target_link_libraries(smtp-server wsock32 ws2_32 smtp-lib)
else()
  target_link_libraries(smtp-server smtp-lib)
endif()
