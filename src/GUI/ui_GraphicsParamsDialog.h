/********************************************************************************
** Form generated from reading UI file 'GraphicsParamsDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICSPARAMSDIALOG_H
#define UI_GRAPHICSPARAMSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "ColorEditorWidget.h"
#include "LineEditorWidget.h"

QT_BEGIN_NAMESPACE

class Ui_GraphicsParamsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_5;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label_14;
    QLabel *label_5;
    ColorEditorWidget *inputBgColor;
    QLabel *label_6;
    ColorEditorWidget *inputSeaColor;
    QLabel *label_7;
    ColorEditorWidget *inputLandColor;
    QLabel *label_8;
    LineEditorWidget *inputSeaBordersLine;
    QLabel *label_9;
    LineEditorWidget *inputBoundariesLine;
    QLabel *label_10;
    LineEditorWidget *inputRiversLine;
    QLabel *label_11;
    LineEditorWidget *inputIsobarsLine;
    QLabel *label_12;
    LineEditorWidget *inputIsotherms0Line;
    LineEditorWidget *inputIsotherms_Line;
    QLabel *label_13;
    QGridLayout *gridLayout;
    QComboBox *inputCloudsColorMode;
    QSpacerItem *spacerItem;
    QLabel *label_15;
    LineEditorWidget *inputThetaE_Line;
    QSpacerItem *verticalSpacer;
    QFrame *frame;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *btOK;
    QPushButton *btCancel;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *GraphicsParamsDialog)
    {
        if (GraphicsParamsDialog->objectName().isEmpty())
            GraphicsParamsDialog->setObjectName(QStringLiteral("GraphicsParamsDialog"));
        GraphicsParamsDialog->resize(517, 444);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(GraphicsParamsDialog->sizePolicy().hasHeightForWidth());
        GraphicsParamsDialog->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(GraphicsParamsDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame_5 = new QFrame(GraphicsParamsDialog);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_5->sizePolicy().hasHeightForWidth());
        frame_5->setSizePolicy(sizePolicy1);
        frame_5->setAutoFillBackground(false);
        frame_5->setFrameShape(QFrame::Box);
        frame_5->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(frame_5);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_14 = new QLabel(frame_5);
        label_14->setObjectName(QStringLiteral("label_14"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_14->setFont(font);
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_14, 8, 0, 1, 1);

        label_5 = new QLabel(frame_5);
        label_5->setObjectName(QStringLiteral("label_5"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        inputBgColor = new ColorEditorWidget(frame_5);
        inputBgColor->setObjectName(QStringLiteral("inputBgColor"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(inputBgColor->sizePolicy().hasHeightForWidth());
        inputBgColor->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(inputBgColor, 0, 1, 1, 1);

        label_6 = new QLabel(frame_5);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_6, 1, 0, 1, 1);

        inputSeaColor = new ColorEditorWidget(frame_5);
        inputSeaColor->setObjectName(QStringLiteral("inputSeaColor"));

        gridLayout_2->addWidget(inputSeaColor, 1, 1, 1, 1);

        label_7 = new QLabel(frame_5);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFont(font);
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_7, 2, 0, 1, 1);

        inputLandColor = new ColorEditorWidget(frame_5);
        inputLandColor->setObjectName(QStringLiteral("inputLandColor"));

        gridLayout_2->addWidget(inputLandColor, 2, 1, 1, 1);

        label_8 = new QLabel(frame_5);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_8, 3, 0, 1, 1);

        inputSeaBordersLine = new LineEditorWidget(frame_5);
        inputSeaBordersLine->setObjectName(QStringLiteral("inputSeaBordersLine"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(inputSeaBordersLine->sizePolicy().hasHeightForWidth());
        inputSeaBordersLine->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(inputSeaBordersLine, 3, 1, 1, 1);

        label_9 = new QLabel(frame_5);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setFont(font);
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_9, 4, 0, 1, 1);

        inputBoundariesLine = new LineEditorWidget(frame_5);
        inputBoundariesLine->setObjectName(QStringLiteral("inputBoundariesLine"));
        sizePolicy4.setHeightForWidth(inputBoundariesLine->sizePolicy().hasHeightForWidth());
        inputBoundariesLine->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(inputBoundariesLine, 4, 1, 1, 1);

        label_10 = new QLabel(frame_5);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setFont(font);
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_10, 5, 0, 1, 1);

        inputRiversLine = new LineEditorWidget(frame_5);
        inputRiversLine->setObjectName(QStringLiteral("inputRiversLine"));
        sizePolicy4.setHeightForWidth(inputRiversLine->sizePolicy().hasHeightForWidth());
        inputRiversLine->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(inputRiversLine, 5, 1, 1, 1);

        label_11 = new QLabel(frame_5);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setFont(font);
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_11, 6, 0, 1, 1);

        inputIsobarsLine = new LineEditorWidget(frame_5);
        inputIsobarsLine->setObjectName(QStringLiteral("inputIsobarsLine"));
        sizePolicy4.setHeightForWidth(inputIsobarsLine->sizePolicy().hasHeightForWidth());
        inputIsobarsLine->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(inputIsobarsLine, 6, 1, 1, 1);

        label_12 = new QLabel(frame_5);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setFont(font);
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_12, 7, 0, 1, 1);

        inputIsotherms0Line = new LineEditorWidget(frame_5);
        inputIsotherms0Line->setObjectName(QStringLiteral("inputIsotherms0Line"));
        sizePolicy4.setHeightForWidth(inputIsotherms0Line->sizePolicy().hasHeightForWidth());
        inputIsotherms0Line->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(inputIsotherms0Line, 7, 1, 1, 1);

        inputIsotherms_Line = new LineEditorWidget(frame_5);
        inputIsotherms_Line->setObjectName(QStringLiteral("inputIsotherms_Line"));

        gridLayout_2->addWidget(inputIsotherms_Line, 8, 1, 1, 1);

        label_13 = new QLabel(frame_5);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setFont(font);
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_13, 10, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        inputCloudsColorMode = new QComboBox(frame_5);
        inputCloudsColorMode->setObjectName(QStringLiteral("inputCloudsColorMode"));
        sizePolicy4.setHeightForWidth(inputCloudsColorMode->sizePolicy().hasHeightForWidth());
        inputCloudsColorMode->setSizePolicy(sizePolicy4);
        inputCloudsColorMode->setMinimumSize(QSize(100, 0));

        gridLayout->addWidget(inputCloudsColorMode, 0, 0, 1, 1);

        spacerItem = new QSpacerItem(143, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 0, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 10, 1, 1, 1);

        label_15 = new QLabel(frame_5);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setFont(font);
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_15, 9, 0, 1, 1);

        inputThetaE_Line = new LineEditorWidget(frame_5);
        inputThetaE_Line->setObjectName(QStringLiteral("inputThetaE_Line"));

        gridLayout_2->addWidget(inputThetaE_Line, 9, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 0, 0, 1, 1);


        verticalLayout->addWidget(frame_5);

        verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        frame = new QFrame(GraphicsParamsDialog);
        frame->setObjectName(QStringLiteral("frame"));
        sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy2);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        hboxLayout = new QHBoxLayout(frame);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setSizeConstraint(QLayout::SetMinimumSize);
        hboxLayout->setContentsMargins(-1, 4, -1, 4);
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        btOK = new QPushButton(frame);
        btOK->setObjectName(QStringLiteral("btOK"));

        hboxLayout->addWidget(btOK);

        btCancel = new QPushButton(frame);
        btCancel->setObjectName(QStringLiteral("btCancel"));

        hboxLayout->addWidget(btCancel);

        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem2);


        verticalLayout->addWidget(frame);

        frame_5->raise();
        frame->raise();

        retranslateUi(GraphicsParamsDialog);

        QMetaObject::connectSlotsByName(GraphicsParamsDialog);
    } // setupUi

    void retranslateUi(QDialog *GraphicsParamsDialog)
    {
        GraphicsParamsDialog->setWindowTitle(QApplication::translate("GraphicsParamsDialog", "Graphical parameters", nullptr));
        label_14->setText(QApplication::translate("GraphicsParamsDialog", "Isotherms (altitude) :", nullptr));
        label_5->setText(QApplication::translate("GraphicsParamsDialog", "Background color :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputBgColor->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputBgColor->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_6->setText(QApplication::translate("GraphicsParamsDialog", "Sea color :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputSeaColor->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputSeaColor->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_7->setText(QApplication::translate("GraphicsParamsDialog", "Land color :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputLandColor->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputLandColor->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_8->setText(QApplication::translate("GraphicsParamsDialog", "Sea border lines :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputSeaBordersLine->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputSeaBordersLine->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_9->setText(QApplication::translate("GraphicsParamsDialog", "Boundaries :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputBoundariesLine->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputBoundariesLine->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_10->setText(QApplication::translate("GraphicsParamsDialog", "Rivers :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputRiversLine->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputRiversLine->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_11->setText(QApplication::translate("GraphicsParamsDialog", "Isobars :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputIsobarsLine->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputIsobarsLine->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_12->setText(QApplication::translate("GraphicsParamsDialog", "Isotherms 0\302\260C :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputIsotherms0Line->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputIsotherms0Line->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        inputIsotherms_Line->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputIsotherms_Line->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_13->setText(QApplication::translate("GraphicsParamsDialog", "Cloud cover :", nullptr));
        label_15->setText(QApplication::translate("GraphicsParamsDialog", "Theta-e :", nullptr));
#ifndef QT_NO_TOOLTIP
        inputThetaE_Line->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputThetaE_Line->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        btOK->setText(QApplication::translate("GraphicsParamsDialog", "Ok", nullptr));
        btCancel->setText(QApplication::translate("GraphicsParamsDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphicsParamsDialog: public Ui_GraphicsParamsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICSPARAMSDIALOG_H
