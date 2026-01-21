/********************************************************************************
** Form generated from reading UI file 'yuvrgbplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YUVRGBPLAYER_H
#define UI_YUVRGBPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_YuvRgbPlayerClass
{
public:

    void setupUi(QWidget *YuvRgbPlayerClass)
    {
        if (YuvRgbPlayerClass->objectName().isEmpty())
            YuvRgbPlayerClass->setObjectName(QString::fromUtf8("YuvRgbPlayerClass"));
        YuvRgbPlayerClass->resize(600, 400);

        retranslateUi(YuvRgbPlayerClass);

        QMetaObject::connectSlotsByName(YuvRgbPlayerClass);
    } // setupUi

    void retranslateUi(QWidget *YuvRgbPlayerClass)
    {
        YuvRgbPlayerClass->setWindowTitle(QCoreApplication::translate("YuvRgbPlayerClass", "YuvRgbPlayer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class YuvRgbPlayerClass: public Ui_YuvRgbPlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YUVRGBPLAYER_H
