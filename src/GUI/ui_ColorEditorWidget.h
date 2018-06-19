/********************************************************************************
** Form generated from reading UI file 'ColorEditorWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLOREDITORWIDGET_H
#define UI_COLOREDITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "ColorEditorWidget.h"

QT_BEGIN_NAMESPACE

class Ui_ColorEditorWidget
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QFrame *frame;
    QGridLayout *gridLayout;
    ColorTestZone *testZone;
    QPushButton *resetButton;

    void setupUi(QWidget *ColorEditorWidget)
    {
        if (ColorEditorWidget->objectName().isEmpty())
            ColorEditorWidget->setObjectName(QStringLiteral("ColorEditorWidget"));
        ColorEditorWidget->resize(253, 27);
        vboxLayout = new QVBoxLayout(ColorEditorWidget);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        frame = new QFrame(ColorEditorWidget);
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
        gridLayout = new QGridLayout(frame);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        testZone = new ColorTestZone(frame);
        testZone->setObjectName(QStringLiteral("testZone"));
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

        gridLayout->addWidget(testZone, 0, 0, 1, 1);


        hboxLayout->addWidget(frame);

        resetButton = new QPushButton(ColorEditorWidget);
        resetButton->setObjectName(QStringLiteral("resetButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(resetButton->sizePolicy().hasHeightForWidth());
        resetButton->setSizePolicy(sizePolicy1);
        resetButton->setCursor(QCursor(Qt::ArrowCursor));

        hboxLayout->addWidget(resetButton);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(ColorEditorWidget);

        QMetaObject::connectSlotsByName(ColorEditorWidget);
    } // setupUi

    void retranslateUi(QWidget *ColorEditorWidget)
    {
        ColorEditorWidget->setWindowTitle(QApplication::translate("ColorEditorWidget", "Form", nullptr));
        resetButton->setText(QApplication::translate("ColorEditorWidget", "Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ColorEditorWidget: public Ui_ColorEditorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLOREDITORWIDGET_H
