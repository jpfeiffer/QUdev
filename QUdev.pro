#-------------------------------------------------
#
# Project created by QtCreator 2011-01-04T09:20:42
#
#-------------------------------------------------

QT       -= gui

TARGET = QUdev
TEMPLATE = lib

DEFINES += QUDEV_LIBRARY

SOURCES += QUdev.cpp \
    QUdev_private.cpp

HEADERS += QUdev.h\
        QUdev_global.h \
    QUdevDeclarations.h \
    QUdev_private.h

symbian {
    #Symbian specific definitions
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2FFA84F
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QUdev.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target

    LIBS += -ludev
}
