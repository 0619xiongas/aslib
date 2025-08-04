cmake_minimum_required(VERSION 3.10)
project(mysql_test)

if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_C_FLAGS_DEBUG_INIT
        CMAKE_CXX_FLAGS_DEBUG_INIT)
        string(REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

set(ASLIB_ROOT "${CMAKE_SOURCE_DIR}/../..")
set(ASLIB_INCLUDE "${ASLIB_ROOT}/include")
set(ASLIB_LIB "${ASLIB_ROOT}/lib")

set(BOOST_INCLUDE "${ASLIB_ROOT}/third/boost/include")
set(BOOST_LIB "${ASLIB_ROOT}/third/boost/lib")

set(MYSQL_LIB "${ASLIB_ROOT}/third/mysql/lib")
set(MYSQL_INCLUDE "${ASLIB_ROOT}/third/mysql/include")

set(CJSON_LIB "${ASLIB_ROOT}/third/cJSON/lib")
set(CJSON_INCLUDE "${ASLIB_ROOT}/third/cJSON/include")

set(VLD_LIB "${ASLIB_ROOT}/third/vld/lib/x64")
set(VLD_INCLUDE "${ASLIB_ROOT}/third/vld/include")

include_directories(
    ${ASLIB_INCLUDE}
    ${BOOST_INCLUDE}
    ${MYSQL_INCLUDE}
    ${CJSON_INCLUDE}
    ${VLD_INCLUDE}
)

link_directories(
    ${ASLIB_LIB}
    ${BOOST_LIB}
    ${MYSQL_LIB}
    ${CJSON_LIB}
    ${VLD_LIB}
)

add_executable(mysql_test
    main.cpp
    DBStmtThread.cpp
    DBStmtThread.h
    DBStmtThreadGroup.cpp
    DBStmtThreadGroup.h
    SQLProducer.cpp
    SQLProducer.h
)

target_link_libraries(mysql_test 
    aslib
    libmysql
    cJSON
    vld
)
set_target_properties(mysql_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/mysql_test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/mysql_test"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin/mysql_test"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/bin/mysql_test"
)


if(WIN32)
    add_custom_command(TARGET mysql_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MYSQL_LIB}/libmysql.dll"
            "$<TARGET_FILE_DIR:mysql_test>/libmysql.dll"
    )
endif()