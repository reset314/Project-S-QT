find_path(LibDataChannel_INCLUDE_DIR rtc/rtc.hpp)
find_library(LibDataChannel_LIBRARY datachannel)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibDataChannel
    REQUIRED_VARS LibDataChannel_LIBRARY LibDataChannel_INCLUDE_DIR
)

if(LibDataChannel_FOUND)
    add_library(LibDataChannel::LibDataChannel UNKNOWN IMPORTED)
    set_target_properties(LibDataChannel::LibDataChannel PROPERTIES
        IMPORTED_LOCATION "${LibDataChannel_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibDataChannel_INCLUDE_DIR}"
    )
endif()
