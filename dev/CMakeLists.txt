# 本地路径
file(GLOB_RECURSE IMGUITRY_UI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp"
)

# ImGui UI 静态库
add_library(imgui_try_ui STATIC ${IMGUITRY_UI_SOURCES})
add_library(imgui_try::ui ALIAS imgui_try_ui)
target_include_directories(imgui_try_ui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(imgui_try_ui PUBLIC imgui_try::imgui)

# ImGui 开发模式可执行文件
add_executable(Imgui_dev src/main.cpp)
target_link_libraries(Imgui_dev PRIVATE imgui_try::ui)

# 链接选项
if(WIN32)
    target_link_options(Imgui_dev PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()

# 编译选项
if(MSVC)
    target_compile_options(Imgui_dev PRIVATE /W4 /utf-8)
else()
    target_compile_options(Imgui_dev PRIVATE -Wall -Wextra)
endif()

# 复制 SDL3 DLL 到输出目录
add_custom_command(TARGET Imgui_dev POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:Imgui_dev>
    COMMENT "Copying SDL3 DLL to output directory"
)

# Doxygen 文档生成
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
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

    doxygen_add_docs(docs
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        COMMENT "Generating API documentation with Doxygen"
    )
endif()
