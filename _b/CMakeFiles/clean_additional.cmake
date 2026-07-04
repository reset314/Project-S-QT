# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "AIChatDesktop_autogen"
  "CMakeFiles\\AIChatDesktop_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AIChatDesktop_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_ApiError_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_ApiError_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_Database_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_Database_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_JsonHelper_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_JsonHelper_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_SyncService_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_SyncService_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_TokenManager_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_TokenManager_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_UnreadTracker_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_UnreadTracker_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\test_UuidGenerator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\test_UuidGenerator_autogen.dir\\ParseCache.txt"
  "test_ApiError_autogen"
  "test_Database_autogen"
  "test_JsonHelper_autogen"
  "test_SyncService_autogen"
  "test_TokenManager_autogen"
  "test_UnreadTracker_autogen"
  "test_UuidGenerator_autogen"
  )
endif()
