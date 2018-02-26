/********************************************************************************
** Form generated from reading UI file 'TextStyleEditorWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTSTYLEEDITORWIDGET_H
#define UI_TEXTSTYLEEDITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "TextStyleEditorWidget.h"

QT_BEGIN_NAMESPACE

class Ui_TextStyleEditorWidget
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QFrame *frame_3;
    QHBoxLayout *hboxLayout1;
    TextDisplayLabel *testZoneLabel;
    QFrame *frame;
    QGridLayout *gridLayout1;
    TextColorWidget *textColorWidget;
    QFrame *frame_2;
    QGridLayout *gridLayout2;
    TextColorWidget *bgColorWidget;
    QPushButton *resetButton;

    void setupUi(QWidget *TextStyleEditorWidget)
    {
        if (TextStyleEditorWidget->objectName().isEmpty())
            TextStyleEditorWidget->setObjectName(QStringLiteral("TextStyleEditorWidget"));
        TextStyleEditorWidget->resize(405, 45);
        gridLayout = new QGridLayout(TextStyleEditorWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        frame_3 = new QFrame(TextStyleEditorWidget);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy);
        frame_3->setMinimumSize(QSize(60, 20));
        frame_3->setAutoFillBackground(false);
        frame_3->setFrameShape(QFrame::Box);
        frame_3->setFrameShadow(QFrame::Raised);
        hboxLayout1 = new QHBoxLayout(frame_3);
        hboxLayout1->setSpacing(0);
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        testZoneLabel = new TextDisplayLabel(frame_3);
        testZoneLabel->setObjectName(QStringLiteral("testZoneLabel"));
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
        testZoneLabel->setPalette(palette);
        testZoneLabel->setCursor(QCursor(Qt::PointingHandCursor));
        testZoneLabel->setAutoFillBackground(true);

        hboxLayout1->addWidget(testZoneLabel);


        hboxLayout->addWidget(frame_3);

        frame = new QFrame(TextStyleEditorWidget);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setMinimumSize(QSize(28, 20));
        frame->setAutoFillBackground(true);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout1 = new QGridLayout(frame);
        gridLayout1->setSpacing(0);
        gridLayout1->setContentsMargins(0, 0, 0, 0);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        textColorWidget = new TextColorWidget(frame);
        textColorWidget->setObjectName(QStringLiteral("textColorWidget"));
        sizePolicy1.setHeightForWidth(textColorWidget->sizePolicy().hasHeightForWidth());
        textColorWidget->setSizePolicy(sizePolicy1);
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush2(QColor(255, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        textColorWidget->setPalette(palette1);
        textColorWidget->setCursor(QCursor(Qt::PointingHandCursor));
        textColorWidget->setAutoFillBackground(true);

        gridLayout1->addWidget(textColorWidget, 0, 0, 1, 1);


        hboxLayout->addWidget(frame);

        frame_2 = new QFrame(TextStyleEditorWidget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setMinimumSize(QSize(28, 20));
        frame_2->setAutoFillBackground(true);
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout2 = new QGridLayout(frame_2);
        gridLayout2->setSpacing(0);
        gridLayout2->setContentsMargins(0, 0, 0, 0);
        gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
        bgColorWidget = new TextColorWidget(frame_2);
        bgColorWidget->setObjectName(QStringLiteral("bgColorWidget"));
        sizePolicy1.setHeightForWidth(bgColorWidget->sizePolicy().hasHeightForWidth());
        bgColorWidget->setSizePolicy(sizePolicy1);
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Base, brush);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        bgColorWidget->setPalette(palette2);
        bgColorWidget->setCursor(QCursor(Qt::PointingHandCursor));
        bgColorWidget->setAutoFillBackground(true);

        gridLayout2->addWidget(bgColorWidget, 0, 0, 1, 1);


        hboxLayout->addWidget(frame_2);

        resetButton = new QPushButton(TextStyleEditorWidget);
        resetButton->setObjectName(QStringLiteral("resetButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(resetButton->sizePolicy().hasHeightForWidth());
        resetButton->setSizePolicy(sizePolicy2);
        resetButton->setCursor(QCursor(Qt::ArrowCursor));

        hboxLayout->addWidget(resetButton);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);


        retranslateUi(TextStyleEditorWidget);

        QMetaObject::connectSlotsByName(TextStyleEditorWidget);
    } // setupUi

    void retranslateUi(QWidget *TextStyleEditorWidget)
    {
        TextStyleEditorWidget->setWindowTitle(QApplication::translate("TextStyleEditorWidget", "Form", nullptr));
        testZoneLabel->setText(QApplication::translate("TextStyleEditorWidget", "TextLabel", nullptr));
#ifndef QT_NO_TOOLTIP
        textColorWidget->setToolTip(QApplication::translate("TextStyleEditorWidget", "Text color", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        bgColorWidget->setToolTip(QApplication::translate("TextStyleEditorWidget", "Background color", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        resetButton->setToolTip(QApplication::translate("TextStyleEditorWidget", "Default values", nullptr));
#endif // QT_NO_TOOLTIP
        resetButton->setText(QApplication::translate("TextStyleEditorWidget", "Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextStyleEditorWidget: public Ui_TextStyleEditorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTSTYLEEDITORWIDGET_H
