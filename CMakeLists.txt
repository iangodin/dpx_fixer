
cmake_minimum_required( VERSION 3.25 )

project( dpx_fixer )

add_executable( dump_header dump_header.cpp dpx.cpp )

add_executable( copy_dpx_metadata copy_dpx_metadata.cpp dpx.cpp )
