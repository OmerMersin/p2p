//
// Created by Omer Mersin on 11/16/24.
//


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networking/stun.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QThread> // Include QThread header

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), peer() {
    ui->setupUi(this);
    peer.setMessageCallback([this](const std::string &message, const std::string &ip, int port) {
        QMetaObject::invokeMethod(this, [this, message, ip, port]() {
            appendLog(QString("Message from %1:%2 - %3")
                              .arg(QString::fromStdString(ip))
                              .arg(port)
                              .arg(QString::fromStdString(message)));
        });
    });
    appendLog("=== Welcome to the P2P Messaging App ===");

    // Initialize STUN, DHT, and P2P networking in a separate thread
    QThread *initThread = QThread::create([this]() { initializeP2P(); });
    connect(initThread, &QThread::finished, initThread, &QObject::deleteLater);
    initThread->start();

    // Connect the send button to the send function
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
}

MainWindow::~MainWindow() {
    peer.stopListening();
    delete ui;
}

void MainWindow::appendLog(const QString &message) {
    QMutexLocker locker(&logMutex);
    ui->logTextBox->append(message);
}

void MainWindow::initializeP2P() {
    QString stunServer = "stun.l.google.com";
    int stunPort = 19302;

    QMetaObject::invokeMethod(this, [this, stunServer, stunPort]() {
        appendLog("Attempting to resolve STUN server: " + stunServer + ":" + QString::number(stunPort));
    });

    auto [ip, port] = STUN::getPublicAddress(stunServer.toStdString(), stunPort);

    if (ip.empty() || port == 0) {
        QMetaObject::invokeMethod(this, [this]() {
            appendLog("Error during STUN resolution: Failed to retrieve public address.");
            appendLog("Failed to resolve STUN server. Peer communication may not work.");
        });
        return;
    }

    publicIP = QString::fromStdString(ip);
    publicPort = port;

    QMetaObject::invokeMethod(this, [this]() {
        appendLog("Your Public IP: " + publicIP);
        appendLog("Your Public Port: " + QString::number(publicPort));
    });

    QString username;
    QMetaObject::invokeMethod(this, [&username]() {
        username = QInputDialog::getText(nullptr, "Enter Username", "Enter your username:");
    }, Qt::BlockingQueuedConnection);

    if (username.isEmpty()) {
        QMetaObject::invokeMethod(this, [this]() {
            appendLog("Error: Username cannot be empty.");
        });
        return;
    }

    dhtManager.startDHT(publicPort);
    dhtManager.announceUsername(username.toStdString(), publicIP.toStdString(), publicPort);

    QMetaObject::invokeMethod(this, [this, username]() {
        appendLog("Username announced: " + username);
    });

    peer.bind(publicPort);
    peer.startListening();

    QMetaObject::invokeMethod(this, [this]() {
        appendLog("Listening for incoming messages...");
    });
}

void MainWindow::onSendButtonClicked() {
    QString peerIP = ui->peerIPInput->text();
    QString peerPortStr = ui->peerPortInput->text();
    QString message = ui->messageInput->text();

    if (peerIP.isEmpty() || peerPortStr.isEmpty() || message.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter valid Peer IP, Peer Port, and Message.");
        return;
    }
    int peerPort = peerPortStr.toInt();

    try {
        peer.sendMessage(message.toStdString(), peerIP.toStdString(), peerPort);
        appendLog("You: " + message);
        ui->messageInput->clear();
    } catch (const std::exception &e) {
        appendLog("Error sending message: " + QString::fromStdString(e.what()));
    }
}

//void MainWindow::onMessageReceived(const QString &message, const QString &senderIP, int senderPort) {
//    QMetaObject::invokeMethod(this, [this, message, senderIP, senderPort]() {
//        appendLog("Message from " + senderIP + ":" + QString::number(senderPort) + " - " + message);
//    });
//}