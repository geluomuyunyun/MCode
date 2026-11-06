cmake_minimum_required(VERSION 3.16)
project(DataStructuresAndAlgorithms C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

include_directories(${CMAKE_SOURCE_DIR}/common)
enable_testing()

# 每个模块目录生成一个测试程序，目标名形如 c_01_dynamic_array
function(add_module dir)
    file(GLOB srcs CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/${dir}/*.c
        ${CMAKE_SOURCE_DIR}/${dir}/*.cpp)
    if(NOT srcs)
        message(WARNING "module ${dir} has no source files, skipped")
        return()
    endif()
    string(REPLACE "/" "_" target ${dir})
    add_executable(${target} ${srcs})
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/${dir})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

# ---------- 第一、二阶段：C ----------
add_module(c/01_dynamic_array)
add_module(c/02_linked_list)
add_module(c/03_stack_queue)
add_module(c/04_string)
add_module(c/05_binary_tree)
add_module(c/06_bst)
add_module(c/07_heap)
add_module(c/08_hash_table)
add_module(c/09_union_find)

# ---------- 第三、四阶段：C++ 数据结构 ----------
add_module(cpp/01_vector)
add_module(cpp/02_list)
add_module(cpp/03_stack_queue)
add_module(cpp/04_hash_map)
add_module(cpp/05_binary_tree)
add_module(cpp/06_avl_tree)
add_module(cpp/07_red_black_tree)
add_module(cpp/08_btree)
add_module(cpp/09_trie)
add_module(cpp/10_segment_tree)
add_module(cpp/11_graph)

# ---------- 第五阶段：C++ 算法 ----------
add_module(cpp/algorithms/01_sort)
add_module(cpp/algorithms/02_search)
add_module(cpp/algorithms/03_backtracking)
add_module(cpp/algorithms/04_divide_conquer)
add_module(cpp/algorithms/05_two_pointers)
add_module(cpp/algorithms/06_sliding_window)
add_module(cpp/algorithms/07_greedy)
add_module(cpp/algorithms/08_dynamic_programming)
