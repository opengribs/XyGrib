/********************************************************************************
** Form generated from reading UI file 'DataPlotDialog.ui'
**
** Created: Mon Dec 20 22:45:54 2010
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAPLOTDIALOG_H
#define UI_DATAPLOTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_DataPlotDialog
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QwtPlot *qwtDataPlot;
    QHBoxLayout *horizontalLayout;
    QComboBox *cbData;
    QDialogButtonBox *btBox;

    void setupUi(QDialog *DataPlotDialog)
    {
        if (DataPlotDialog->objectName().isEmpty())
            DataPlotDialog->setObjectName(QString::fromUtf8("DataPlotDialog"));
        DataPlotDialog->setWindowModality(Qt::NonModal);
        DataPlotDialog->resize(939, 739);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DataPlotDialog->sizePolicy().hasHeightForWidth());
        DataPlotDialog->setSizePolicy(sizePolicy);
        DataPlotDialog->setMinimumSize(QSize(0, 0));
        DataPlotDialog->setFocusPolicy(Qt::NoFocus);
        DataPlotDialog->setSizeGripEnabled(false);
        DataPlotDialog->setModal(false);
        horizontalLayout_2 = new QHBoxLayout(DataPlotDialog);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qwtDataPlot = new QwtPlot(DataPlotDialog);
        qwtDataPlot->setObjectName(QString::fromUtf8("qwtDataPlot"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(2);
        sizePolicy1.setHeightForWidth(qwtDataPlot->sizePolicy().hasHeightForWidth());
        qwtDataPlot->setSizePolicy(sizePolicy1);
        qwtDataPlot->setMinimumSize(QSize(650, 600));
        qwtDataPlot->setMaximumSize(QSize(16777215, 16777215));
        qwtDataPlot->setCursor(QCursor(Qt::CrossCursor));
        qwtDataPlot->setMouseTracking(false);
        qwtDataPlot->setFocusPolicy(Qt::TabFocus);
        qwtDataPlot->setFrameShape(QFrame::Panel);
        qwtDataPlot->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(qwtDataPlot);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        cbData = new QComboBox(DataPlotDialog);
        cbData->setObjectName(QString::fromUtf8("cbData"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(cbData->sizePolicy().hasHeightForWidth());
        cbData->setSizePolicy(sizePolicy2);
        cbData->setMinimumSize(QSize(194, 24));

        horizontalLayout->addWidget(cbData);

        btBox = new QDialogButtonBox(DataPlotDialog);
        btBox->setObjectName(QString::fromUtf8("btBox"));
        btBox->setEnabled(true);
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btBox->sizePolicy().hasHeightForWidth());
        btBox->setSizePolicy(sizePolicy3);
        btBox->setMinimumSize(QSize(194, 32));
        btBox->setOrientation(Qt::Horizontal);
        btBox->setStandardButtons(QDialogButtonBox::Ok);
        btBox->setCenterButtons(false);

        horizontalLayout->addWidget(btBox);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(DataPlotDialog);

        QMetaObject::connectSlotsByName(DataPlotDialog);
    } // setupUi

    void retranslateUi(QDialog *DataPlotDialog)
    {
        DataPlotDialog->setWindowTitle(QApplication::translate("DataPlotDialog", "Data Plot", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DataPlotDialog: public Ui_DataPlotDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAPLOTDIALOG_H
