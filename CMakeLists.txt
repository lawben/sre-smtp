cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sre-smtp C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(smtp-lib src/smtp-lib/smtpserver.h)
set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(smtp-server src/smtp-server/main.cpp)
target_link_libraries(smtp-server smtp-lib)
