/********************************************************************************
** Form generated from reading ui file 'QtSandboxForm.ui'
**
** Created: Thu 29. Jan 11:51:14 2009
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTSANDBOXFORM_H
#define UI_QTSANDBOXFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSandboxForm
{
public:
    QAction *actionLoad;
    QAction *actionSave;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QSplitter *splitter;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *buttonAutoAlign;
    QPushButton *buttonStopAutomatic;
    QPushButton *buttonSave;
    QPushButton *buttonReport;
    QTreeView *worldTreeView;
    QSpacerItem *verticalSpacer_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *QtSandboxForm)
    {
    if (QtSandboxForm->objectName().isEmpty())
        QtSandboxForm->setObjectName(QString::fromUtf8("QtSandboxForm"));
    QtSandboxForm->resize(800, 600);
    actionLoad = new QAction(QtSandboxForm);
    actionLoad->setObjectName(QString::fromUtf8("actionLoad"));
    actionSave = new QAction(QtSandboxForm);
    actionSave->setObjectName(QString::fromUtf8("actionSave"));
    centralwidget = new QWidget(QtSandboxForm);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    centralwidget->setGeometry(QRect(0, 45, 800, 536));
    horizontalLayout_3 = new QHBoxLayout(centralwidget);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    splitter = new QSplitter(centralwidget);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Vertical);
    splitter->setOpaqueResize(true);
    splitter->setChildrenCollapsible(true);

    horizontalLayout_2->addWidget(splitter);

    groupBox = new QGroupBox(centralwidget);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
    groupBox->setSizePolicy(sizePolicy);
    groupBox->setFlat(true);
    horizontalLayout = new QHBoxLayout(groupBox);
    horizontalLayout->setMargin(0);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    buttonAutoAlign = new QPushButton(groupBox);
    buttonAutoAlign->setObjectName(QString::fromUtf8("buttonAutoAlign"));

    verticalLayout->addWidget(buttonAutoAlign);

    buttonStopAutomatic = new QPushButton(groupBox);
    buttonStopAutomatic->setObjectName(QString::fromUtf8("buttonStopAutomatic"));

    verticalLayout->addWidget(buttonStopAutomatic);

    buttonSave = new QPushButton(groupBox);
    buttonSave->setObjectName(QString::fromUtf8("buttonSave"));

    verticalLayout->addWidget(buttonSave);

    buttonReport = new QPushButton(groupBox);
    buttonReport->setObjectName(QString::fromUtf8("buttonReport"));

    verticalLayout->addWidget(buttonReport);

    worldTreeView = new QTreeView(groupBox);
    worldTreeView->setObjectName(QString::fromUtf8("worldTreeView"));
    worldTreeView->setMaximumSize(QSize(200, 16777215));
    worldTreeView->setWordWrap(true);
    worldTreeView->setExpandsOnDoubleClick(false);

    verticalLayout->addWidget(worldTreeView);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer_2);


    horizontalLayout->addLayout(verticalLayout);


    horizontalLayout_2->addWidget(groupBox);


    horizontalLayout_3->addLayout(horizontalLayout_2);

    QtSandboxForm->setCentralWidget(centralwidget);
    menubar = new QMenuBar(QtSandboxForm);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 800, 21));
    menubar->setDefaultUp(false);
    QtSandboxForm->setMenuBar(menubar);
    statusbar = new QStatusBar(QtSandboxForm);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    statusbar->setGeometry(QRect(0, 581, 800, 19));
    QtSandboxForm->setStatusBar(statusbar);
    toolBar = new QToolBar(QtSandboxForm);
    toolBar->setObjectName(QString::fromUtf8("toolBar"));
    toolBar->setGeometry(QRect(0, 21, 800, 24));
    QtSandboxForm->addToolBar(Qt::TopToolBarArea, toolBar);

    toolBar->addAction(actionLoad);
    toolBar->addAction(actionSave);

    retranslateUi(QtSandboxForm);

    QMetaObject::connectSlotsByName(QtSandboxForm);
    } // setupUi

    void retranslateUi(QMainWindow *QtSandboxForm)
    {
    QtSandboxForm->setWindowTitle(QApplication::translate("QtSandboxForm", "MainWindow", 0, QApplication::UnicodeUTF8));
    actionLoad->setText(QApplication::translate("QtSandboxForm", "Load", 0, QApplication::UnicodeUTF8));
    actionSave->setText(QApplication::translate("QtSandboxForm", "Save", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("QtSandboxForm", "Blah blah", 0, QApplication::UnicodeUTF8));
    buttonAutoAlign->setText(QApplication::translate("QtSandboxForm", "Automatic Alignment", 0, QApplication::UnicodeUTF8));
    buttonStopAutomatic->setText(QApplication::translate("QtSandboxForm", "Stop", 0, QApplication::UnicodeUTF8));
    buttonSave->setText(QApplication::translate("QtSandboxForm", "Save", 0, QApplication::UnicodeUTF8));
    buttonReport->setText(QApplication::translate("QtSandboxForm", "Report", 0, QApplication::UnicodeUTF8));
    toolBar->setWindowTitle(QApplication::translate("QtSandboxForm", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtSandboxForm: public Ui_QtSandboxForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSANDBOXFORM_H
