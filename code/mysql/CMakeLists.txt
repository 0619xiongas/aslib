file(GLOB_RECURSE MYSQL_SOURCES 
    "primary/*.cpp"
    "stmt/*.cpp"
    "*.cpp"
)
file (GLOB_RECURSE MYSQL_HEADERS 
    "primary/*.h"
    "stmt/*.h"
    "*.h"
)
add_library(mysql OBJECT
    ${MYSQL_SOURCES}
    ${MYSQL_HEADERS}
)

target_include_directories(mysql PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)