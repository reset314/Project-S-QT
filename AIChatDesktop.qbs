import qbs

Project {
    name: "AIChatDesktop"
    minimumQbsVersion: "2.0"

    CppApplication {
        name: "AIChatDesktop"
        consoleApplication: false
        type: ["application"]

        cpp.cxxLanguageVersion: "c++17"
        cpp.includePaths: [
            "src",
            "src/core/vendor",
            "src/core/vendor/aqt-stylesheets/src",
            "src/core/vendor/aqt-stylesheets/src/include",
            "src/core/vendor/aqt-stylesheets/third-party",
            "src/core/vendor/aqt-stylesheets/third-party/cpp-peglib",
        ]
        cpp.defines: ["QT_NO_DEBUG"]

        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.quickcontrols2" }
        Depends { name: "Qt.quick.dialogs" }        // FileDialog in ThemeSettingsPage
        Depends { name: "Qt.network" }
        Depends { name: "Qt.websockets" }
        Depends { name: "Qt.sql" }
        Depends { name: "Qt.multimedia" }
        Depends { name: "Qt.multimediawidgets" }
        Depends { name: "Qt.widgets" }
        Depends { name: "Qt.concurrent" }
        Depends { name: "nlohmann_json"; required: false }

        files: [
            "src/main.cpp",
            // Utils
            "src/core/utils/UuidGenerator.cpp",
            "src/core/utils/UuidGenerator.h",
            "src/core/utils/SecondaryWindow.cpp",
            "src/core/utils/SecondaryWindow.h",
            "src/core/utils/JsonHelper.h",
            "src/core/utils/ApiError.h",
            // Theme
            "src/core/theme/ThemeManager.cpp",
            "src/core/theme/ThemeManager.h",
            // Storage
            "src/core/storage/Database.cpp",
            "src/core/storage/Database.h",
            // Auth
            "src/core/auth/Keychain.cpp",
            "src/core/auth/Keychain.h",
            "src/core/auth/TokenManager.cpp",
            "src/core/auth/TokenManager.h",
            // Network
            "src/core/network/HttpClient.cpp",
            "src/core/network/HttpClient.h",
            "src/core/network/ChatStreamClient.cpp",
            "src/core/network/ChatStreamClient.h",
            "src/core/network/CallSignalingClient.cpp",
            "src/core/network/CallSignalingClient.h",
            // Models
            "src/core/models/MessageDTO.h",
            "src/core/models/MediaDTO.h",
            "src/core/models/MemoryDTO.h",
            "src/core/models/AIUserDTO.h",
            "src/core/models/ConversationDTO.h",
            "src/core/models/ExpansionModuleDTO.h",
            "src/core/models/ContactDTO.h",
            // Repositories
            "src/repositories/AuthRepository.cpp",
            "src/repositories/AuthRepository.h",
            "src/repositories/ChatRepository.cpp",
            "src/repositories/ChatRepository.h",
            "src/repositories/ConversationRepository.cpp",
            "src/repositories/ConversationRepository.h",
            "src/repositories/AIUserRepository.cpp",
            "src/repositories/AIUserRepository.h",
            "src/repositories/UserRepository.cpp",
            "src/repositories/UserRepository.h",
            "src/repositories/MemoryRepository.cpp",
            "src/repositories/MemoryRepository.h",
            "src/repositories/ProfileRepository.cpp",
            "src/repositories/ProfileRepository.h",
            "src/repositories/UploadRepository.cpp",
            "src/repositories/UploadRepository.h",
            "src/repositories/ExpansionRepository.cpp",
            "src/repositories/ExpansionRepository.h",
            "src/repositories/CallRepository.cpp",
            "src/repositories/CallRepository.h",
            "src/repositories/TTSRepository.cpp",
            "src/repositories/TTSRepository.h",
            // Services
            "src/services/ChatService.cpp",
            "src/services/ChatService.h",
            "src/services/SyncService.cpp",
            "src/services/SyncService.h",
            "src/services/UnreadTracker.cpp",
            "src/services/UnreadTracker.h",
            "src/services/NotificationService.cpp",
            "src/services/NotificationService.h",
            "src/services/WebRtcEngine.cpp",
            "src/services/WebRtcEngine.h",
            "src/services/AudioManager.cpp",
            "src/services/AudioManager.h",
            "src/services/VideoManager.cpp",
            "src/services/VideoManager.h",
            "src/services/CallManager.cpp",
            "src/services/CallManager.h",
            // ViewModels
            "src/viewmodels/ChatMessagesModel.cpp",
            "src/viewmodels/ChatMessagesModel.h",
            "src/viewmodels/ConversationListModel.cpp",
            "src/viewmodels/ConversationListModel.h",
            "src/viewmodels/ContactListModel.cpp",
            "src/viewmodels/ContactListModel.h",
            "src/viewmodels/LoginViewModel.cpp",
            "src/viewmodels/LoginViewModel.h",
            "src/viewmodels/SettingsViewModel.cpp",
            "src/viewmodels/SettingsViewModel.h",
            // Platform
            "src/platform/PlatformIntegration.cpp",
            "src/platform/PlatformIntegration.h",
            "src/platform/WinTaskbar.cpp",
            "src/platform/WinTaskbar.h",
            "src/platform/MacDock.cpp",
            "src/platform/MacDock.h",
            "src/platform/LinuxDesktop.cpp",
            "src/platform/LinuxDesktop.h",
            // Vendor
            "src/core/vendor/tl/expected.hpp",
            // aqt-stylesheets
            "src/core/vendor/aqt-stylesheets/src/Convert.cpp",
            "src/core/vendor/aqt-stylesheets/src/CssParser.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleChecker.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleEngine.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleEngineSetup.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleMatchTree.cpp",
            "src/core/vendor/aqt-stylesheets/src/StylePlugin.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleSet.cpp",
            "src/core/vendor/aqt-stylesheets/src/StyleSetProps.cpp",
            "src/core/vendor/aqt-stylesheets/src/StylesDirWatcher.cpp",
            "src/core/vendor/aqt-stylesheets/src/UrlUtils.cpp",
            // QML Resources
            "resources/qml.qrc",
        ]

        Properties {
            condition: qbs.targetOS.contains("windows")
            cpp.dynamicLibraries: ["dwmapi"]
        }
    }
}
