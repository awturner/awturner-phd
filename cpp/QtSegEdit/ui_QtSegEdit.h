/********************************************************************************
** Form generated from reading ui file 'QtSegEdit.ui'
**
** Created: Fri 1. May 16:27:33 2009
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTSEGEDIT_H
#define UI_QTSEGEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_frmQtSegEdit
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame;
    QMenuBar *menubar;
    QMenu *menu_File;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *frmQtSegEdit)
    {
    if (frmQtSegEdit->objectName().isEmpty())
        frmQtSegEdit->setObjectName(QString::fromUtf8("frmQtSegEdit"));
    frmQtSegEdit->resize(800, 600);
    centralwidget = new QWidget(frmQtSegEdit);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    centralwidget->setGeometry(QRect(0, 21, 800, 560));
    horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    scrollArea = new QScrollArea(centralwidget);
    scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 780, 540));
    horizontalLayout_2 = new QHBoxLayout(scrollAreaWidgetContents);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    frame = new QFrame(scrollAreaWidgetContents);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    horizontalLayout_2->addWidget(frame);

    scrollArea->setWidget(scrollAreaWidgetContents);

    horizontalLayout->addWidget(scrollArea);

    frmQtSegEdit->setCentralWidget(centralwidget);
    menubar = new QMenuBar(frmQtSegEdit);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 800, 21));
    menu_File = new QMenu(menubar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    frmQtSegEdit->setMenuBar(menubar);
    statusbar = new QStatusBar(frmQtSegEdit);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    statusbar->setGeometry(QRect(0, 581, 800, 19));
    frmQtSegEdit->setStatusBar(statusbar);

    menubar->addAction(menu_File->menuAction());

    retranslateUi(frmQtSegEdit);

    QMetaObject::connectSlotsByName(frmQtSegEdit);
    } // setupUi

    void retranslateUi(QMainWindow *frmQtSegEdit)
    {
    frmQtSegEdit->setWindowTitle(QApplication::translate("frmQtSegEdit", "Segmentation Editor", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("frmQtSegEdit", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class frmQtSegEdit: public Ui_frmQtSegEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSEGEDIT_H
