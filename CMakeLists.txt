cmake_minimum_required(VERSION 3.13)
project(project_web_server C)

set(CMAKE_C_STANDARD 11)

add_executable(project_web_server webserver.c socklib.c socklib.h)
