# Recursively collect all .cpp files under a source root.
function(imguitry_collect_src_files out_var src_root)
    file(GLOB_RECURSE _imguitry_src_files
        "${src_root}/*.cpp"
    )
    set(${out_var} ${_imguitry_src_files} PARENT_SCOPE)
endfunction()
