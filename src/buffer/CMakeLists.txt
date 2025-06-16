file(GLOB BUFFER_SOURCES "*.cpp")
file(GLOB BUFFER_HEADERS "../../include/buffer/*.h")

add_library(buffer OBJECT
    ${BUFFER_SOURCES}
    ${BUFFER_HEADERS}
)

target_include_directories(buffer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)