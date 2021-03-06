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

FILE(GLOB bstr_h strings/*.h)
FILE(GLOB dbg_h dbg/*.h)
FILE(GLOB inc_src */src)
FILE(GLOB all_h */src/*.h)

install(FILES ${bstr_h} DESTINATION ${DEST})
install(FILES ${dbg_h} DESTINATION ${DEST})
install(FILES ${all_h} DESTINATION ${DEST})

#openssl
find_package(OpenSSL)

#bstrlib
#@author websnarf
#@git https://github.com/websnarf/bstrlib
FILE(GLOB bstr_src strings/*.c)
add_library(bstring_obj OBJECT ${bstr_src})
add_library(bstring $<TARGET_OBJECTS:bstring_obj>)
target_include_directories(bstring PRIVATE dbg)
target_include_directories(bstring PRIVATE strings)
target_include_directories(bstring PRIVATE "*/src")
install(TARGETS bstring)


# Vector
add_library(vector_obj OBJECT vector/src/vector.c)
add_library(vector $<TARGET_OBJECTS:vector_obj>)
target_include_directories(vector_obj PRIVATE ${inc_src} "dbg/")
add_dependencies(vector bstring)
install(TARGETS vector)

add_executable(vector_tests vector/tests/vector_tests.c vector/tests/munit.c)
target_link_libraries(vector_tests PRIVATE vector)
target_include_directories(vector_tests PRIVATE ${inc_src} "dbg/")
add_test(NAME vector_test
         COMMAND vector_tests)

# Map
add_library(map_obj OBJECT map/src/map.c)
add_library(map $<TARGET_OBJECTS:map_obj>)
target_link_libraries(map PRIVATE vector)
target_include_directories(map_obj PRIVATE ${inc_src} "dbg/")
add_dependencies(map vector)
install(TARGETS map)

add_executable(map_tests map/tests/map_tests.c map/tests/munit.c $<TARGET_OBJECTS:map>)
target_include_directories(map_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(map_tests PRIVATE map vector bstring)
add_test(NAME map_test
         COMMAND map_tests)

# Stack
add_library(stack_obj OBJECT stack/src/stack.c)
add_library(stack $<TARGET_OBJECTS:stack_obj>)
target_link_libraries(stack PRIVATE vector)
target_include_directories(stack_obj PRIVATE ${inc_src} "dbg/")
install(TARGETS stack)
add_dependencies(stack vector)

add_executable(stack_tests stack/tests/stack_tests.c stack/tests/munit.c)
target_include_directories(stack_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(stack_tests PRIVATE stack)
add_test(NAME stack_test
         COMMAND stack_tests)

#queue
add_library(queue_obj OBJECT queue/src/queue.c)
add_library(queue $<TARGET_OBJECTS:queue_obj>)
target_link_libraries(queue PRIVATE vector)
target_include_directories(queue_obj PRIVATE ${inc_src} "dbg/")
install(TARGETS queue)
add_dependencies(queue vector)

add_executable(queue_tests queue/tests/queue_tests.c queue/tests/munit.c)
target_include_directories(queue_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(queue_tests PRIVATE queue)
add_test(NAME queue_test
         COMMAND queue_tests)

#ringbuffer
add_library(ringbuffer_obj OBJECT ringbuffer/src/ringbuffer.c)
add_library(ringbuffer $<TARGET_OBJECTS:ringbuffer_obj>)
target_link_libraries(ringbuffer PRIVATE vector bstring)
target_include_directories(ringbuffer_obj PRIVATE ${inc_src} "dbg/")
install(TARGETS ringbuffer)
add_dependencies(ringbuffer vector)
add_dependencies(ringbuffer bstring)


add_executable(ringbuffer_tests ringbuffer/tests/reingbuffer_tests.c ringbuffer/tests/munit.c ringbuffer/src/ringbuffer.c)
target_include_directories(ringbuffer_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(ringbuffer_tests PRIVATE ringbuffer)
add_test(NAME ringbuffer_test
         COMMAND ringbuffer_tests)

#titree
add_library(tritree_obj OBJECT tritree/src/tritree.c)
add_library(tritree $<TARGET_OBJECTS:tritree_obj> )
target_link_libraries(tritree PRIVATE vector bstring)
target_include_directories(tritree_obj PRIVATE ${inc_src} "dbg/")
install(TARGETS tritree)
add_dependencies(tritree vector)

add_executable(tritree_tests tritree/tests/tritree_tests.c tritree/tests/munit.c)
target_include_directories(tritree_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(tritree_tests tritree)
add_test(NAME tritree_test
         COMMAND tritree_tests)

#bintree
add_library(bintree_obj OBJECT bintree/src/bintree.c)
add_library(bintree $<TARGET_OBJECTS:bintree_obj>)
target_include_directories(bintree_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(bintree PRIVATE vector bstring)
install(TARGETS bintree)
add_dependencies(bintree vector)

add_executable(bintree_tests bintree/tests/bintree_tests.c bintree/tests/munit.c)
target_include_directories(bintree_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(bintree_tests PRIVATE bintree)
add_test(NAME bintree_test
         COMMAND bintree_tests)

#fileio
add_library(fileio_obj OBJECT io/src/fileio.c)
add_library(fileio $<TARGET_OBJECTS:fileio_obj>)
target_include_directories(fileio_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(fileio PRIVATE bstring ringbuffer)

install(TARGETS fileio)
add_dependencies(fileio ringbuffer)

add_executable(io_tests io/tests/fileio_test.c io/tests/munit.c)
target_include_directories(io_tests PRIVATE ${inc_src} "dbg/")
target_link_libraries(io_tests PRIVATE fileio)
add_test(NAME io_tests
         COMMAND io_tests)

#io SSL
add_library(fileio_ssl_obj OBJECT io/src/fileio.c)
target_compile_definitions(fileio_ssl_obj PRIVATE _WITH_OPEN_SSL)
add_library(fileio_ssl $<TARGET_OBJECTS:fileio_ssl_obj>)
target_include_directories(fileio_ssl_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(fileio_ssl PRIVATE ringbuffer bstring OpenSSL::SSL)

install(TARGETS fileio_ssl)
add_dependencies(fileio_ssl ringbuffer)

#argparse
add_library(argparse_obj OBJECT argparse/src/bstrlib.c argparse/src/argparse.c)
add_library(argparse  $<TARGET_OBJECTS:argparse_obj>)
target_include_directories(argparse_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(argparse PRIVATE tritree bstring vector)
install(TARGETS argparse)
add_dependencies(argparse vector)
add_dependencies(argparse tritree)

add_executable(argparse_tests argparse/tests/argparse_tests.c argparse/tests/munit.c)
target_include_directories(argparse_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(argparse_tests PRIVATE argparse)
add_test(NAME argparse_test
         COMMAND argparse_tests)

#sockets
add_library(soc_obj OBJECT sockets/src/soc.c)
add_library(soc $<TARGET_OBJECTS:soc_obj>)
target_include_directories(soc_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(soc PRIVATE fileio_ssl)
install(TARGETS soc)
add_dependencies(soc fileio)
add_dependencies(soc ringbuffer)

add_executable(soc_tests sockets/tests/socket_tests.c sockets/tests/munit.c)
target_include_directories(soc_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(soc_tests PRIVATE soc fileio_ssl bstring)
add_test(NAME soc_tests
         COMMAND soc_tests)

#request
add_library(req_obj OBJECT webserver/src/request.c)
add_library(req $<TARGET_OBJECTS:req_obj>)
target_include_directories(req_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(req PRIVATE fileio map)
install(TARGETS req)
add_dependencies(req fileio)
add_dependencies(req soc)
add_dependencies(req tritree)

add_executable(req_tests webserver/tests/request_tests.c sockets/tests/munit.c)
target_include_directories(req_tests PRIVATE ${inc_src} "dbg/")
target_link_libraries(req_tests PRIVATE req)
add_test(NAME req_tests
         COMMAND req_tests)
#ssl
add_library(soc_ssl_obj OBJECT sockets/src/ssl.c)
add_library(soc_ssl $<TARGET_OBJECTS:soc_ssl_obj>)
target_include_directories(soc_ssl_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(soc_ssl PRIVATE soc fileio_ssl toml)
install(TARGETS soc_ssl)

add_dependencies(soc fileio)
add_dependencies(soc ringbuffer)
add_dependencies(soc_ssl fileio)

add_executable(soc_ssl_tests sockets/tests/ssl_tests.c sockets/tests/munit.c)
target_include_directories(soc_ssl_tests BEFORE PRIVATE ${inc_src} "dbg/")
target_link_libraries(soc_ssl_tests PRIVATE soc_ssl pthread)

#add_test(NAME soc_ssl_tests
  #       COMMAND soc_ssl_tests)

#web
add_library(web_obj OBJECT webserver/src/webserver.c webserver/src/request.c)
add_library(web $<TARGET_OBJECTS:web_obj>)
target_include_directories(web_obj PRIVATE ${inc_src} "dbg/")
target_link_libraries(web PRIVATE pthread fileio_ssl soc_ssl req tritree)

install(TARGETS web)
add_dependencies(web fileio)
add_dependencies(web soc)
add_dependencies(web tritree)

add_executable(web_tests webserver/tests/webserver_test.c webserver/tests/munit.c)
target_include_directories(web_tests PRIVATE ${inc_src} "dbg/")
target_link_libraries(web_tests PRIVATE web)
add_test(NAME web_tests
         COMMAND web_tests)

#genssl
add_library(genssl_obj OBJECT genssl/src/genssl.c)
target_include_directories(genssl_obj PRIVATE ${inc_src} "dbg/")

add_library(genssl $<TARGET_OBJECTS:genssl_obj>)
target_link_libraries(genssl PRIVATE fileio_ssl soc_ssl)

install(TARGETS genssl)
add_dependencies(genssl fileio_ssl)
add_dependencies(genssl soc_ssl)

add_executable(genssl_tests genssl/test/genssl_tests.c genssl/test/munit.c)
target_include_directories(genssl_tests PRIVATE ${inc_src} "dbg/")
target_link_libraries(genssl_tests PRIVATE genssl)
add_test(NAME genssl_tests
         COMMAND genssl_tests)
