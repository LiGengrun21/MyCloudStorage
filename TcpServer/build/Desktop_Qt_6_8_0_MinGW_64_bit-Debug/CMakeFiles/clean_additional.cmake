# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\TcpServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\TcpServer_autogen.dir\\ParseCache.txt"
  "TcpServer_autogen"
  )
endif()
