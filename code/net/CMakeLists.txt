file(GLOB_RECURSE NET_SOURCES 
    "asio/*.cpp"
    "uv/*.cpp"
)
file (GLOB_RECURSE NET_HEADERS 
    "uv/*.h"
    "asio/*.h"
)
add_library(net OBJECT
    ${NET_SOURCES}
    ${NET_HEADERS}
)

target_include_directories(net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)