/********************************************************************************
** Form generated from reading UI file 'PoiEditorDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POIEDITORDIALOG_H
#define UI_POIEDITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "ColorEditorWidget.h"
#include "PositionEditorWidget.h"
#include "TextStyleEditorWidget.h"

QT_BEGIN_NAMESPACE

class Ui_PoiEditorDialog
{
public:
    QVBoxLayout *vboxLayout;
    QFrame *frame;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QSpacerItem *spacerItem;
    QLineEdit *inputName;
    QFrame *frame_2;
    QVBoxLayout *vboxLayout1;
    QLabel *label_2;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    PositionEditorWidget *inputPosition;
    QFrame *frame_3;
    QHBoxLayout *hboxLayout2;
    QLabel *label_3;
    QSpacerItem *spacerItem2;
    ColorEditorWidget *inputMarkColor;
    QFrame *frame_4;
    QHBoxLayout *hboxLayout3;
    QLabel *label_4;
    QSpacerItem *spacerItem3;
    TextStyleEditorWidget *inputStyle;
    QFrame *line;
    QCheckBox *cbIsMovable;
    QCheckBox *cbShowLabel;
    QFrame *line_2;
    QHBoxLayout *hboxLayout4;
    QPushButton *btValid;
    QPushButton *btCancel;
    QPushButton *btDelete;

    void setupUi(QDialog *PoiEditorDialog)
    {
        if (PoiEditorDialog->objectName().isEmpty())
            PoiEditorDialog->setObjectName(QStringLiteral("PoiEditorDialog"));
        PoiEditorDialog->resize(342, 361);
        vboxLayout = new QVBoxLayout(PoiEditorDialog);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        frame = new QFrame(PoiEditorDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setAutoFillBackground(false);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        hboxLayout = new QHBoxLayout(frame);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        hboxLayout->addWidget(label);

        spacerItem = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        inputName = new QLineEdit(frame);
        inputName->setObjectName(QStringLiteral("inputName"));

        hboxLayout->addWidget(inputName);


        vboxLayout->addWidget(frame);

        frame_2 = new QFrame(PoiEditorDialog);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setAutoFillBackground(false);
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Raised);
        vboxLayout1 = new QVBoxLayout(frame_2);
        vboxLayout1->setSpacing(0);
        vboxLayout1->setObjectName(QStringLiteral("vboxLayout1"));
        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        vboxLayout1->addWidget(label_2);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        spacerItem1 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        inputPosition = new PositionEditorWidget(frame_2);
        inputPosition->setObjectName(QStringLiteral("inputPosition"));

        hboxLayout1->addWidget(inputPosition);


        vboxLayout1->addLayout(hboxLayout1);


        vboxLayout->addWidget(frame_2);

        frame_3 = new QFrame(PoiEditorDialog);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setAutoFillBackground(false);
        frame_3->setFrameShape(QFrame::Box);
        frame_3->setFrameShadow(QFrame::Raised);
        hboxLayout2 = new QHBoxLayout(frame_3);
        hboxLayout2->setObjectName(QStringLiteral("hboxLayout2"));
        label_3 = new QLabel(frame_3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        hboxLayout2->addWidget(label_3);

        spacerItem2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem2);

        inputMarkColor = new ColorEditorWidget(frame_3);
        inputMarkColor->setObjectName(QStringLiteral("inputMarkColor"));

        hboxLayout2->addWidget(inputMarkColor);


        vboxLayout->addWidget(frame_3);

        frame_4 = new QFrame(PoiEditorDialog);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setAutoFillBackground(false);
        frame_4->setFrameShape(QFrame::Box);
        frame_4->setFrameShadow(QFrame::Raised);
        hboxLayout3 = new QHBoxLayout(frame_4);
        hboxLayout3->setObjectName(QStringLiteral("hboxLayout3"));
        label_4 = new QLabel(frame_4);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);
        label_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        hboxLayout3->addWidget(label_4);

        spacerItem3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout3->addItem(spacerItem3);

        inputStyle = new TextStyleEditorWidget(frame_4);
        inputStyle->setObjectName(QStringLiteral("inputStyle"));

        hboxLayout3->addWidget(inputStyle);


        vboxLayout->addWidget(frame_4);

        line = new QFrame(PoiEditorDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(line);

        cbIsMovable = new QCheckBox(PoiEditorDialog);
        cbIsMovable->setObjectName(QStringLiteral("cbIsMovable"));

        vboxLayout->addWidget(cbIsMovable);

        cbShowLabel = new QCheckBox(PoiEditorDialog);
        cbShowLabel->setObjectName(QStringLiteral("cbShowLabel"));

        vboxLayout->addWidget(cbShowLabel);

        line_2 = new QFrame(PoiEditorDialog);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(line_2);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setObjectName(QStringLiteral("hboxLayout4"));
        btValid = new QPushButton(PoiEditorDialog);
        btValid->setObjectName(QStringLiteral("btValid"));

        hboxLayout4->addWidget(btValid);

        btCancel = new QPushButton(PoiEditorDialog);
        btCancel->setObjectName(QStringLiteral("btCancel"));

        hboxLayout4->addWidget(btCancel);

        btDelete = new QPushButton(PoiEditorDialog);
        btDelete->setObjectName(QStringLiteral("btDelete"));

        hboxLayout4->addWidget(btDelete);


        vboxLayout->addLayout(hboxLayout4);


        retranslateUi(PoiEditorDialog);

        QMetaObject::connectSlotsByName(PoiEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *PoiEditorDialog)
    {
        PoiEditorDialog->setWindowTitle(QApplication::translate("PoiEditorDialog", "Point of interest", nullptr));
        label->setText(QApplication::translate("PoiEditorDialog", "Name", nullptr));
        label_2->setText(QApplication::translate("PoiEditorDialog", "Location", nullptr));
#ifndef QT_NO_TOOLTIP
        inputPosition->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputPosition->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_3->setText(QApplication::translate("PoiEditorDialog", "Mark color", nullptr));
#ifndef QT_NO_TOOLTIP
        inputMarkColor->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputMarkColor->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        label_4->setText(QApplication::translate("PoiEditorDialog", "Style", nullptr));
#ifndef QT_NO_TOOLTIP
        inputStyle->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        inputStyle->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        cbIsMovable->setText(QApplication::translate("PoiEditorDialog", "Can be moved with mouse (Ctrl+Left click)", nullptr));
        cbShowLabel->setText(QApplication::translate("PoiEditorDialog", "Display the name", nullptr));
        btValid->setText(QApplication::translate("PoiEditorDialog", "Ok", nullptr));
        btCancel->setText(QApplication::translate("PoiEditorDialog", "Cancel", nullptr));
        btDelete->setText(QApplication::translate("PoiEditorDialog", "Delete this POI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PoiEditorDialog: public Ui_PoiEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POIEDITORDIALOG_H
