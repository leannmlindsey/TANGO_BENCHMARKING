cmake_minimum_required (VERSION 3.4)
project (seqan3_tutorial CXX)
 
# add seqan3 to search path
list (APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../seqan3/build_system")
 
# require seqan3 with a version between >=3.0.0 and <4.0.0
find_package (seqan3 3.0 REQUIRED)
find_package(OpenMP REQUIRED)


add_executable (seqan_aa_align_multithread seqan_aa_align_multithread.cpp)
target_link_libraries (seqan_aa_align_multithread seqan3::seqan3 OpenMP::OpenMP_CXX)

add_executable (seqan_dna_align_multithread seqan_dna_align_multithread.cpp)
target_link_libraries (seqan_dna_align_multithread seqan3::seqan3 OpenMP::OpenMP_CXX)

