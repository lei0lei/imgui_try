cmake_minimum_required(VERSION 3.22)
project(imgui_try LANGUAGES CXX C)

include(FetchContent)
# 本地路径
set(_imguitry_local_paths "${CMAKE_CURRENT_SOURCE_DIR}/cmake/local_paths.cmake")
if(EXISTS "${_imguitry_local_paths}")
    include("${_imguitry_local_paths}")
endif()
unset(_imguitry_local_paths)

# 选项
option(IMGUITRY_BUILD_DEV "Build the imgui_try development application" ${PROJECT_IS_TOP_LEVEL})
option(IMGUITRY_USE_VOLK "Use volk as Vulkan function loader" OFF)
option(IMGUITRY_WITH_LUA "Fetch and build Lua support" ON)

find_package(Vulkan REQUIRED)

# SDL3 获取
if(NOT TARGET SDL3::SDL3)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# vk-bootstrap 获取
if(NOT TARGET vk-bootstrap::vk-bootstrap)
    set(VK_BOOTSTRAP_TEST OFF CACHE BOOL "" FORCE)
    set(VK_BOOTSTRAP_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
        GIT_TAG v1.4.321
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(vk_bootstrap)
endif()

# volk 获取
if(IMGUITRY_USE_VOLK AND NOT TARGET volk::volk)
    set(VOLK_INSTALL OFF CACHE BOOL "" FORCE)
    set(VOLK_PULL_IN_VULKAN ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG vulkan-sdk-1.4.321.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(volk)
endif()

# Lua 获取
if(IMGUITRY_WITH_LUA AND NOT TARGET imgui_try_lua)
    FetchContent_Declare(
        lua
        GIT_REPOSITORY https://github.com/lua/lua.git
        GIT_TAG v5.5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(lua)

    set(IMGUITRY_LUA_SOURCES
        lapi.c
        lauxlib.c
        lbaselib.c
        lcode.c
        lcorolib.c
        lctype.c
        ldblib.c
        ldebug.c
        ldo.c
        ldump.c
        lfunc.c
        lgc.c
        linit.c
        liolib.c
        llex.c
        lmathlib.c
        lmem.c
        loadlib.c
        lobject.c
        lopcodes.c
        loslib.c
        lparser.c
        lstate.c
        lstring.c
        lstrlib.c
        ltable.c
        ltablib.c
        ltm.c
        lundump.c
        lutf8lib.c
        lvm.c
        lzio.c
    )
    list(TRANSFORM IMGUITRY_LUA_SOURCES PREPEND "${lua_SOURCE_DIR}/")

    add_library(imgui_try_lua STATIC ${IMGUITRY_LUA_SOURCES})
    add_library(imgui_try::lua ALIAS imgui_try_lua)
    target_include_directories(imgui_try_lua PUBLIC "${lua_SOURCE_DIR}")
endif()

# ImGui 获取
set(IMGUITRY_IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_vulkan.cpp
)
list(TRANSFORM IMGUITRY_IMGUI_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

# ImGui 静态库
add_library(imgui_try_imgui STATIC
    ${IMGUITRY_IMGUI_SOURCES}
)
# ImGui 静态库别名
add_library(imgui_try::imgui ALIAS imgui_try_imgui)

target_compile_features(imgui_try_imgui PUBLIC cxx_std_17)
# ImGui 头文件路径
target_include_directories(imgui_try_imgui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"
)
# ImGui 链接库
target_link_libraries(imgui_try_imgui PUBLIC
    SDL3::SDL3
    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap
)

# Lua 链接库
if(IMGUITRY_WITH_LUA)
    target_link_libraries(imgui_try_imgui PUBLIC imgui_try::lua)
endif()

# Volk 链接库
if(IMGUITRY_USE_VOLK)
    target_compile_definitions(imgui_try_imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK)
    target_link_libraries(imgui_try_imgui PUBLIC volk::volk)
endif()

# 开发模式
if(IMGUITRY_BUILD_DEV)
    add_subdirectory(dev)
endif()
