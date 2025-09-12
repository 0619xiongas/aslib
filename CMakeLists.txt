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

    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(ASIO_STANDALONE)
    add_compile_definitions(ASIO_SEPARATE_COMPILATION)
endif()

set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    #设置asio路径
    set(ASIO_INCLUDE_DIR "${THIRD_PARTY_DIR}/asio/include")
    set(ASIO_LIB_DIR "${THIRD_PARTY_DIR}/asio/lib")
    
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
    #设置asio路径
    set(ASIO_INCLUDE_DIR "${THIRD_PARTY_DIR}/asio/include")
    
    find_package(MySQL REQUIRED)
    find_package(uv REQUIRED)
endif()

include_directories(${ASIO_INCLUDE_DIR})
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${LIBUV_INCLUDE_DIR})

# 添加各模块
add_subdirectory(code/tools)
add_subdirectory(code/file)
add_subdirectory(code/buffer)
add_subdirectory(code/algorithm)
add_subdirectory(code/time)
add_subdirectory(code/log)
add_subdirectory(code/mysql)
add_subdirectory(code/thread)
add_subdirectory(code/net)
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

link_directories(
    ${ASIO_LIB_DIR}
    ${LIBUV_LIB_DIR}
    ${MYSQL_LIB_DIR}
)
target_link_libraries(aslib PUBLIC 
    asio
    uv
if(WIN32)
    ws2_32
endif()
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

# 设置输出目录
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")
set(LIB_OUTPUT_DIR "${OUTPUT_DIR}/lib")
set(INCLUDE_OUTPUT_DIR "${OUTPUT_DIR}/include")

# 设置静态库输出目录
set_target_properties(aslib PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
)

# 创建输出目录
add_custom_command(TARGET aslib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LIB_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INCLUDE_OUTPUT_DIR}"
    COMMENT "Creating output directories"
)

# 复制所有头文件到include目录，保持目录结构
file(GLOB_RECURSE HEADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/code/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/code/*.hpp"
)

foreach(header_file ${HEADER_FILES})
    # 获取头文件相对于code目录的相对路径
    file(RELATIVE_PATH relative_path "${CMAKE_CURRENT_SOURCE_DIR}/code" "${header_file}")
    # 获取目标目录路径
    get_filename_component(target_dir "${INCLUDE_OUTPUT_DIR}/${relative_path}" DIRECTORY)
    
    # 创建目标目录并复制头文件
    add_custom_command(TARGET aslib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${target_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${header_file}" "${target_dir}/"
        COMMENT "Copying header file: ${relative_path}"
    )
endforeach()