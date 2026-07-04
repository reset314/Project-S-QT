QtModule {
    qtModuleName: "Multimedia"
    Depends { name: "Qt"; submodules: ["core","gui","network"]}

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
    libNameForLinkerDebug: "Qt6Multimedia"
    libNameForLinkerRelease: "Qt6Multimedia"
    libFilePathDebug: undefined
    libFilePathRelease: "D:/QT/6.11.1/mingw_64/lib/libQt6Multimedia.a"
    pluginTypes: ["multimedia"]
    moduleConfig: ["shared"]
    cpp.defines: ["QT_MULTIMEDIA_LIB"]
    cpp.systemIncludePaths: ["D:/QT/6.11.1/mingw_64/include","D:/QT/6.11.1/mingw_64/include/QtMultimedia"]
    cpp.libraryPaths: []
    Group {
        files: "D:/QT/6.11.1/mingw_64/metatypes/qt6multimedia_metatypes.json"
        filesAreTargets: true
        fileTags: ["qt.core.metatypes"]
    }
    Group {
        files: libFilePath
        filesAreTargets: true
        fileTags: ["dynamiclibrary_import"]
    }
}
