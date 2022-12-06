cmake_minimum_required(VERSION 3.16)

project(Hei)

set(CMAKE_CXX_STANDARD 17)

File(GLOB_RECURSE   SOURCE_FILES Hei/Source/*.cpp )
File(GLOB_RECURSE   HEADER_FILES Hei/Source/*.h   )
File(GLOB_RECURSE RESOURCE_FILES Hei/Resources/*.*)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
#set( CMAKE_APPLE_SILICON_PROCESSOR         ON  CACHE BOOL "xxx" FORCE )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Hei FILES ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

add_subdirectory(Deps)

add_executable(Hei ${SOURCE_FILES})
target_link_libraries(Hei PUBLIC Engine)
target_include_directories(Hei PUBLIC ${DEPENDENCY_INCLUDE_DIR})

if(UNIX)

target_link_libraries (Hei PUBLIC pthread dl)
target_compile_options(Hei PUBLIC -Wl,-rpath -Wl,.)

endif()
