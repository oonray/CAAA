cmake_minimum_required(VERSION 3.10)
project(CAAA VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

set(CPACK_PACKAGE_VENDOR "oonray")
set(CPACK_PACKAGE_CONTACT "alexanderbjornsrud@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "oonray")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LISENCE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEB_COMPONENT_INSTALL YES)
include(CPack)

set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME} CACHE STRING "caaa")
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/caaa")

include_directories(BEFORE dbg)
include_directories(BEFORE strings)
include_directories(BEFORE vector/src)
include_directories(BEFORE ringbuffer/src)

#HFILES
set(DEST "include/caaa")

FILE(GLOB bstr_h strings/*.h)
FILE(GLOB dbg_h dbg/*.h)
install(FILES stack/src/stack.h DESTINATION ${DEST})
install(FILES queue/src/queue.h DESTINATION ${DEST})
install(FILES map/src/map.h DESTINATION ${DEST})
install(FILES vector/src/vector.h DESTINATION ${DEST})
install(FILES ringbuffer/src/ringbuffer.h DESTINATION ${DEST})
install(FILES tritree/src/tritree.h DESTINATION ${DEST})
install(FILES ${bstr_h} DESTINATION ${DEST}/bstring)
install(FILES ${dbg_h} DESTINATION ${DEST})
install(FILES bintree/src/bintree.h DESTINATION ${DEST})
install(FILES io/src/fileio.h DESTINATION ${DEST})
install(FILES argparse/src/argparse.h DESTINATION ${DEST})


#bstrlib
#@author websnarf
#@git https://github.com/websnarf/bstrlib
FILE(GLOB bstr_src strings/*.c)
add_library(bstring_obj OBJECT ${bstr_src})
add_library(bstring $<TARGET_OBJECTS:bstring_obj>)
install(TARGETS bstring)


# Vector
add_library(vector_obj OBJECT vector/src/vector.c)
add_library(vector $<TARGET_OBJECTS:vector_obj>)
install(TARGETS vector)

add_executable(vector_tests vector/tests/vector_tests.c vector/tests/munit.c)
target_link_libraries(vector_tests PRIVATE vector)
add_test(NAME vector_test
         COMMAND vector_tests)

# Map
add_library(map_obj OBJECT map/src/map.c)
add_library(map $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:map_obj>)
install(TARGETS map)

add_executable(map_tests map/tests/map_tests.c map/tests/munit.c $<TARGET_OBJECTS:map>)
target_include_directories(map_tests BEFORE PRIVATE map/src)
target_link_libraries(map_tests PRIVATE map vector bstring)
add_test(NAME map_test
         COMMAND map_tests)

# Stack
add_library(stack_obj OBJECT stack/src/stack.c)
add_library(stack $<TARGET_OBJECTS:vector> $<TARGET_OBJECTS:stack_obj>)
install(TARGETS stack)

add_executable(stack_tests stack/tests/stack_tests.c stack/tests/munit.c)
target_include_directories(stack_tests BEFORE PRIVATE stack/src)
target_link_libraries(stack_tests PRIVATE stack vector)
add_test(NAME stack_test
         COMMAND stack_tests)

#queue
add_library(queue_obj OBJECT queue/src/queue.c)
add_library(queue $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:queue_obj>)
install(TARGETS queue)

add_executable(queue_tests queue/tests/queue_tests.c queue/tests/munit.c)
target_include_directories(queue_tests BEFORE PRIVATE queue/src)
target_link_libraries(queue_tests PRIVATE queue vector)
add_test(NAME queue_test
         COMMAND queue_tests)

#ringbuffer
add_library(ringbuffer_obj OBJECT ringbuffer/src/ringbuffer.c)
add_library(ringbuffer $<TARGET_OBJECTS:vector> $<TARGET_OBJECTS:ringbuffer_obj> $<TARGET_OBJECTS:bstring>)
install(TARGETS ringbuffer)


add_executable(ringbuffer_tests ringbuffer/tests/reingbuffer_tests.c ringbuffer/tests/munit.c ringbuffer/src/ringbuffer.c)
target_include_directories(ringbuffer_tests BEFORE PRIVATE ringbuffer/src)
target_link_libraries(ringbuffer_tests PRIVATE ringbuffer vector bstring)
add_test(NAME ringbuffer_test
         COMMAND ringbuffer_tests)

#titree
add_library(tritree_obj OBJECT tritree/src/tritree.c)
add_library(tritree $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:tritree_obj> $<TARGET_OBJECTS:tritree_obj> $<TARGET_OBJECTS:bstring_obj>)
install(TARGETS tritree)

add_executable(tritree_tests tritree/tests/tritree_tests.c tritree/tests/munit.c)
target_include_directories(tritree_tests BEFORE PRIVATE tritree/src)
target_link_libraries(tritree_tests tritree)
add_test(NAME tritree_test
         COMMAND tritree_tests)

#bintree
add_library(bintree_obj OBJECT bintree/src/bintree.c)
add_library(bintree $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:bintree_obj>)
install(TARGETS bintree)

add_executable(bintree_tests bintree/tests/bintree_tests.c bintree/tests/munit.c)
target_include_directories(bintree_tests BEFORE PRIVATE bintree/src)
target_link_libraries(bintree_tests PRIVATE bintree vector bstring)
add_test(NAME bintree_test
         COMMAND bintree_tests)

#fileio
add_library(fileio_obj OBJECT io/src/fileio.c)
add_library(fileio $<TARGET_OBJECTS:ringbuffer_obj> $<TARGET_OBJECTS:fileio_obj>)
install(TARGETS fileio)

add_executable(io_tests io/tests/fileio_test.c io/tests/munit.c)
target_include_directories(io_tests BEFORE PRIVATE io/src)
target_link_libraries(io_tests PRIVATE fileio ringbuffer bstring)
add_test(NAME io_tests
         COMMAND io_tests)

#argparse
add_library(argparse_obj OBJECT argparse/src/bstrlib.c argparse/src/argparse.c)
add_library(argparse $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:tritree_obj> $<TARGET_OBJECTS:argparse_obj>)
install(TARGETS argparse)

add_executable(argparse_tests argparse/tests/argparse_tests.c argparse/tests/munit.c)
target_include_directories(argparse_tests BEFORE PRIVATE argparse/src)
target_include_directories(argparse_tests BEFORE PRIVATE tritree/src)
target_link_libraries(argparse_tests PRIVATE argparse)

add_test(NAME argparse_test
         COMMAND argparse_tests)

#sockets
add_library(soc_obj OBJECT sockets/src/soc.c)
add_library(soc $<TARGET_OBJECTS:soc_obj> $<TARGET_OBJECTS:fileio_obj> $<TARGET_OBJECTS:ringbuffer_obj>)
install(TARGETS soc)

add_executable(soc_tests sockets/tests/socket_tests.c sockets/tests/munit.c)
target_include_directories(soc_tests BEFORE PRIVATE sockets/src)
target_link_libraries(soc_tests PRIVATE fileio soc bstring)
add_test(NAME soc_tests
         COMMAND soc_tests)
