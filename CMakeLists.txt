#
# Copyright © Dougie Lawson 2020, All rights reserved.
#

cmake_minimum_required(VERSION 3.0)

project(Zappi_API C)

add_executable(zparse parseZ.c parseJ.c parseD.c)
add_executable(dailyev dailyEV.c)
add_executable(getup getUP.c)

find_library(lib_m m)
find_library(lib_curl curl)
find_library(lib_config config)
find_library(lib_mariadb mariadb)
find_library(lib_json-c json-c)
find_library(lib_pcre pcre)

list(APPEND EXTRA_INCLUDES "/usr/local/include/json")

include_directories("/usr/include/mariadb/mysql")
include_directories("/usr/include/mariadb")

target_link_libraries(zparse ${lib_m} ${lib_curl} ${lib_config} ${lib_mariadb} ${lib_json-c})
target_link_libraries(getup ${lib_pcre} ${lib_curl} ${lib_config})
target_link_libraries(dailyev ${lib_mariadb} ${lib_config})

