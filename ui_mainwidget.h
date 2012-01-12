/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created: Thu Jan 12 08:47:09 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyWidget
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QCheckBox *fill;
    QCheckBox *points;
    QCheckBox *wireframe;
    QLabel *label_4;
    QSlider *shininess;
    QSlider *red;
    QSlider *green;
    QSlider *blue;
    QSlider *alpha;
    QLabel *label_5;
    QLabel *label_6;
    QSlider *specular;
    QComboBox *combo;

    void setupUi(QWidget *MyWidget)
    {
        if (MyWidget->objectName().isEmpty())
            MyWidget->setObjectName(QString::fromUtf8("MyWidget"));
        MyWidget->resize(510, 268);
        label = new QLabel(MyWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(50, 40, 44, 16));
        label_2 = new QLabel(MyWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 90, 44, 16));
        label_3 = new QLabel(MyWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 70, 44, 16));
        fill = new QCheckBox(MyWidget);
        fill->setObjectName(QString::fromUtf8("fill"));
        fill->setGeometry(QRect(60, 210, 74, 21));
        points = new QCheckBox(MyWidget);
        points->setObjectName(QString::fromUtf8("points"));
        points->setGeometry(QRect(133, 220, 71, 21));
        wireframe = new QCheckBox(MyWidget);
        wireframe->setObjectName(QString::fromUtf8("wireframe"));
        wireframe->setGeometry(QRect(230, 220, 74, 21));
        label_4 = new QLabel(MyWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(30, 120, 44, 16));
        shininess = new QSlider(MyWidget);
        shininess->setObjectName(QString::fromUtf8("shininess"));
        shininess->setGeometry(QRect(90, 190, 251, 23));
        shininess->setMaximum(1280);
        shininess->setOrientation(Qt::Horizontal);
        red = new QSlider(MyWidget);
        red->setObjectName(QString::fromUtf8("red"));
        red->setGeometry(QRect(100, 40, 211, 23));
        red->setMaximum(1000);
        red->setOrientation(Qt::Horizontal);
        green = new QSlider(MyWidget);
        green->setObjectName(QString::fromUtf8("green"));
        green->setGeometry(QRect(99, 60, 221, 23));
        green->setMaximum(1000);
        green->setOrientation(Qt::Horizontal);
        blue = new QSlider(MyWidget);
        blue->setObjectName(QString::fromUtf8("blue"));
        blue->setGeometry(QRect(90, 90, 231, 23));
        blue->setMaximum(1000);
        blue->setOrientation(Qt::Horizontal);
        alpha = new QSlider(MyWidget);
        alpha->setObjectName(QString::fromUtf8("alpha"));
        alpha->setGeometry(QRect(90, 120, 241, 23));
        alpha->setMaximum(1000);
        alpha->setOrientation(Qt::Horizontal);
        label_5 = new QLabel(MyWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 190, 51, 16));
        label_6 = new QLabel(MyWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 160, 81, 16));
        specular = new QSlider(MyWidget);
        specular->setObjectName(QString::fromUtf8("specular"));
        specular->setGeometry(QRect(120, 160, 251, 23));
        specular->setMaximum(1000);
        specular->setOrientation(Qt::Horizontal);
        combo = new QComboBox(MyWidget);
        combo->setObjectName(QString::fromUtf8("combo"));
        combo->setGeometry(QRect(40, 10, 71, 22));

        retranslateUi(MyWidget);

        QMetaObject::connectSlotsByName(MyWidget);
    } // setupUi

    void retranslateUi(QWidget *MyWidget)
    {
        MyWidget->setWindowTitle(QApplication::translate("MyWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MyWidget", "Red", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MyWidget", "Blue", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MyWidget", "Green", 0, QApplication::UnicodeUTF8));
        fill->setText(QApplication::translate("MyWidget", "fill", 0, QApplication::UnicodeUTF8));
        points->setText(QApplication::translate("MyWidget", "points", 0, QApplication::UnicodeUTF8));
        wireframe->setText(QApplication::translate("MyWidget", "wireframe", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MyWidget", "Alpha", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MyWidget", "Shininess", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MyWidget", "Specular [X,X,X]", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MyWidget: public Ui_MyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
