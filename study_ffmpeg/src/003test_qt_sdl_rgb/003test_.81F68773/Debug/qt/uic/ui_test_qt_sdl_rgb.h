/********************************************************************************
** Form generated from reading UI file 'test_qt_sdl_rgb.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_QT_SDL_RGB_H
#define UI_TEST_QT_SDL_RGB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_test_qt_sdl_rgbClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *test_qt_sdl_rgbClass)
    {
        if (test_qt_sdl_rgbClass->objectName().isEmpty())
            test_qt_sdl_rgbClass->setObjectName(QString::fromUtf8("test_qt_sdl_rgbClass"));
        test_qt_sdl_rgbClass->resize(600, 400);
        menuBar = new QMenuBar(test_qt_sdl_rgbClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        test_qt_sdl_rgbClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(test_qt_sdl_rgbClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        test_qt_sdl_rgbClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(test_qt_sdl_rgbClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        test_qt_sdl_rgbClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(test_qt_sdl_rgbClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        test_qt_sdl_rgbClass->setStatusBar(statusBar);

        retranslateUi(test_qt_sdl_rgbClass);

        QMetaObject::connectSlotsByName(test_qt_sdl_rgbClass);
    } // setupUi

    void retranslateUi(QMainWindow *test_qt_sdl_rgbClass)
    {
        test_qt_sdl_rgbClass->setWindowTitle(QCoreApplication::translate("test_qt_sdl_rgbClass", "test_qt_sdl_rgb", nullptr));
    } // retranslateUi

};

namespace Ui {
    class test_qt_sdl_rgbClass: public Ui_test_qt_sdl_rgbClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_QT_SDL_RGB_H
