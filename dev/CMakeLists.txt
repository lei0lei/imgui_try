cmake_minimum_required(VERSION 3.10)
project(example_sdl3_vulkan)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDL3 源码路径（根据你的实际路径修改）
set(SDL3_SOURCE_DIR "D:/p90x/SDL3-3.4.0")

# 将 SDL3 作为子项目编译
if(EXISTS "${SDL3_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Building SDL3 from source: ${SDL3_SOURCE_DIR}")
    
    # 禁用 SDL3 的一些不需要的选项，加快编译
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    
    # 添加 SDL3 子目录
    add_subdirectory(${SDL3_SOURCE_DIR} ${CMAKE_BINARY_DIR}/SDL3 EXCLUDE_FROM_ALL)
    
    set(SDL3_LIBRARY SDL3::SDL3)
else()
    message(FATAL_ERROR "SDL3 source not found at ${SDL3_SOURCE_DIR}")
endif()

# 查找 Vulkan
find_package(Vulkan REQUIRED)

# ImGui 源文件（dev 文件夹在项目根目录，所以返回上一级）
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# Lua 源码路径（根据你的实际路径修改）
set(LUA_SOURCE_DIR "D:/p90x/lua-5.5.0.tar/lua-5.5.0")

# 将 Lua 作为子项目编译
if(EXISTS "${LUA_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Building Lua from source: ${LUA_SOURCE_DIR}")

    # 添加 Lua 子目录
    add_subdirectory(${LUA_SOURCE_DIR} ${CMAKE_BINARY_DIR}/Lua EXCLUDE_FROM_ALL)

    set(LUA_LIBRARY lua) # 假设生成的库名为 lua
else()
    message(FATAL_ERROR "Lua source not found at ${LUA_SOURCE_DIR}")
endif()

# 包含目录
include_directories(${LUA_SOURCE_DIR}/include)

# 链接库
list(APPEND EXTRA_LIBRARIES ${LUA_LIBRARY})


# 自动收集所有 cpp 文件，减少维护负担
file(GLOB SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/services/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/command/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workbench/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/workbench/*.cpp"
)
add_executable(example_sdl3_vulkan ${SRC_FILES} ${IMGUI_SOURCES})

# 包含目录
target_include_directories(example_sdl3_vulkan PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${Vulkan_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(example_sdl3_vulkan
    ${SDL3_LIBRARY}
    ${Vulkan_LIBRARIES}
    ${EXTRA_LIBRARIES} # 添加 Lua 库
)

# Windows 特定：隐藏控制台窗口
if(WIN32)
    target_link_options(example_sdl3_vulkan PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()

# 自动复制 SDL3.dll 到可执行文件目录
add_custom_command(TARGET example_sdl3_vulkan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:example_sdl3_vulkan>
    COMMENT "Copying SDL3.dll to output directory"
)
