cmake_minimum_required(VERSION 3.16)
project(wasm_raycaster LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/web)

add_executable(engine
    wasm/engine.cpp
)

set_target_properties(engine PROPERTIES
    OUTPUT_NAME "engine"
    SUFFIX ".wasm"
)

target_compile_options(engine PRIVATE
    -O3
)

target_link_options(engine PRIVATE
    -O3
    --no-entry
    -sSTANDALONE_WASM=0
    -sEXPORTED_FUNCTIONS=["_render","_getFramebuffer","_getHeight","_getWidth","_move","_rotate"]
    -sEXPORT_ALL=0
)

