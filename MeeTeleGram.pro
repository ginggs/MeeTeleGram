QT += network

# Add more folders to ship with the application, here
folder_01.source = qml/MeeTeleGram
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

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

INCLUDEPATH += tgl/ tgl/build
LIBS += -L $$PWD/tgl/build/libs/ -ltgl -lz -lrt -lm   -lssl -lcrypto

include(src/src.pri)

DISTFILES += \
    qml/MeeTeleGram/MainPage.qml
