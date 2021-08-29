cmake_minimum_required(VERSION 3.10)
project(CAAA VERSION 1.0)

include_directories(BEFORE dbg)
include_directories(BEFORE vector/src)

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

# Map
add_library(map_obj OBJECT map/src/map.c)
add_library(map $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:map_obj>)
install(TARGETS map)

# Stack
add_library(stack_obj OBJECT stack/src/stack.c)
add_library(stack $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:stack_obj>)
install(TARGETS stack)

#queue
add_library(queue_obj OBJECT queue/src/queue.c)
add_library(queue $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:queue_obj>)
install(TARGETS queue)

#ringbuffer
add_library(ringbuffer_obj OBJECT ringbuffer/src/ringbuffer.c)
add_library(ringbuffer $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:ringbuffer_obj>)
install(TARGETS ringbuffer)

#titree
add_library(tritree_obj OBJECT tritree/src/tritree.c)
add_library(tritree $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:tritree_obj>)
install(TARGETS tritree)

#bintree
add_library(bintree_obj OBJECT bintree/src/bintree.c)
add_library(bintree $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:bintree_obj>)
install(TARGETS bintree)

#fileio
add_library(fileio_obj OBJECT io/src/fileio.c)
add_library(fileio $<TARGET_OBJECTS:vector_obj> $<TARGET_OBJECTS:fileio_obj>)
install(TARGETS fileio)

if (${UNIX})
  set(DEST "include/c")
else()
  set(DEST "c")
endif (${UNIX})


FILE(GLOB bstr_h string/*.h)
install(FILES stack/src/stack.h DESTINATION ${DEST})
install(FILES queue/src/queue.h DESTINATION ${DEST})
install(FILES map/src/map.h DESTINATION ${DEST})
install(FILES vector/src/vector.h DESTINATION ${DEST})
install(FILES ringbuffer/src/ringbuffer.h DESTINATION ${DEST})
install(FILES tritree/src/tritree.h DESTINATION ${DEST})
install(FILES ${bstr_h} DESTINATION ${DEST}"/bstr")
install(FILES bintree/src/bintree.h DESTINATION ${DEST})
install(FILES io/src/fileio.h DESTINATION ${DEST})
