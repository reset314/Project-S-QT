import qbs.FileInfo
import qbs.Host
import qbs.TextFile
import "qml.js" as Qml

QtModule {
    qtModuleName: "Qml"
    Depends { name: "Qt"; submodules: ["core","qmlintegration","network"]}

    property string qmlImportScannerName: Qt.core.qmlImportScannerName
    property string qmlImportScannerFilePath: Qt.core.qmlImportScannerFilePath
    property string qmlPath: "D:/QT/6.11.1/mingw_64/qml"

    property bool generateCacheFiles: false
    Depends { name: "Qt.qmlcache"; condition: generateCacheFiles; required: false }
    readonly property bool cachingEnabled: generateCacheFiles && Qt.qmlcache.present
    property string qmlCacheGenPath
    Properties {
        condition: cachingEnabled
        Qt.qmlcache.qmlCacheGenPath: qmlCacheGenPath || original
        Qt.qmlcache.installDir: cacheFilesInstallDir || original
    }

    property string cacheFilesInstallDir

    readonly property string pluginListFilePathDebug: product.buildDirectory + "/plugins.list.d"
    readonly property string pluginListFilePathRelease: product.buildDirectory + "/plugins.list"
    property bool linkPlugins: isStaticLibrary && Qt.plugin_support.linkPlugins

    version: "6.11.1"
    hasLibrary: true
    architectures: ["x86_64"]
    targetPlatform: "windows"
    staticLibsDebug: (linkPlugins ? ['@' + pluginListFilePathDebug] : []).concat([])
    staticLibsRelease: (linkPlugins ? ['@' + pluginListFilePathRelease] : []).concat([])
    dynamicLibsDebug: []
    dynamicLibsRelease: []
    linkerFlagsDebug: []
    linkerFlagsRelease: []
    frameworksDebug: []
    frameworksRelease: []
    frameworkPathsDebug: []
    frameworkPathsRelease: []
    libNameForLinkerDebug: "Qt6Qml"
    libNameForLinkerRelease: "Qt6Qml"
    libFilePathDebug: undefined
    libFilePathRelease: "D:/QT/6.11.1/mingw_64/lib/libQt6Qml.a"
    pluginTypes: ["qmltooling"]
    moduleConfig: ["shared"]
    cpp.defines: ["QT_QML_LIB"]
    cpp.systemIncludePaths: ["D:/QT/6.11.1/mingw_64/include","D:/QT/6.11.1/mingw_64/include/QtQml"]
    cpp.libraryPaths: []
    Group {
        files: "D:/QT/6.11.1/mingw_64/metatypes/qt6qml_metatypes.json"
        filesAreTargets: true
        fileTags: ["qt.core.metatypes"]
    }
    Group {
        files: libFilePath
        filesAreTargets: true
        fileTags: ["dynamiclibrary_import"]
    }

    FileTagger {
        patterns: ["*.qml"]
        fileTags: ["qt.qml.qml"]
    }

    FileTagger {
        patterns: ["*.js"]
        fileTags: ["qt.qml.js"]
    }

    // Type registration
    property string importName
    property string importVersion: product.version
    readonly property stringList _importVersionParts: (importVersion || "").split(".")
    property string typesFileName: {
        if (product.type && product.type.contains("application"))
            return product.targetName + ".qmltypes";
        return "plugins.qmltypes";
    }
    property string typesInstallDir
    property stringList extraMetaTypesFiles


    Group {
        condition: importName
        product.Qt.core.generateMetaTypesFile: true
        Rule {
            inputs: "qt.core.metatypes"
            multiplex: true
            explicitlyDependsOnFromDependencies: "qt.core.metatypes"
            Artifact {
                filePath: product.targetName.toLowerCase() + "_qmltyperegistrations.cpp"
                fileTags: ["cpp", "unmocable", "qt.untranslatable"]
            }
            Artifact {
                filePath: product.Qt.qml.typesFileName
                fileTags: "qt.qml.types"
                qbs.install: product.Qt.qml.typesInstallDir
                qbs.installDir: product.Qt.qml.typesInstallDir
            }
            prepare: Qml.typeRegistrarCommands.apply(Qml, arguments)
        }
    }

    Rule {
        condition: linkPlugins
        multiplex: true
        requiresInputs: false
        inputs: ["qt.qml.qml"]
        outputFileTags: ["cpp", "qt.qml.pluginlist", "qt.untranslatable"]
        outputArtifacts: Qml.generatePluginImportOutputArtifacts(product, inputs)
        prepare: Qml.generatePluginImportCommands.apply(Qml, arguments)
    }

    validate: {
        if (importName) {
            if (!importVersion)
                throw "Qt.qml.importVersion must be set if Qt.qml.importName is set.";
            var isInt = function(value) {
                return !isNaN(value) && parseInt(Number(value)) == value
                        && !isNaN(parseInt(value, 10));
            }
            if (!isInt(_importVersionParts[0])
                    || (_importVersionParts.length > 1 && !isInt(_importVersionParts[1]))) {
                throw "Qt.qml.importVersion must be of the form x.y, where x and y are integers.";
            }

        }
    }
}
