QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#DESTDIR = $$PWD/bin
#QMAKE_OUT_DIR += /Users/hehong/git/sqtp_cpp/app_trader_client/bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    apiadapter.cpp \
    dialoglogin.cpp \
    globalsetting.cpp \
    main.cpp \
    mainwindow.cpp \
    placeorderform.cpp \
    tabbestquot.cpp \
    taboptparam.cpp \
    taborders.cpp

HEADERS += \
    apiadapter.h \
    datafeed_struct.h \
    dialoglogin.h \
    globalsetting.h \
    mainwindow.h \
    placeorderform.h \
    tabbestquot.h \
    taboptparam.h \
    taborders.h

# Default rules for deployment.
#qnx: target.path = ./bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target



FORMS += \
    dialoglogin.ui \
    placeorderform.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/release/ -lframe
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/debug/ -lframe
else:unix: LIBS += -L$$PWD/../lib/ -lframe


INCLUDEPATH += $$PWD/../libframe/include
INCLUDEPATH += $$PWD/../libsqtp/include
INCLUDEPATH += $$PWD/../libsqbz/include

DEPENDPATH += $$PWD/../libframe/include

macx {
    QMAKE_POST_LINK += cp $$PWD/config.xml $$OUT_PWD/app_trader_client.app/Contents/MacOS/
}

RESOURCES +=
