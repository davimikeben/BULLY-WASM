cmake_minimum_required(VERSION 3.16)
project(bully_web CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- Detectar Emscripten ----
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly (Emscripten)")
    set(EMSCRIPTEN_FLAGS
        "-s USE_WEBGL2=1"
        "-s WASM=1"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s MAXIMUM_MEMORY=4GB"
        "-s STACK_SIZE=5MB"
        "-s EXPORT_ES6=1"
        "-s MODULARIZE=1"
        "-s EXPORT_NAME='BullyWeb'"
        "-s ENVIRONMENT='web,worker'"
        "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue','setValue','UTF8ToString','stringToUTF8']"
    )
    add_compile_options(${EMSCRIPTEN_FLAGS})
    add_link_options(${EMSCRIPTEN_FLAGS})
else()
    message(STATUS "Building native (for testing ELF loader / relocations)")
endif()

# ---- Runtime core ----
add_library(bully_runtime STATIC
    runtime/elf/parser.cpp
    runtime/elf/relocations.cpp
    runtime/elf/linker.cpp
    runtime/android/shim.cpp
    runtime/android/jni.cpp
    graphics/gles3/gles3_shim.cpp
    graphics/egl/egl_shim.cpp
    audio/openal/openal_shim.cpp
)

target_include_directories(bully_runtime PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/runtime
    ${CMAKE_SOURCE_DIR}/graphics
    ${CMAKE_SOURCE_DIR}/audio
)

# ---- WASM entry (browser) ----
if(EMSCRIPTEN)
    add_executable(bully_web browser/src/main.cpp)
    target_link_libraries(bully_web PRIVATE bully_runtime)
    set_target_properties(bully_web PROPERTIES SUFFIX ".js")
endif()

# ---- Native tests ----
enable_testing()
add_subdirectory(tests)
