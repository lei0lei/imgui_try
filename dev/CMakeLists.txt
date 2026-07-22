cmake_minimum_required(VERSION 3.22)

# Keep `cmake -S dev -B dev/build` working for local development. When this
# directory is added by the repository root, all library targets already exist.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(Imgui_dev LANGUAGES CXX C)
    set(IMGUITRY_BUILD_DEV OFF CACHE BOOL "" FORCE)
    add_subdirectory(".." "${CMAKE_BINARY_DIR}/imgui_try")
endif()

add_executable(Imgui_dev src/main.cpp)
target_link_libraries(Imgui_dev PRIVATE imgui_try::ui)

if(WIN32)
    target_link_options(Imgui_dev PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()

if(MSVC)
    target_compile_options(Imgui_dev PRIVATE /W4 /utf-8)
else()
    target_compile_options(Imgui_dev PRIVATE -Wall -Wextra)
endif()

add_custom_command(TARGET Imgui_dev POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:Imgui_dev>
    COMMENT "Copying SDL3 DLL to output directory"
)

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
