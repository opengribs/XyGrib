/********************************************************************************
** Form generated from reading UI file 'CurveDrawerDialog.ui'
**
** Created: Thu May 19 08:01:34 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVEDRAWERDIALOG_H
#define UI_CURVEDRAWERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_CurveDrawerDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *lbData;
    QComboBox *cbData;
    QCheckBox *chkCurrent;
    QSpacerItem *horizontalSpacer;
    QPushButton *btExit;
    QwtPlot *qwtDataPlot;

    void setupUi(QDialog *CurveDrawerDialog)
    {
        if (CurveDrawerDialog->objectName().isEmpty())
            CurveDrawerDialog->setObjectName(QString::fromUtf8("CurveDrawerDialog"));
        CurveDrawerDialog->setWindowModality(Qt::NonModal);
        CurveDrawerDialog->resize(939, 739);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CurveDrawerDialog->sizePolicy().hasHeightForWidth());
        CurveDrawerDialog->setSizePolicy(sizePolicy);
        CurveDrawerDialog->setMinimumSize(QSize(0, 0));
        CurveDrawerDialog->setFocusPolicy(Qt::NoFocus);
        CurveDrawerDialog->setSizeGripEnabled(false);
        CurveDrawerDialog->setModal(false);
        horizontalLayout_2 = new QHBoxLayout(CurveDrawerDialog);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        lbData = new QLabel(CurveDrawerDialog);
        lbData->setObjectName(QString::fromUtf8("lbData"));
        lbData->setEnabled(true);

        horizontalLayout->addWidget(lbData);

        cbData = new QComboBox(CurveDrawerDialog);
        cbData->setObjectName(QString::fromUtf8("cbData"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(cbData->sizePolicy().hasHeightForWidth());
        cbData->setSizePolicy(sizePolicy1);
        cbData->setMinimumSize(QSize(194, 24));

        horizontalLayout->addWidget(cbData);

        chkCurrent = new QCheckBox(CurveDrawerDialog);
        chkCurrent->setObjectName(QString::fromUtf8("chkCurrent"));
        chkCurrent->setCheckable(true);
        chkCurrent->setChecked(true);

        horizontalLayout->addWidget(chkCurrent);

        horizontalSpacer = new QSpacerItem(120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btExit = new QPushButton(CurveDrawerDialog);
        btExit->setObjectName(QString::fromUtf8("btExit"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btExit->sizePolicy().hasHeightForWidth());
        btExit->setSizePolicy(sizePolicy2);
        btExit->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8("./data/img/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        btExit->setIcon(icon);
        btExit->setCheckable(false);
        btExit->setAutoDefault(false);
        btExit->setFlat(false);

        horizontalLayout->addWidget(btExit);


        verticalLayout->addLayout(horizontalLayout);

        qwtDataPlot = new QwtPlot(CurveDrawerDialog);
        qwtDataPlot->setObjectName(QString::fromUtf8("qwtDataPlot"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(2);
        sizePolicy3.setHeightForWidth(qwtDataPlot->sizePolicy().hasHeightForWidth());
        qwtDataPlot->setSizePolicy(sizePolicy3);
        qwtDataPlot->setMinimumSize(QSize(650, 600));
        qwtDataPlot->setMaximumSize(QSize(16777215, 16777215));
        qwtDataPlot->setCursor(QCursor(Qt::CrossCursor));
        qwtDataPlot->setMouseTracking(false);
        qwtDataPlot->setFocusPolicy(Qt::TabFocus);
        qwtDataPlot->setFrameShape(QFrame::Panel);
        qwtDataPlot->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(qwtDataPlot);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(CurveDrawerDialog);

        QMetaObject::connectSlotsByName(CurveDrawerDialog);
    } // setupUi

    void retranslateUi(QDialog *CurveDrawerDialog)
    {
        CurveDrawerDialog->setWindowTitle(QApplication::translate("CurveDrawerDialog", "Data Plot", 0, QApplication::UnicodeUTF8));
        lbData->setText(QApplication::translate("CurveDrawerDialog", "Data", 0, QApplication::UnicodeUTF8));
        chkCurrent->setText(QApplication::translate("CurveDrawerDialog", "Current TimeView", 0, QApplication::UnicodeUTF8));
        btExit->setText(QApplication::translate("CurveDrawerDialog", "Exit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CurveDrawerDialog: public Ui_CurveDrawerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVEDRAWERDIALOG_H
