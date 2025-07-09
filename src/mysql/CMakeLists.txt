file(GLOB_RECURSE MYSQL_SOURCES 
    "primary/*.cpp"
    "stmt/*.cpp"
    "*.cpp"
)
file (GLOB_RECURSE MYSQL_HEADERS 
    "../../include/mysql/primary/*.h"
    "../../include/mysql/stmt/*.h"
    "../../include/mysql/*.h"
)
add_library(mysql OBJECT
    ${MYSQL_SOURCES}
    ${MYSQL_HEADERS}
)

target_include_directories(mysql PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)