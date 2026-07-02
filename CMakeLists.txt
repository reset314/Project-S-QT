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
    src/core/auth/Keychain.cpp
    src/core/auth/TokenManager.cpp
    src/core/network/HttpClient.cpp
    src/core/network/ChatStreamClient.cpp
    # Task 7 — Repositories (Part 1)
    src/repositories/AuthRepository.cpp
    src/repositories/ChatRepository.cpp
    src/repositories/ConversationRepository.cpp
    src/repositories/AIUserRepository.cpp
    # Task 8 — Repositories (Part 2)
    src/repositories/UserRepository.cpp
    src/repositories/MemoryRepository.cpp
    src/repositories/ProfileRepository.cpp
    src/repositories/UploadRepository.cpp
    src/repositories/ExpansionRepository.cpp
    src/repositories/CallRepository.cpp
    src/repositories/TTSRepository.cpp
    # Task 9 — Services: ChatService & SyncService
    src/services/ChatService.cpp
    src/services/SyncService.cpp
    # Task 10 — Services: UnreadTracker & NotificationService
    src/services/UnreadTracker.cpp
    src/services/NotificationService.cpp
    # Task 11 — ViewModels
    src/viewmodels/ChatMessagesModel.cpp
    src/viewmodels/ConversationListModel.cpp
    src/viewmodels/ContactListModel.cpp
    src/viewmodels/LoginViewModel.cpp
    src/viewmodels/SettingsViewModel.cpp
    # Task 16 — Call System
    src/core/network/CallSignalingClient.cpp
    src/services/WebRtcEngine.cpp
    src/services/AudioManager.cpp
    src/services/VideoManager.cpp
    src/services/CallManager.cpp
    # Task 17 — Platform Integration
    src/platform/WinTaskbar.cpp
    src/platform/MacDock.cpp
    src/platform/LinuxDesktop.cpp
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

# ── Unit Tests ───────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)

    set(TEST_SOURCES
        tests/tst_Database.cpp
        tests/tst_ApiError.cpp
        tests/tst_JsonHelper.cpp
        tests/tst_UuidGenerator.cpp
        tests/tst_TokenManager.cpp
        tests/tst_SyncService.cpp
        tests/tst_UnreadTracker.cpp
    )

    # Re-use production source files (exclude main.cpp which has its own main())
    set(PROD_SOURCES_FOR_TESTS
        src/core/utils/UuidGenerator.cpp
        src/core/storage/Database.cpp
        src/core/auth/Keychain.cpp
        src/core/auth/TokenManager.cpp
        src/core/network/HttpClient.cpp
        src/core/network/ChatStreamClient.cpp
        src/core/network/CallSignalingClient.cpp
        src/repositories/AuthRepository.cpp
        src/repositories/ChatRepository.cpp
        src/repositories/ConversationRepository.cpp
        src/repositories/AIUserRepository.cpp
        src/repositories/UserRepository.cpp
        src/repositories/MemoryRepository.cpp
        src/repositories/ProfileRepository.cpp
        src/repositories/UploadRepository.cpp
        src/repositories/ExpansionRepository.cpp
        src/repositories/CallRepository.cpp
        src/repositories/TTSRepository.cpp
        src/services/ChatService.cpp
        src/services/SyncService.cpp
        src/services/UnreadTracker.cpp
        src/services/NotificationService.cpp
        src/services/WebRtcEngine.cpp
        src/services/AudioManager.cpp
        src/services/VideoManager.cpp
        src/services/CallManager.cpp
    )

    add_executable(AIChatDesktopTests ${TEST_SOURCES} ${PROD_SOURCES_FOR_TESTS})

    target_include_directories(AIChatDesktopTests PRIVATE src src/core/vendor)

    target_link_libraries(AIChatDesktopTests PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network
        Qt6::WebSockets
        Qt6::Sql
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Widgets
        Qt6::Test
        nlohmann_json::nlohmann_json
    )

    if(LibDataChannel_FOUND)
        target_link_libraries(AIChatDesktopTests PRIVATE LibDataChannel::LibDataChannel)
        target_compile_definitions(AIChatDesktopTests PRIVATE HAS_LIBDATACHANNEL)
    endif()

    # Register the test with CTest
    add_test(NAME AIChatDesktopTests COMMAND AIChatDesktopTests)
endif()
