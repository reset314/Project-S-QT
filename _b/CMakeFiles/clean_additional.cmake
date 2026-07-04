# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "AIChatDesktop_autogen"
  "CMakeFiles\\AIChatDesktop_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AIChatDesktop_autogen.dir\\ParseCache.txt"
  )
endif()
