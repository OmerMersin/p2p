/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *logTextBox;
    QHBoxLayout *horizontalLayout;
    QLineEdit *peerIDInput;
    QLineEdit *peerIPInput;
    QLineEdit *peerPortInput;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *messageInput;
    QPushButton *sendButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(885, 638);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        logTextBox = new QTextEdit(centralwidget);
        logTextBox->setObjectName("logTextBox");

        verticalLayout->addWidget(logTextBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        peerIDInput = new QLineEdit(centralwidget);
        peerIDInput->setObjectName("peerIDInput");

        horizontalLayout->addWidget(peerIDInput);

        peerIPInput = new QLineEdit(centralwidget);
        peerIPInput->setObjectName("peerIPInput");

        horizontalLayout->addWidget(peerIPInput);

        peerPortInput = new QLineEdit(centralwidget);
        peerPortInput->setObjectName("peerPortInput");

        horizontalLayout->addWidget(peerPortInput);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        messageInput = new QLineEdit(centralwidget);
        messageInput->setObjectName("messageInput");

        horizontalLayout_2->addWidget(messageInput);

        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");

        horizontalLayout_2->addWidget(sendButton);


        verticalLayout->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "P2P Messaging", nullptr));
        peerIDInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Peer ID", nullptr));
        peerIPInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Peer IP", nullptr));
        peerPortInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Peer Port", nullptr));
        messageInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Type your message here...", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
