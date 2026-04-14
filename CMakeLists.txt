cmake_minimum_required(VERSION 3.16)

# ✅ خواندن VERSION از فایل
file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

# ✅ تجزیه version به اجزای MAJOR.MINOR.PATCH
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

# ✅ محاسبه versionCode (مثلاً: 1.3.0 → 10300)
math(EXPR VERSION_CODE "${VERSION_MAJOR} * 10000 + ${VERSION_MINOR} * 100 + ${VERSION_PATCH}")

project(QMLVosk VERSION ${VERSION_STRING} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(QMLVosk
    main.cpp
)

qt_add_qml_module(QMLVosk
    URI QMLVosk
    QML_FILES
        Main.qml
        theme/Theme.qml
        components/CButton.qml
        components/CTextField.qml
        components/Toast.qml
    SOURCES
        backend.h backend.cpp
        GrammerModel.h GrammerModel.cpp
        ../../QtLibraries/vosk/voiceengine.h ../../QtLibraries/vosk/voiceengine.cpp
)

qt6_add_resources(${PROJECT_NAME} "qmlchartsgallery"
    PREFIX "/"
    FILES
        version.txt
)

# ---------------- Vosk paths ----------------

set(VOSK_WINDOWS_DIR "D:/my_Projects/Programming/Qt/QtLibraries/voskWin")
set(VOSK_ANDROID_DIR "D:/my_Projects/Programming/Qt/QtLibraries/voskAndroid")

# include path (header)
target_include_directories(QMLVosk PRIVATE
    ${VOSK_WINDOWS_DIR}
)

# ---------------- Windows ----------------
if (WIN32 AND NOT ANDROID)

    target_link_directories(QMLVosk PRIVATE
        ${VOSK_WINDOWS_DIR}
    )

    target_link_libraries(QMLVosk PRIVATE
        vosk.dll
    )

    # copy DLL
    add_custom_command(TARGET QMLVosk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${VOSK_WINDOWS_DIR}/libvosk.dll
        $<TARGET_FILE_DIR:QMLVosk>
    )

    # مسیر فولدر منبع
    set(SOURCE_ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/assets/models/vosk-model-small-fa-0.5")

    # مسیر فولدر مقصد
    set(TARGET_ASSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/vosk-model-small-fa-0.5")

    add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets to build directory..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${SOURCE_ASSET_DIR}"
                "${TARGET_ASSET_DIR}"
        COMMENT "Copy assets folder to binary dir"
    )

    # وابسته‌کردن این عمل به اجرای برنامه (اختیاری ولی توصیه‌شده)
    add_dependencies(${PROJECT_NAME} copy_assets)

endif()

# ---------------- Android ----------------
if (ANDROID)

    message(STATUS "Configuring Vosk for Android")

    add_library(vosk SHARED IMPORTED)

    set_target_properties(vosk PROPERTIES
        IMPORTED_LOCATION
        "${VOSK_ANDROID_DIR}/${ANDROID_ABI}/libvosk.so"
    )

    target_include_directories(QMLVosk PRIVATE
        ${VOSK_ANDROID_DIR}
    )

    target_link_libraries(QMLVosk PRIVATE
        vosk
    )

endif()

if(ANDROID)
    set_target_properties(QMLVosk PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/android
            QT_ANDROID_VERSION_NAME "${VERSION_STRING}"
            QT_ANDROID_VERSION_CODE "${VERSION_CODE}"
    )
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(QMLVosk PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QMLVosk
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(QMLVosk
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS QMLVosk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

message(STATUS "VOSK_WINDOWS_DIR = ${VOSK_WINDOWS_DIR}")
message(STATUS "ANDROID_ABI = ${ANDROID_ABI}")
