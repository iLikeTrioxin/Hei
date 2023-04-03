cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Hei)

if(HEI_BUILD_SERVER)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Server.cmake)
    set(HEI_BUILD_NAME "Server")
else()
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Client.cmake)
    set(HEI_BUILD_NAME "Client")
endif()

#File(GLOB_RECURSE   SOURCE_FILES Hei/Source/*.cpp )
#File(GLOB_RECURSE   HEADER_FILES Hei/Source/*.h   )
#File(GLOB_RECURSE RESOURCE_FILES Hei/Resources/*.*)

#list(FILTER SOURCE_FILES EXCLUDE REGEX ".*Hei/Source/Server/Main.cpp")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -lstdc++fs")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -lstdc++fs")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -S -fverbose-asm")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_DEBUG} -S -fverbose-asm")

#set( CMAKE_APPLE_SILICON_PROCESSOR         ON  CACHE BOOL "xxx" FORCE )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Hei FILES ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

# for entt
add_compile_definitions(NOMINMAX)

add_subdirectory(Deps)




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${HEI_BUILD_NAME}>)

file(COPY Hei/Resources DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${HEI_BUILD_NAME})

add_executable(Hei ${SOURCE_FILES})
target_link_libraries(Hei PUBLIC PetrolEngine ${STATIC_LIBS})
target_include_directories(Hei PUBLIC ${PETROL_ENGINE_INCLUDE})

if(UNIX)
    target_link_libraries (Hei PUBLIC pthread dl)
    target_compile_options(Hei PUBLIC -Wl,-rpath -Wl,.)
endif()

