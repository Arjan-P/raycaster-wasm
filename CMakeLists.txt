cmake_minimum_required(VERSION 3.16)
project(wasm_raycaster LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/web/public)

add_executable(engine
    wasm/engine.cpp
    wasm/engine_core.cpp
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
    -sEXPORTED_FUNCTIONS=["_engine_move","_engine_rotate","_engine_render","_init","_engine_map_buffer","_engine_draw_map_bg","_engine_top_down_screenbuffer","_engine_screenbuffer","_engine_screen_width","_engine_screen_height","_engine_map_screen_width","_engine_map_screen_height"]
    -sEXPORT_ALL=0
)

