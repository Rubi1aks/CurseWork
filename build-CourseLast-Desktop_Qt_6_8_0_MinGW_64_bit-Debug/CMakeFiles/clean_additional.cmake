# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CourseLast_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CourseLast_autogen.dir\\ParseCache.txt"
  "CourseLast_autogen"
  )
endif()
