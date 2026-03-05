cmake_minimum_required(VERSION 3.10)
project(example_sdl3_vulkan)

option(IMGUITRY_USE_VOLK "Use volk as Vulkan function loader" OFF)
set(IMGUITRY_VOLK_SOURCE_DIR "D:/github/volk" CACHE PATH "Path to volk source directory")

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

if(IMGUITRY_USE_VOLK)
    if(EXISTS "${IMGUITRY_VOLK_SOURCE_DIR}/CMakeLists.txt")
        message(STATUS "Building volk from source: ${IMGUITRY_VOLK_SOURCE_DIR}")
        set(VOLK_INSTALL OFF CACHE BOOL "" FORCE)
        add_subdirectory(${IMGUITRY_VOLK_SOURCE_DIR} ${CMAKE_BINARY_DIR}/volk EXCLUDE_FROM_ALL)
    else()
        message(FATAL_ERROR "volk source not found at ${IMGUITRY_VOLK_SOURCE_DIR}")
    endif()
endif()

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
include_directories(${LUA_SOURCE_DIR}/src)

# 链接库
list(APPEND EXTRA_LIBRARIES ${LUA_LIBRARY})


# 自动收集所有 cpp 文件，减少维护负担
file(GLOB SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms/*.cpp"
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

if(IMGUITRY_USE_VOLK)
    target_compile_definitions(example_sdl3_vulkan PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
    target_include_directories(example_sdl3_vulkan PRIVATE ${IMGUITRY_VOLK_SOURCE_DIR})
endif()

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

# 生成 Doxygen 文档
# 设置 Doxygen 可执行文件路径（如果不在 PATH 中）
set(DOXYGEN_EXECUTABLE "D:/doxygen/bin/doxygen.exe")

find_package(Doxygen)
if(DOXYGEN_FOUND)
    # 设置 Doxygen 配置
    set(DOXYGEN_PROJECT_NAME "imgui_try-dev")
    set(DOXYGEN_PROJECT_BRIEF "开发工作区")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    set(DOXYGEN_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_FILE_PATTERNS "*.h *.hpp *.cpp")
    set(DOXYGEN_EXCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/build")
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_HTML_OUTPUT "html")
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_OUTPUT_LANGUAGE Chinese)
    set(DOXYGEN_QUIET YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_FULL_SIDEBAR YES)
    set(DOXYGEN_HAVE_DOT NO)

    # 创建 docs 目标
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating API documentation with Doxygen"
    )

    # 可选：在构建后自动生成文档
    # add_dependencies(example_sdl3_vulkan docs)
else()
    message(WARNING "Doxygen not found. Documentation will not be generated.")
endif()
