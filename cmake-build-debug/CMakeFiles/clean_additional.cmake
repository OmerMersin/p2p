# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/p2p_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/p2p_autogen.dir/ParseCache.txt"
  "p2p_autogen"
  )
endif()
