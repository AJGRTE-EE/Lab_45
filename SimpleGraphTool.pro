#-------------------------------------------------
#
# Project created by QtCreator 2019-06-05T10:02:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleGraphTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        dvninputdialog.cpp \
        graphics\sources\EdgeGraphicsItem.cpp \
        basis\sources\Graph.cpp \
        graphics\sources\GraphGraphicsScene.cpp \
        graphics\sources\GraphGraphicsView.cpp \
        widgets\sources\AdjacencyMatrixTable.cpp \
        basis\sources\GraphUtils.cpp \
        widgets\sources\MultiLineInputDialog.cpp \
        main.cpp \
        mainwindow.cpp \
        basis\sources\Node.cpp \
        graphics\sources\NodeGraphicsItem.cpp \
        widgets\sources\ElementPropertiesTable.cpp \
        widgets\sources\GraphPropertiesTable.cpp \
        widgets\sources\GraphOptionDialog.cpp \
        widgets\sources\IncidenceMatrixTable.cpp \
        widgets\sources\MultiComboboxDialog.cpp

HEADERS += \
    dvninputdialog.h \
    graphics\headers\EdgeGraphicsItem.h \
    graphics\headers\GraphGraphicsScene.h \
    graphics\headers\GraphGraphicsView.h \
    graphics\headers\NodeGraphicsItem.h \
    widgets\headers\AdjacencyMatrixTable.h \
    basis\headers\Graph.h \
    basis\headers\GraphUtils.h \
    widgets\headers\MultiLineInputDialog.h \
    widgets\headers\ElementPropertiesTable.h \
    widgets\headers\GraphPropertiesTable.h \
    widgets\headers\GraphOptionDialog.h \
    basis\headers\Edge.h \
    widgets\headers\IncidenceMatrixTable.h \
    basis\headers\Matrix.h \
    utils\random.h \
    utils\qdebugstream.h \
    basis\headers\Node.h \
    widgets\headers\MultiComboboxDialog.h \
    mainwindow.h

FORMS += \
        mainwindow.ui

#win32:RC_ICONS += C:\Users\btnt51\Documents\simple-graph-tool\icon.ico
