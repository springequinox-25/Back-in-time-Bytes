QT += widgets
QT += core gui widgets testlib
QT += multimedia

//QT += core gui multimedia

CONFIG += c++20

HEADERS += \
    fifteenpuzzle.h \
    mainmenu.h \
    settingswindow.h \
    snakegame.h \
    TicTacToe.h \
    game2048.h \
    tictactoesetting.h

SOURCES += \
    fifteenpuzzle.cpp \
    main.cpp \
    mainmenu.cpp \
    settingswindow.cpp \
    snakegame.cpp \
    TicTacToe.cpp \
    game2048.cpp \
    tictactoesetting.cpp


DISTFILES += \
    desertBG.jpg

RESOURCES += \
    resources.qrc




