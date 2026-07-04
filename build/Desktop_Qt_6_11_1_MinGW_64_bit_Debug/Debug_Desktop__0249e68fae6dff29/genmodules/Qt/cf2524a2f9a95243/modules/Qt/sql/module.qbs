QtModule {
    qtModuleName: "Sql"
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
    libNameForLinkerDebug: "Qt6Sql"
    libNameForLinkerRelease: "Qt6Sql"
    libFilePathDebug: undefined
    libFilePathRelease: "D:/QT/6.11.1/mingw_64/lib/libQt6Sql.a"
    pluginTypes: ["sqldrivers"]
    moduleConfig: ["shared"]
    cpp.defines: ["QT_SQL_LIB"]
    cpp.systemIncludePaths: ["D:/QT/6.11.1/mingw_64/include","D:/QT/6.11.1/mingw_64/include/QtSql"]
    cpp.libraryPaths: []
    Group {
        files: "D:/QT/6.11.1/mingw_64/metatypes/qt6sql_metatypes.json"
        filesAreTargets: true
        fileTags: ["qt.core.metatypes"]
    }
    Group {
        files: libFilePath
        filesAreTargets: true
        fileTags: ["dynamiclibrary_import"]
    }
}
