cmake_minimum_required(VERSION 3.16)

project(Hei)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(USE_ADDONS Assimp Freetype OpenAL OpenGL GLFW Static SSD1306 ENet Bullet)
#set(USE_ADDONS *)

File(GLOB_RECURSE   SOURCE_FILES Hei/Source/*.cpp )
File(GLOB_RECURSE   HEADER_FILES Hei/Source/*.h   )
File(GLOB_RECURSE RESOURCE_FILES Hei/Resources/*.*)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -lstdc++fs")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -lstdc++fs")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -S -fverbose-asm")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_DEBUG} -S -fverbose-asm")

#message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")
#set( CMAKE_APPLE_SILICON_PROCESSOR         ON  CACHE BOOL "xxx" FORCE )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Hei FILES ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

add_subdirectory(Deps)

add_executable(Hei ${SOURCE_FILES})
message(STATUS "sdf: ${STATIC_LIBS}")
target_link_libraries(Hei PUBLIC PetrolEngine ${STATIC_LIBS})
target_include_directories(Hei PUBLIC ${PETROL_ENGINE_INCLUDE})

if(UNIX)

target_link_libraries (Hei PUBLIC pthread dl)
target_compile_options(Hei PUBLIC -Wl,-rpath -Wl,.)

endif()
