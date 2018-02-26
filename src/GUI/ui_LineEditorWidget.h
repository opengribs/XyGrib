/********************************************************************************
** Form generated from reading UI file 'LineEditorWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINEEDITORWIDGET_H
#define UI_LINEEDITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "LineEditorWidget.h"

QT_BEGIN_NAMESPACE

class Ui_LineEditorWidget
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QDoubleSpinBox *sbWidth;
    QFrame *frame;
    QGridLayout *gridLayout1;
    LineTestZone *testZone;
    QPushButton *resetButton;

    void setupUi(QWidget *LineEditorWidget)
    {
        if (LineEditorWidget->objectName().isEmpty())
            LineEditorWidget->setObjectName(QStringLiteral("LineEditorWidget"));
        LineEditorWidget->resize(253, 27);
        gridLayout = new QGridLayout(LineEditorWidget);
        gridLayout->setSpacing(4);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        sbWidth = new QDoubleSpinBox(LineEditorWidget);
        sbWidth->setObjectName(QStringLiteral("sbWidth"));
        sbWidth->setDecimals(1);
        sbWidth->setSingleStep(0.2);

        hboxLayout->addWidget(sbWidth);

        frame = new QFrame(LineEditorWidget);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(20, 20));
        frame->setAutoFillBackground(true);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout1 = new QGridLayout(frame);
        gridLayout1->setSpacing(0);
        gridLayout1->setContentsMargins(0, 0, 0, 0);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        testZone = new LineTestZone(frame);
        testZone->setObjectName(QStringLiteral("testZone"));
        testZone->setMinimumSize(QSize(100, 0));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 127, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        testZone->setPalette(palette);
        testZone->setCursor(QCursor(Qt::PointingHandCursor));
        testZone->setAutoFillBackground(true);

        gridLayout1->addWidget(testZone, 0, 0, 1, 1);


        hboxLayout->addWidget(frame);

        resetButton = new QPushButton(LineEditorWidget);
        resetButton->setObjectName(QStringLiteral("resetButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(resetButton->sizePolicy().hasHeightForWidth());
        resetButton->setSizePolicy(sizePolicy1);
        resetButton->setCursor(QCursor(Qt::ArrowCursor));

        hboxLayout->addWidget(resetButton);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);


        retranslateUi(LineEditorWidget);

        QMetaObject::connectSlotsByName(LineEditorWidget);
    } // setupUi

    void retranslateUi(QWidget *LineEditorWidget)
    {
        LineEditorWidget->setWindowTitle(QApplication::translate("LineEditorWidget", "Form", nullptr));
        resetButton->setText(QApplication::translate("LineEditorWidget", "Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LineEditorWidget: public Ui_LineEditorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINEEDITORWIDGET_H
