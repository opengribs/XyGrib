/********************************************************************************
** Form generated from reading UI file 'MeteotableOptionsDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METEOTABLEOPTIONSDIALOG_H
#define UI_METEOTABLEOPTIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MeteotableOptionsDialog
{
public:
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QComboBox *inputCloudsColorMode;
    QSpacerItem *spacerItem;
    QLabel *label_5;
    QCheckBox *cbShowBeauforts;
    QSpacerItem *spacer;
    QSpacerItem *spacerItem1;
    QLabel *label_4;
    QCheckBox *cbShowWindArrows;
    QLabel *label_6;
    QCheckBox *cbSunMoonAlmanac;
    QFrame *line;
    QSpacerItem *spacerItem2;
    QPushButton *btOK;
    QPushButton *btCancel;
    QSpacerItem *spacerItem3;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QVBoxLayout *verticalLayout;
    QSpacerItem *spacerItem4;
    QPushButton *btAddVisibleItem;
    QPushButton *btRemoveVisibleItem;
    QSpacerItem *spacerItem5;
    QLabel *label_3;
    QListWidget *listHiddenData;
    QListWidget *listVisibleData;

    void setupUi(QDialog *MeteotableOptionsDialog)
    {
        if (MeteotableOptionsDialog->objectName().isEmpty())
            MeteotableOptionsDialog->setObjectName(QStringLiteral("MeteotableOptionsDialog"));
        MeteotableOptionsDialog->resize(719, 594);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MeteotableOptionsDialog->sizePolicy().hasHeightForWidth());
        MeteotableOptionsDialog->setSizePolicy(sizePolicy);
        gridLayout_3 = new QGridLayout(MeteotableOptionsDialog);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(2);
        label = new QLabel(MeteotableOptionsDialog);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        inputCloudsColorMode = new QComboBox(MeteotableOptionsDialog);
        inputCloudsColorMode->setObjectName(QStringLiteral("inputCloudsColorMode"));

        gridLayout_2->addWidget(inputCloudsColorMode, 0, 1, 1, 2);

        spacerItem = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(spacerItem, 0, 5, 1, 1);

        label_5 = new QLabel(MeteotableOptionsDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setFont(font);

        gridLayout_2->addWidget(label_5, 1, 0, 1, 2);

        cbShowBeauforts = new QCheckBox(MeteotableOptionsDialog);
        cbShowBeauforts->setObjectName(QStringLiteral("cbShowBeauforts"));

        gridLayout_2->addWidget(cbShowBeauforts, 1, 2, 1, 1);

        spacer = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(spacer, 1, 5, 1, 1);

        spacerItem1 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(spacerItem1, 2, 5, 1, 1);

        label_4 = new QLabel(MeteotableOptionsDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font);

        gridLayout_2->addWidget(label_4, 1, 3, 1, 1);

        cbShowWindArrows = new QCheckBox(MeteotableOptionsDialog);
        cbShowWindArrows->setObjectName(QStringLiteral("cbShowWindArrows"));

        gridLayout_2->addWidget(cbShowWindArrows, 1, 4, 1, 1);

        label_6 = new QLabel(MeteotableOptionsDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_6, 2, 0, 1, 2);

        cbSunMoonAlmanac = new QCheckBox(MeteotableOptionsDialog);
        cbSunMoonAlmanac->setObjectName(QStringLiteral("cbSunMoonAlmanac"));

        gridLayout_2->addWidget(cbSunMoonAlmanac, 2, 2, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 1, 0, 1, 4);

        line = new QFrame(MeteotableOptionsDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(line, 2, 0, 1, 4);

        spacerItem2 = new QSpacerItem(254, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(spacerItem2, 3, 0, 1, 1);

        btOK = new QPushButton(MeteotableOptionsDialog);
        btOK->setObjectName(QStringLiteral("btOK"));

        gridLayout_3->addWidget(btOK, 3, 1, 1, 1);

        btCancel = new QPushButton(MeteotableOptionsDialog);
        btCancel->setObjectName(QStringLiteral("btCancel"));

        gridLayout_3->addWidget(btCancel, 3, 2, 1, 1);

        spacerItem3 = new QSpacerItem(253, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(spacerItem3, 3, 3, 1, 1);

        frame = new QFrame(MeteotableOptionsDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setAutoFillBackground(false);
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        spacerItem4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(spacerItem4);

        btAddVisibleItem = new QPushButton(frame);
        btAddVisibleItem->setObjectName(QStringLiteral("btAddVisibleItem"));

        verticalLayout->addWidget(btAddVisibleItem);

        btRemoveVisibleItem = new QPushButton(frame);
        btRemoveVisibleItem->setObjectName(QStringLiteral("btRemoveVisibleItem"));

        verticalLayout->addWidget(btRemoveVisibleItem);

        spacerItem5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(spacerItem5);


        gridLayout->addLayout(verticalLayout, 0, 1, 2, 1);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        listHiddenData = new QListWidget(frame);
        listHiddenData->setObjectName(QStringLiteral("listHiddenData"));
        listHiddenData->setDragEnabled(true);
        listHiddenData->setDragDropOverwriteMode(false);
        listHiddenData->setDragDropMode(QAbstractItemView::InternalMove);
        listHiddenData->setSelectionMode(QAbstractItemView::ExtendedSelection);

        gridLayout->addWidget(listHiddenData, 1, 0, 1, 1);

        listVisibleData = new QListWidget(frame);
        listVisibleData->setObjectName(QStringLiteral("listVisibleData"));
        listVisibleData->setDragEnabled(true);
        listVisibleData->setDragDropOverwriteMode(false);
        listVisibleData->setDragDropMode(QAbstractItemView::InternalMove);
        listVisibleData->setSelectionMode(QAbstractItemView::ExtendedSelection);

        gridLayout->addWidget(listVisibleData, 1, 2, 1, 1);


        gridLayout_3->addWidget(frame, 0, 0, 1, 4);


        retranslateUi(MeteotableOptionsDialog);

        QMetaObject::connectSlotsByName(MeteotableOptionsDialog);
    } // setupUi

    void retranslateUi(QDialog *MeteotableOptionsDialog)
    {
        MeteotableOptionsDialog->setWindowTitle(QApplication::translate("MeteotableOptionsDialog", "Meteotable parameters", nullptr));
        label->setText(QApplication::translate("MeteotableOptionsDialog", "Cloud cover:", nullptr));
        label_5->setText(QApplication::translate("MeteotableOptionsDialog", "Show wind in Beauforts:", nullptr));
        cbShowBeauforts->setText(QString());
        label_4->setText(QApplication::translate("MeteotableOptionsDialog", "Show wind arrows:", nullptr));
        cbShowWindArrows->setText(QString());
        label_6->setText(QApplication::translate("MeteotableOptionsDialog", "Sun and Moon almanac:", nullptr));
        cbSunMoonAlmanac->setText(QString());
        btOK->setText(QApplication::translate("MeteotableOptionsDialog", "Ok", nullptr));
        btCancel->setText(QApplication::translate("MeteotableOptionsDialog", "Cancel", nullptr));
        label_2->setText(QApplication::translate("MeteotableOptionsDialog", "Avalaible data", nullptr));
        btAddVisibleItem->setText(QApplication::translate("MeteotableOptionsDialog", "->", nullptr));
        btRemoveVisibleItem->setText(QApplication::translate("MeteotableOptionsDialog", "<-", nullptr));
        label_3->setText(QApplication::translate("MeteotableOptionsDialog", "Visible data", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MeteotableOptionsDialog: public Ui_MeteotableOptionsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METEOTABLEOPTIONSDIALOG_H
