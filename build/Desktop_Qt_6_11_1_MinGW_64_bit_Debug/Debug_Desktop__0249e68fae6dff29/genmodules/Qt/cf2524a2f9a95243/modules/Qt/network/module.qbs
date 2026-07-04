QtModule {
    qtModuleName: "Network"
    Depends { name: "Qt"; submodules: ["core"]}

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
    libNameForLinkerDebug: "Qt6Network"
    libNameForLinkerRelease: "Qt6Network"
    libFilePathDebug: undefined
    libFilePathRelease: "D:/QT/6.11.1/mingw_64/lib/libQt6Network.a"
    pluginTypes: ["networkaccess","networkinformation","tls"]
    moduleConfig: ["shared"]
    cpp.defines: ["QT_NETWORK_LIB"]
    cpp.systemIncludePaths: ["D:/QT/6.11.1/mingw_64/include","D:/QT/6.11.1/mingw_64/include/QtNetwork"]
    cpp.libraryPaths: []
    Group {
        files: "D:/QT/6.11.1/mingw_64/metatypes/qt6network_metatypes.json"
        filesAreTargets: true
        fileTags: ["qt.core.metatypes"]
    }
    Group {
        files: libFilePath
        filesAreTargets: true
        fileTags: ["dynamiclibrary_import"]
    }
}
