QT += network

# Add more folders to ship with the application, here
qmldir.source = qml/MeeTeleGram
qmldir.target = qml
l10ndir.source = l10n
DEPLOYMENTFOLDERS = qmldir l10ndir

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE6D91DC2

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
#CONFIG += mobility
#MOBILITY += location

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

#RESOURCES += \
#    MeeTeleGram.qrc

OTHER_FILES += \
    images/MeeTeleGram.png

splash.files = images/splash.jpeg images/splash-l.jpeg
splash.path = /opt/MeeTeleGram/
export(splash.files)
export(splash.path)

server_pubkey.files = tg-server.pub
server_pubkey.path = /opt/MeeTeleGram/
export(server_pubkey.files)
export(server_pubkey.path)

INSTALLS += splash server_pubkey

TGL_BUILD_PATH = build_$$QMAKE_HOST.arch
# workaround for meego scratchbox
contains(QMAKE_HOST.arch, x86_64):!isEmpty(MEEGO_VERSION_MAJOR) {
    TGL_BUILD_PATH = build_n9_x86
}

INCLUDEPATH += tgl/ tgl/$$TGL_BUILD_PATH
LIBS += -L $$PWD/tgl/$$TGL_BUILD_PATH/libs/ -ltgl -lz -lrt -lm   -lssl -lcrypto

lupdate_only {
SOURCES += qml/MeeTeleGram/*
  isEmpty(MEEGO_VERSION_MAJOR) {
    SOURCES += qml/MeeTeleGram_PC/*
  }
}

#QMAKE_CXXFLAGS +=NO_LOG_REDIRECT
isEmpty(MEEGO_VERSION_MAJOR) {
    QMAKE_CXXFLAGS += "-D PC_BUILD"
}

TRANSLATIONS += l10n/MeeTeleGram.fa.ts

include(src/src.pri)

DISTFILES += \
    qml/*

include(updateqm.pri)
