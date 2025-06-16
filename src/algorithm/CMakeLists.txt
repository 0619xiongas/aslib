file(GLOB ALGORITHM_SOURCES "*.cpp")
file(GLOB ALGORITHM_HEADERS "../../include/algorithm/*.h")

add_library(algorithm OBJECT
    ${ALGORITHM_SOURCES}
    ${ALGORITHM_HEADERS}
)

target_include_directories(algorithm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)