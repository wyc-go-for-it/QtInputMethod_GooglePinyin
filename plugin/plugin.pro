QT          = core gui-private widgets

TEMPLATE    = lib
CONFIG += shared
TARGET      = softKeyboardPlugin
DEFINES += PLUGIN_LIBRARY

HEADERS     += \
            softKeyboard.h

SOURCES     += \
    softKeyboard.cpp

INCLUDEPATH += $$PWD/../googlepinyin
DEPENDPATH += $$PWD/../googlepinyin

win32{
    CONFIG(debug, debug|release){
        TARGET  = $$join(TARGET,,,d)
        LIBS        += -L$$PWD/../build/lib/googlepinyin -lgooglepinyind

        LIB_PATH = win32/debug
    }CONFIG(release, debug|release){
        TARGET  = $$TARGET
        LIBS        += -L$$PWD/../build/lib/googlepinyin -lgooglepinyin

        LIB_PATH = win32/release
    }

}
unix:!mac:{
    contains(ANDROID_TARGET_ARCH,x86_64){
         DIR = googlepinyin_x86_64
     }
     contains(ANDROID_TARGET_ARCH,x86){
         DIR = googlepinyin_x86
     }
     contains(ANDROID_TARGET_ARCH,arm64-v8a){
         DIR = googlepinyin_arm64-v8a
     }
     contains(ANDROID_TARGET_ARCH,armeabi-v7a){
         DIR = googlepinyin_armeabi-v7a
     }

    LIBS  += -L$$PWD/../build/lib/$${DIR} -lgooglepinyin

    LIB_PATH = android/
}

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

DESTDIR  = $$PWD/../build/bin/platforminputcontexts/$${LIB_PATH}

RESOURCES += \
    res.qrc

