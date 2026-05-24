cmake_minimum_required(VERSION 3.20)
project(IADorisovkaPlugin LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(SANITIZE "Enable compiler sanitizers" OFF)
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
         $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
    add_link_options(
         $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/gui-interface)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
add_library(${PROJECT_NAME} SHARED
    ${SRC_FILES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/gui-interface/include
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    cum
    dr4
    hui
    pp
)


                      