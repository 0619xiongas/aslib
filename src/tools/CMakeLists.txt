file(GLOB TOOLS_SOURCES "*.cpp")
file(GLOB TOOLS_HEADERS "../../include/tools/*.h" "../../include/tools/*.hpp")

add_library(tools OBJECT
    ${TOOLS_SOURCES}
    ${TOOLS_HEADERS}
)

target_include_directories(tools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)