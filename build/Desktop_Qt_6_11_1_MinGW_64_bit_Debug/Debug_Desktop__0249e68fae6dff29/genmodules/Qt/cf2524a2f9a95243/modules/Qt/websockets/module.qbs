QtModule {
    qtModuleName: "WebSockets"
    Depends { name: "Qt"; submodules: ["core","network"]}

    version: "6.11.1"
    architectures: ["x86_64"]
    targetPlatform: "windows"
    hasLibrary: true
    staticLibsDebug: []
    staticLibsRelease: []
    dynamicLibsDebug: []
    dynamicLibsRelease: []
    linkerFlagsDebug: []
    linkerFlagsRelease: []
    frameworksDebug: []
    frameworksRelease: []
    frameworkPathsDebug: []
    frameworkPathsRelease: []
    libNameForLinkerDebug: "Qt6WebSockets"
    libNameForLinkerRelease: "Qt6WebSockets"
    libFilePathDebug: undefined
    libFilePathRelease: "D:/QT/6.11.1/mingw_64/lib/libQt6WebSockets.a"
    pluginTypes: []
    moduleConfig: ["shared"]
    cpp.defines: ["QT_WEBSOCKETS_LIB"]
    cpp.systemIncludePaths: ["D:/QT/6.11.1/mingw_64/include","D:/QT/6.11.1/mingw_64/include/QtWebSockets"]
    cpp.libraryPaths: []
    Group {
        files: "D:/QT/6.11.1/mingw_64/metatypes/qt6websockets_metatypes.json"
        filesAreTargets: true
        fileTags: ["qt.core.metatypes"]
    }
    Group {
        files: libFilePath
        filesAreTargets: true
        fileTags: ["dynamiclibrary_import"]
    }
}
