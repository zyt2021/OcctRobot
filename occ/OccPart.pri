Qt += opengl

OCCTLIB_PATH = D:/OpenCasCade

HEADERS += \
    $$PWD/CafShapePrs.h \
    $$PWD/ModelImport.h \
    $$PWD/OccWidget.h \
    $$PWD/XCAFBroswer.h

SOURCES += \
    $$PWD/CafShapePrs.cpp \
    $$PWD/ModelImport.cpp \
    $$PWD/OccWidget.cpp \
    $$PWD/XCAFBroswer.cpp

win32 {
    contains(QT_ARCH, x86_64){
        contains(QMAKE_MSC_VER, 1916){
            INCLUDEPATH += $$OCCTLIB_PATH/inc
            LIBS += $$OCCTLIB_PATH/lib/*.lib
            message("using msvc")
        }else{
            mingw{
                INCLUDEPATH += $$OCCTLIB_PATH/inc
                LIBS += $$OCCTLIB_PATH/lib/lib*.a
                message("using mingw")
            }else{
                message("wrong kit config")
            }
        }
    }else{
        message("wrong system version")
    }
}
