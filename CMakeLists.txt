cmake_minimum_required(VERSION 3.10)
project(aslib LANGUAGES CXX)

if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_C_FLAGS_DEBUG_INIT
        CMAKE_CXX_FLAGS_DEBUG_INIT)
        string(REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    #设置boost路径
    set(BOOST_ROOT "${THIRD_PARTY_DIR}/boost")
    set(Boost_NO_SYSTEM_PATHS ON)
    set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include")
    set(Boost_LIBRARY_DIR "${BOOST_ROOT}/lib")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    #设置mysql路径
    set(MYSQL_INCLUDE_DIR "${THIRD_PARTY_DIR}/mysql/include")
    set(MYSQL_LIB_DIR "${THIRD_PARTY_DIR}/mysql/lib")

    #设置libuv路径
    set(LIBUV_INCLUDE_DIR "${THIRD_PARTY_DIR}/libuv/include")
    set(LIBUV_LIB_DIR "${THIRD_PARTY_DIR}/libuv/lib")

    find_library(MYSQL_LIB 
    NAMES libmysql mysql 
    PATHS ${MYSQL_LIB_DIR} 
    NO_DEFAULT_PATH
    )

    find_library(MYSQL_LIB 
    NAMES libmysqld mysql 
    PATHS ${MYSQL_LIB_DIR} 
    NO_DEFAULT_PATH
    )
else()
    #boost
    set(Boost_USE_STATIC_LIBS ON)
    find_package(MySQL REQUIRED)
    find_package(uv REQUIRED)
endif()

find_package(Boost REQUIRED COMPONENTS filesystem system thread)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${LIBUV_INCLUDE_DIR})

# 添加各模块
add_subdirectory(src/file)
add_subdirectory(src/buffer)
add_subdirectory(src/algorithm)
add_subdirectory(src/time)
add_subdirectory(src/log)
add_subdirectory(src/mysql)
add_subdirectory(src/thread)
add_subdirectory(src/tools)
add_subdirectory(src/net)
# 你有其他模块就继续 add_subdirectory(src/xxx)

# 汇总所有 OBJECT 库，生成最终静态库
add_library(aslib STATIC
    $<TARGET_OBJECTS:file>
    $<TARGET_OBJECTS:buffer>
    $<TARGET_OBJECTS:algorithm>
    $<TARGET_OBJECTS:time>
    $<TARGET_OBJECTS:log>
    $<TARGET_OBJECTS:mysql>
    $<TARGET_OBJECTS:thread>
    $<TARGET_OBJECTS:tools>  # 如果有 tools 模块
    $<TARGET_OBJECTS:net>
    # 其他模块继续添加
)

target_include_directories(aslib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
link_directories(
    ${Boost_LIBRARY_DIR}
    ${LIBUV_LIB_DIR}
)
target_link_libraries(aslib PUBLIC 
    boost_filesystem
    boost_system
    boost_thread
    uv
)
if(WIN32)
    target_link_directories(aslib PUBLIC ${MYSQL_LIB_DIR})
    target_link_libraries(aslib PUBLIC ${MYSQL_LIBRARIES})
else()
    target_link_libraries(aslib PUBLIC MySQL::MySQL)
endif()

if(MSVC)
    target_compile_options(aslib PRIVATE /W4)
else()
    target_compile_options(aslib PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    set_target_properties(aslib PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    )
endif()