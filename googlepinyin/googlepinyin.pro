QT          -= gui

TEMPLATE    = lib

SOURCES     += \
            dictbuilder.cpp \
            dictlist.cpp \
            dicttrie.cpp \
            lpicache.cpp \
            matrixsearch.cpp \
            mystdlib.cpp \
            ngram.cpp \
            pinyinime.cpp \
            searchutility.cpp \
            spellingtable.cpp \
            spellingtrie.cpp \
            splparser.cpp \
            sync.cpp \
            userdict.cpp \
            utf16char.cpp \
            utf16reader.cpp

HEADERS     += \
            atomdictbase.h \
            dictbuilder.h \
            dictdef.h \
            dictlist.h \
            dicttrie.h \
            lpicache.h \
            matrixsearch.h \
            mystdlib.h \
            ngram.h \
            pinyinime.h \
            searchutility.h \
            spellingtable.h \
            spellingtrie.h \
            splparser.h \
            sync.h \
            userdict.h \
            utf16char.h \
            utf16reader.h

CONFIG      += staticlib
TARGET      = googlepinyin

win32{
#    CONFIG      += debug_and_release build_all
    
    CONFIG(debug, debug|release){
        TARGET  = $$join(TARGET,,,d)
    }
    CONFIG(release, debug|release){
        TARGET  = $$TARGET
    }
}


msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

win32:{
    LIB_PATH = googlepinyin
}else:unix:!macx:{
    contains(ANDROID_TARGET_ARCH,x86_64){
         LIB_PATH = googlepinyin_x86_64
     }
     contains(ANDROID_TARGET_ARCH,x86){
         LIB_PATH = googlepinyin_x86
     }
     contains(ANDROID_TARGET_ARCH,arm64-v8a){
         LIB_PATH = googlepinyin_arm64-v8a
     }
     contains(ANDROID_TARGET_ARCH,armeabi-v7a){
         LIB_PATH = googlepinyin_armeabi-v7a
     }
}

DESTDIR     = $$PWD/../build/lib/$${LIB_PATH}
