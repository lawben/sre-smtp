cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sre-smtp C CXX)

add_library(smtp-lib src/smtp-lib/smtpserver.h src/smtp-lib/smtpserver.cpp)
set_target_properties(smtp-lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(smtp-server src/smtp-server/main.cpp)
target_include_directories(smtp-server PRIVATE src)
target_link_libraries(smtp-server smtp-lib)
