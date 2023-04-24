cmake_minimum_required(VERSION 3.10)
 
project(CAAA VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

set(CPACK_PACKAGE_VENDOR "oonray")
set(CPACK_PACKAGE_CONTACT "alexanderbjornsrud@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "oonray")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEB_COMPONENT_INSTALL YES)
include(CPack)

set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME} CACHE STRING "caaa")
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})


#HFILES
set(DEST "include/caaa")

FILE(GLOB bstr_h inc/bstring/*.h)
install(FILES ${bstr_h} DESTINATION ${DEST})

FILE(GLOB dbg_h dbg/*.h)
install(FILES ${dbg_h} DESTINATION ${DEST})

FILE(GLOB all_h */src/*.h)
install(FILES ${all_h} DESTINATION ${DEST})

FILE(GLOB inc_src */src)

include_directories(${inc_src})
include_directories("dbg/")
include_directories("inc/")
include_directories("strings/")


#munit
FILE(GLOB munit inc/munit/*.c)
add_library(ca_munit_obj OBJECT inc/munit/munit.c)
add_library(ca_munit $<TARGET_OBJECTS:ca_munit_obj>)

#bstrlib
#@author websnarf
#@git https://github.com/websnarf/bstrlib
FILE(GLOB bstr_src inc/bstring/*.c)
add_library(bstring_obj OBJECT ${bstr_src})
add_library(ca_string $<TARGET_OBJECTS:bstring_obj>)
install(TARGETS ca_string)


# Vector
add_library(vector_obj OBJECT vector/src/ca_vector.c)
add_library(ca_vector $<TARGET_OBJECTS:vector_obj>)
install(TARGETS ca_vector)

add_executable(vector_tests vector/tests/vector_tests.c)
target_link_libraries(vector_tests PRIVATE ca_vector ca_munit)
add_test(NAME vector_test
         COMMAND vector_tests)

# Map
add_library(map_obj OBJECT map/src/ca_map.c)
add_library(ca_map $<TARGET_OBJECTS:map_obj>)
target_link_libraries(ca_map PRIVATE ca_vector)
add_dependencies(ca_map ca_vector)
install(TARGETS ca_map)

add_executable(map_tests map/tests/map_tests.c)
target_link_libraries(map_tests PRIVATE ca_map ca_vector ca_string ca_munit)
add_test(NAME map_test
         COMMAND map_tests)

# Stack
add_library(stack_obj OBJECT stack/src/ca_stack.c)
add_library(ca_stack $<TARGET_OBJECTS:stack_obj>)
target_link_libraries(ca_stack PRIVATE ca_vector)
install(TARGETS ca_stack)
add_dependencies(ca_stack ca_vector)

add_executable(stack_tests stack/tests/stack_tests.c)
target_link_libraries(stack_tests PRIVATE ca_stack ca_vector ca_munit)
add_test(NAME stack_test
         COMMAND stack_tests)

#queue
add_library(queue_obj OBJECT queue/src/ca_queue.c)
add_library(ca_queue $<TARGET_OBJECTS:queue_obj>)
target_link_libraries(ca_queue PRIVATE ca_vector)
install(TARGETS ca_queue)
add_dependencies(ca_queue ca_vector)

add_executable(queue_tests queue/tests/queue_tests.c)
target_link_libraries(queue_tests PRIVATE ca_queue ca_munit)
add_test(NAME queue_test
         COMMAND queue_tests)

#ringbuffer
add_library(ringbuffer_obj OBJECT ringbuffer/src/ca_ringbuffer.c)
add_library(ca_ringbuffer $<TARGET_OBJECTS:ringbuffer_obj>)
target_link_libraries(ca_ringbuffer PRIVATE ca_vector ca_string)
install(TARGETS ca_ringbuffer)
add_dependencies(ca_ringbuffer ca_vector ca_string)


add_executable(ringbuffer_tests ringbuffer/tests/reingbuffer_tests.c)
target_link_libraries(ringbuffer_tests PRIVATE ca_ringbuffer ca_munit)
add_test(NAME ringbuffer_test
         COMMAND ringbuffer_tests)

#titree
add_library(tritree_obj OBJECT tritree/src/ca_tritree.c)
add_library(ca_tritree $<TARGET_OBJECTS:tritree_obj> )
target_link_libraries(ca_tritree PRIVATE ca_vector ca_string)
install(TARGETS ca_tritree)
add_dependencies(ca_tritree ca_vector)

add_executable(tritree_tests tritree/tests/tritree_tests.c)
target_link_libraries(tritree_tests ca_tritree ca_munit)
add_test(NAME tritree_test
         COMMAND tritree_tests)

#bintree
add_library(bintree_obj OBJECT bintree/src/ca_bintree.c)
add_library(ca_bintree $<TARGET_OBJECTS:bintree_obj>)
target_link_libraries(ca_bintree PRIVATE ca_vector ca_string)
install(TARGETS ca_bintree)
add_dependencies(ca_bintree ca_vector)

add_executable(bintree_tests bintree/tests/bintree_tests.c)
target_link_libraries(bintree_tests PRIVATE ca_bintree ca_munit)
add_test(NAME bintree_test
         COMMAND bintree_tests)

#fileio
add_library(fileio_obj OBJECT io/src/ca_fileio.c)
add_library(ca_fileio $<TARGET_OBJECTS:fileio_obj>)
target_link_libraries(ca_fileio PRIVATE ca_string ca_ringbuffer ca_vector)

install(TARGETS ca_fileio)
add_dependencies(ca_fileio ca_ringbuffer)

add_executable(io_tests io/tests/fileio_test.c)
target_link_libraries(io_tests PRIVATE ca_fileio ca_string ca_munit)
add_test(NAME io_tests
         COMMAND io_tests)

#io SSL
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   set(OSX TRUE)
   set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@1.1")
endif()

#openssl
find_package(OpenSSL)
if(${OPENSSL_FOUND})
message("openssl: ${OPENSSL_FOUND}")
message("openssl-include: ${OPENSSL_INCLUDE_DIR}")

add_library(fileio_ssl_obj OBJECT io/src/ca_fileio.c io/src/ca_fileio_ssl.c)
target_include_directories(fileio_ssl_obj PRIVATE ${OPENSSL_INCLUDE_DIR})
target_compile_definitions(fileio_ssl_obj PRIVATE _CA_WITH_OPEN_SSL)

add_library(ca_fileio_ssl $<TARGET_OBJECTS:fileio_ssl_obj>)
target_link_libraries(ca_fileio_ssl PRIVATE ca_ringbuffer ca_string OpenSSL::SSL)


install(TARGETS ca_fileio_ssl)
add_dependencies(ca_fileio_ssl ca_ringbuffer)
endif()

##argparse
#use optargs
# man 3 optargs

#sockets
#add_library(soc_obj OBJECT sockets/src/soc.c)
#add_library(soc $<TARGET_OBJECTS:soc_obj>)
#target_link_libraries(soc PRIVATE fileio_ssl)
#install(TARGETS soc)
#add_dependencies(soc fileio)
#add_dependencies(soc ringbuffer)

#add_executable(soc_tests sockets/tests/socket_tests.c sockets/tests/munit.c)
#target_link_libraries(soc_tests PRIVATE soc fileio_ssl ca_string)
#add_test(NAME soc_tests
#         COMMAND soc_tests)

#  #request
# use libcurl

#  #web
#  use mongoose.h
