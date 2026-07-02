cmake_minimum_required(VERSION 3.21)
project(AIChatDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick QuickControls2 Network WebSockets Sql Multimedia MultimediaWidgets Widgets
)

find_package(nlohmann_json REQUIRED)
find_package(LibDataChannel QUIET)

set(SOURCES
    src/main.cpp
    src/core/utils/UuidGenerator.cpp
    src/core/storage/Database.cpp
    # Additional sources added in later tasks
)

set(RESOURCES
    resources/qml.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src src/core/vendor)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Widgets
    nlohmann_json::nlohmann_json
)

if(LibDataChannel_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE LibDataChannel::LibDataChannel)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_LIBDATACHANNEL)
endif()

# vcpkg integration hint
if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "vcpkg detected: $ENV{VCPKG_ROOT}")
endif()
