cmake_minimum_required(VERSION 3.10)
project(asioTest)

# if(MSVC)
#     foreach(flag_var
#         CMAKE_C_FLAGS_DEBUG
#         CMAKE_CXX_FLAGS_DEBUG
#         CMAKE_C_FLAGS_DEBUG_INIT
#         CMAKE_CXX_FLAGS_DEBUG_INIT)
#         string(REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
#     endforeach()
# endif()

# 假设 aslib 库已编译好，lib和include目录与Test同级
set(ASLIB_ROOT "${CMAKE_SOURCE_DIR}/../../")
set(ASLIB_INCLUDE "${ASLIB_ROOT}/include")
set(ASLIB_LIB "${ASLIB_ROOT}/lib/Debug")

# Boost 和 libuv 路径（如有需要可调整）
set(BOOST_LIB "${ASLIB_ROOT}/third/boost/lib")
set(BOOST_INCLUDE "${ASLIB_ROOT}/third/boost/include")
set(VLD_LIB "${ASLIB_ROOT}/third/vld/lib/x64")
set(VLD_INCLUDE "${ASLIB_ROOT}/third/vld/include")

file(GLOB NETCPP_PROTO_SRCS "${CMAKE_SOURCE_DIR}/public/netcpp/*.pb.cc")
include_directories(${CMAKE_SOURCE_DIR}/public/netcpp)

include_directories(
    ${ASLIB_INCLUDE}
    ${BOOST_INCLUDE}
    ${VLD_INCLUDE}
)
include_directories(${CMAKE_SOURCE_DIR}/../../third/protobuf/include)

link_directories(
    ${ASLIB_LIB}
    ${BOOST_LIB}
    ${VLD_LIB}
)
link_directories(${CMAKE_SOURCE_DIR}/../../third/protobuf/lib/debug)

# ucServer 可执行文件
add_executable(asioServer
    asioServer/main.cpp
    asioServer/asioTestServer.cpp
    asioServer/asioTestServer.h
    ${NETCPP_PROTO_SRCS}
)

# uvClient 可执行文件
add_executable(asioClient
    asioClient/main.cpp
    asioClient/asioTestClient.cpp
    asioClient/asioTestClient.h
    ${NETCPP_PROTO_SRCS}
)

# 链接 aslib、boost、libuv
target_link_libraries(asioServer
    aslib
    libprotobufd
    vld
)

target_link_libraries(asioClient
    aslib
    libprotobufd
    vld
)

# 可选：设置输出目录
set_target_properties(asioServer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/server")
set_target_properties(asioClient PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/client")

if(MSVC)
    target_compile_options(asioServer PRIVATE $<$<CONFIG:Debug>:/MTd>)
    target_compile_options(asioClient PRIVATE $<$<CONFIG:Debug>:/MTd>)
endif()

