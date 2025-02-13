QT       += core gui-private  gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cLineEdit.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

win32{
#    CONFIG      += debug_and_release build_all

    CONFIG(debug, debug|release){
        TARGET  = $$join(TARGET,,,d)
        LIBS        += -L$$PWD/../build/bin/platforminputcontexts/win32/debug -lsoftKeyboardPlugind
    }CONFIG(release, debug|release){
        TARGET  = $$TARGET
        LIBS        += -L$$PWD/../build/bin/platforminputcontexts/win32/release -lsoftKeyboardPlugin
    }
}
unix:!mac:{
    contains(ANDROID_TARGET_ARCH,x86_64){
         LIBS += -L$$PWD/../build/bin/platforminputcontexts/android -lsoftKeyboardPlugin_x86_64
     }
     contains(ANDROID_TARGET_ARCH,x86){
         LIBS += -L$$PWD/../build/bin/platforminputcontexts/android -lsoftKeyboardPlugin_x86
     }
     contains(ANDROID_TARGET_ARCH,arm64-v8a){
         LIBS += -L$$PWD/../build/bin/platforminputcontexts/android -lsoftKeyboardPlugin_arm64-v8a
     }
     contains(ANDROID_TARGET_ARCH,armeabi-v7a){
         LIBS += -L$$PWD/../build/bin/platforminputcontexts/android -lsoftKeyboardPlugin_armeabi-v7a
     }
}

INCLUDEPATH += $$PWD/../plugin
DEPENDPATH += $$PWD/../plugin

DESTDIR = $$PWD/../build/bin


dict_files.files += $$PWD/../googlepinyin/dict/*
dict_files.path = /assets/dict
INSTALLS += dict_files

