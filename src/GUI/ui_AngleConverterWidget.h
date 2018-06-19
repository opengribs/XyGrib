/********************************************************************************
** Form generated from reading UI file 'AngleConverterWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANGLECONVERTERWIDGET_H
#define UI_ANGLECONVERTERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AngleConverterWidget
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout1;
    QLabel *label;
    QHBoxLayout *hboxLayout;
    QSpinBox *dddmmss_deg;
    QSpinBox *dddmmss_min;
    QDoubleSpinBox *dddmmss_sec;
    QLabel *label_2;
    QHBoxLayout *hboxLayout1;
    QSpinBox *dddmm_deg;
    QDoubleSpinBox *dddmm_min;
    QLabel *label_3;
    QDoubleSpinBox *ddd_deg;
    QLabel *label_4;
    QDoubleSpinBox *ddd_rad;
    QLabel *label_5;
    QDoubleSpinBox *ddd_grad;

    void setupUi(QWidget *AngleConverterWidget)
    {
        if (AngleConverterWidget->objectName().isEmpty())
            AngleConverterWidget->setObjectName(QStringLiteral("AngleConverterWidget"));
        AngleConverterWidget->resize(368, 180);
        AngleConverterWidget->setStyleSheet(QStringLiteral(""));
        gridLayout = new QGridLayout(AngleConverterWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(AngleConverterWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setAutoFillBackground(false);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout1 = new QGridLayout(frame);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label, 0, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        dddmmss_deg = new QSpinBox(frame);
        dddmmss_deg->setObjectName(QStringLiteral("dddmmss_deg"));
        dddmmss_deg->setMinimum(0);
        dddmmss_deg->setMaximum(10000000);

        hboxLayout->addWidget(dddmmss_deg);

        dddmmss_min = new QSpinBox(frame);
        dddmmss_min->setObjectName(QStringLiteral("dddmmss_min"));
        dddmmss_min->setMinimum(0);
        dddmmss_min->setMaximum(59);

        hboxLayout->addWidget(dddmmss_min);

        dddmmss_sec = new QDoubleSpinBox(frame);
        dddmmss_sec->setObjectName(QStringLiteral("dddmmss_sec"));
        dddmmss_sec->setDecimals(4);
        dddmmss_sec->setMaximum(60);
        dddmmss_sec->setSingleStep(0.1);

        hboxLayout->addWidget(dddmmss_sec);


        gridLayout1->addLayout(hboxLayout, 0, 1, 1, 1);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        dddmm_deg = new QSpinBox(frame);
        dddmm_deg->setObjectName(QStringLiteral("dddmm_deg"));
        dddmm_deg->setMinimum(0);
        dddmm_deg->setMaximum(10000000);

        hboxLayout1->addWidget(dddmm_deg);

        dddmm_min = new QDoubleSpinBox(frame);
        dddmm_min->setObjectName(QStringLiteral("dddmm_min"));
        dddmm_min->setDecimals(6);
        dddmm_min->setMaximum(60);
        dddmm_min->setSingleStep(0.1);

        hboxLayout1->addWidget(dddmm_min);


        gridLayout1->addLayout(hboxLayout1, 1, 1, 1, 1);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label_3, 2, 0, 1, 1);

        ddd_deg = new QDoubleSpinBox(frame);
        ddd_deg->setObjectName(QStringLiteral("ddd_deg"));
        ddd_deg->setDecimals(8);
        ddd_deg->setMinimum(0);
        ddd_deg->setMaximum(1e+7);
        ddd_deg->setSingleStep(0.1);

        gridLayout1->addWidget(ddd_deg, 2, 1, 1, 1);

        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label_4, 3, 0, 1, 1);

        ddd_rad = new QDoubleSpinBox(frame);
        ddd_rad->setObjectName(QStringLiteral("ddd_rad"));
        ddd_rad->setDecimals(8);
        ddd_rad->setMinimum(0);
        ddd_rad->setMaximum(1e+7);
        ddd_rad->setSingleStep(0.1);

        gridLayout1->addWidget(ddd_rad, 3, 1, 1, 1);

        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label_5, 4, 0, 1, 1);

        ddd_grad = new QDoubleSpinBox(frame);
        ddd_grad->setObjectName(QStringLiteral("ddd_grad"));
        ddd_grad->setDecimals(8);
        ddd_grad->setMinimum(0);
        ddd_grad->setMaximum(1e+7);
        ddd_grad->setSingleStep(0.1);

        gridLayout1->addWidget(ddd_grad, 4, 1, 1, 1);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(AngleConverterWidget);

        QMetaObject::connectSlotsByName(AngleConverterWidget);
    } // setupUi

    void retranslateUi(QWidget *AngleConverterWidget)
    {
        AngleConverterWidget->setWindowTitle(QApplication::translate("AngleConverterWidget", "Angle", nullptr));
        label->setText(QApplication::translate("AngleConverterWidget", "ddd\302\260 mm' ss\"", nullptr));
        dddmmss_deg->setSuffix(QApplication::translate("AngleConverterWidget", " \302\260", nullptr));
        dddmmss_min->setSuffix(QApplication::translate("AngleConverterWidget", " '", nullptr));
        dddmmss_sec->setSuffix(QApplication::translate("AngleConverterWidget", " \"", nullptr));
        label_2->setText(QApplication::translate("AngleConverterWidget", "ddd\302\260 mm.mm'", nullptr));
        dddmm_deg->setSuffix(QApplication::translate("AngleConverterWidget", " \302\260", nullptr));
        dddmm_min->setSuffix(QApplication::translate("AngleConverterWidget", " '", nullptr));
        label_3->setText(QApplication::translate("AngleConverterWidget", "ddd.dddd\302\260", nullptr));
        ddd_deg->setSuffix(QApplication::translate("AngleConverterWidget", " \302\260", nullptr));
        label_4->setText(QApplication::translate("AngleConverterWidget", "radians", nullptr));
        ddd_rad->setSuffix(QApplication::translate("AngleConverterWidget", " rd", nullptr));
        label_5->setText(QApplication::translate("AngleConverterWidget", "grades", nullptr));
        ddd_grad->setSuffix(QApplication::translate("AngleConverterWidget", " gr", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AngleConverterWidget: public Ui_AngleConverterWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANGLECONVERTERWIDGET_H
