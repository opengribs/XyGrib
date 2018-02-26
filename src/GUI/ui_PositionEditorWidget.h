/********************************************************************************
** Form generated from reading UI file 'PositionEditorWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POSITIONEDITORWIDGET_H
#define UI_POSITIONEDITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PositionEditorWidget
{
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLabel *label_lat;
    QComboBox *lat_sign;
    QSpinBox *lat_degrees;
    QDoubleSpinBox *lat_minutes;
    QComboBox *lat_NS;
    QLabel *label_lon;
    QComboBox *lon_sign;
    QSpinBox *lon_degrees;
    QDoubleSpinBox *lon_minutes;
    QComboBox *lon_EW;

    void setupUi(QWidget *PositionEditorWidget)
    {
        if (PositionEditorWidget->objectName().isEmpty())
            PositionEditorWidget->setObjectName(QStringLiteral("PositionEditorWidget"));
        PositionEditorWidget->resize(459, 72);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PositionEditorWidget->sizePolicy().hasHeightForWidth());
        PositionEditorWidget->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(PositionEditorWidget);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_lat = new QLabel(PositionEditorWidget);
        label_lat->setObjectName(QStringLiteral("label_lat"));
        label_lat->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_lat, 0, 0, 1, 1);

        lat_sign = new QComboBox(PositionEditorWidget);
        lat_sign->setObjectName(QStringLiteral("lat_sign"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lat_sign->sizePolicy().hasHeightForWidth());
        lat_sign->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lat_sign, 0, 1, 1, 1);

        lat_degrees = new QSpinBox(PositionEditorWidget);
        lat_degrees->setObjectName(QStringLiteral("lat_degrees"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lat_degrees->sizePolicy().hasHeightForWidth());
        lat_degrees->setSizePolicy(sizePolicy2);
        lat_degrees->setMaximum(89);

        gridLayout->addWidget(lat_degrees, 0, 2, 1, 1);

        lat_minutes = new QDoubleSpinBox(PositionEditorWidget);
        lat_minutes->setObjectName(QStringLiteral("lat_minutes"));
        sizePolicy2.setHeightForWidth(lat_minutes->sizePolicy().hasHeightForWidth());
        lat_minutes->setSizePolicy(sizePolicy2);
        lat_minutes->setMaximum(59.99);

        gridLayout->addWidget(lat_minutes, 0, 3, 1, 1);

        lat_NS = new QComboBox(PositionEditorWidget);
        lat_NS->setObjectName(QStringLiteral("lat_NS"));
        sizePolicy1.setHeightForWidth(lat_NS->sizePolicy().hasHeightForWidth());
        lat_NS->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lat_NS, 0, 4, 1, 1);

        label_lon = new QLabel(PositionEditorWidget);
        label_lon->setObjectName(QStringLiteral("label_lon"));
        label_lon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_lon, 1, 0, 1, 1);

        lon_sign = new QComboBox(PositionEditorWidget);
        lon_sign->setObjectName(QStringLiteral("lon_sign"));
        sizePolicy1.setHeightForWidth(lon_sign->sizePolicy().hasHeightForWidth());
        lon_sign->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lon_sign, 1, 1, 1, 1);

        lon_degrees = new QSpinBox(PositionEditorWidget);
        lon_degrees->setObjectName(QStringLiteral("lon_degrees"));
        sizePolicy2.setHeightForWidth(lon_degrees->sizePolicy().hasHeightForWidth());
        lon_degrees->setSizePolicy(sizePolicy2);
        lon_degrees->setMaximum(359);

        gridLayout->addWidget(lon_degrees, 1, 2, 1, 1);

        lon_minutes = new QDoubleSpinBox(PositionEditorWidget);
        lon_minutes->setObjectName(QStringLiteral("lon_minutes"));
        sizePolicy2.setHeightForWidth(lon_minutes->sizePolicy().hasHeightForWidth());
        lon_minutes->setSizePolicy(sizePolicy2);
        lon_minutes->setMaximum(59.99);

        gridLayout->addWidget(lon_minutes, 1, 3, 1, 1);

        lon_EW = new QComboBox(PositionEditorWidget);
        lon_EW->setObjectName(QStringLiteral("lon_EW"));
        sizePolicy1.setHeightForWidth(lon_EW->sizePolicy().hasHeightForWidth());
        lon_EW->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lon_EW, 1, 4, 1, 1);


        vboxLayout->addLayout(gridLayout);


        retranslateUi(PositionEditorWidget);

        QMetaObject::connectSlotsByName(PositionEditorWidget);
    } // setupUi

    void retranslateUi(QWidget *PositionEditorWidget)
    {
        PositionEditorWidget->setWindowTitle(QApplication::translate("PositionEditorWidget", "Form", nullptr));
        label_lat->setText(QApplication::translate("PositionEditorWidget", "Latitude", nullptr));
        lat_degrees->setSuffix(QApplication::translate("PositionEditorWidget", " \302\260", nullptr));
        lat_minutes->setSuffix(QApplication::translate("PositionEditorWidget", " '", nullptr));
        label_lon->setText(QApplication::translate("PositionEditorWidget", "Longitude", nullptr));
        lon_degrees->setSuffix(QApplication::translate("PositionEditorWidget", " \302\260", nullptr));
        lon_minutes->setSuffix(QApplication::translate("PositionEditorWidget", " '", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PositionEditorWidget: public Ui_PositionEditorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POSITIONEDITORWIDGET_H
