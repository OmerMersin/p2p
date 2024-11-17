//
// Created by Omer Mersin on 11/16/24.
//


#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "networking/stun.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QHostInfo>
#include <QThread>
#include <sstream> // For std::istringstream
#include <vector>
#include "utils.h"
#include <QRandomGenerator> // Include this at the top of your file



MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), peer() {
    ui->setupUi(this);

    // Log the welcome message
    appendLog("=== Welcome to the P2P Messaging App ===");

    // Set up message callback for handling incoming messages
    peer.setMessageCallback([this](const std::string &message, const std::string &ip, int port) {
        // Log and route the incoming message
        QMetaObject::invokeMethod(this, [this, message, ip, port]() {
            appendLog(QString("Received message from %1:%2 - %3")
                              .arg(QString::fromStdString(ip))
                              .arg(port)
                              .arg(QString::fromStdString(message)));

            // Pass the message to the DHT for processing
            if (dht) {
                dht->handleIncomingMessage(message, ip, port);
            } else {
                appendLog("Error: DHT instance is not initialized.");
            }
        });
    });

    // Start the initialization of P2P networking in a separate thread
    QThread *initThread = QThread::create([this]() {
        try {
            initializeP2P();
        } catch (const std::exception &e) {
            appendLog("Error during P2P initialization: " + QString::fromStdString(e.what()));
        }
    });
    connect(initThread, &QThread::finished, initThread, &QObject::deleteLater);
    initThread->start();

    // Connect the send button click event to the message sending function
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);

    appendLog("Initialization process started...");
}

MainWindow::~MainWindow() {
    peer.stopListening();
    delete dht;
    delete ui;
}

void MainWindow::appendLog(const QString &message) {
    QMutexLocker locker(&logMutex);
    ui->logTextBox->append(message);
}

void MainWindow::initializeP2P() {
    QString bootstrapIP = "127.0.0.1"; // Fixed IP for the bootstrap node
    int bootstrapPort = 12345;        // Fixed port for the bootstrap node

    QString stunServer = "stun.l.google.com";
    int stunPort = 19302;

    // Ask for username
    QString username;
    QMetaObject::invokeMethod(this, [&username]() {
        username = QInputDialog::getText(nullptr, "Enter Username", "Enter your username:");
    }, Qt::BlockingQueuedConnection);

    if (username.isEmpty()) {
        QMetaObject::invokeMethod(this, [this]() {
            appendLog("Error: Username cannot be empty. Exiting initialization.");
        });
        return;
    }

    // Initialize variables
    bool isBootstrap = (username == "bootstrap");
    if (isBootstrap) {
        publicIP = bootstrapIP;
        publicPort = bootstrapPort; // Fixed port for the bootstrap node
    } else {
        // Resolve Public IP and Port using STUN for other nodes
        QMetaObject::invokeMethod(this, [this, stunServer, stunPort]() {
            appendLog("Attempting to resolve STUN server: " + stunServer + ":" + QString::number(stunPort));
        });

        auto [ip, port] = STUN::getPublicAddress(stunServer.toStdString(), stunPort);

        if (ip.empty() || port == 0) {
            QMetaObject::invokeMethod(this, [this]() {
                appendLog("Error during STUN resolution: Failed to retrieve public address.");
                appendLog("Using fallback local IP and dynamic port.");
            });
            publicIP = "127.0.0.1"; // Fallback for local testing
            publicPort = 10000 + QRandomGenerator::global()->bounded(10000); // Random dynamic port
        } else {
            publicIP = QString::fromStdString(ip);
            publicPort = port;
        }
    }

    QMetaObject::invokeMethod(this, [this]() {
        appendLog("Your Public IP: " + publicIP);
        appendLog("Your Public Port: " + QString::number(publicPort));
    });

    // Initialize the DHT
    dht = new DHT(username.toStdString(), publicIP.toStdString(), publicPort);

    // Add self to the DHT
    QString selfID = username;
    dht->addNode({selfID.toStdString(), publicIP.toStdString(), publicPort});

    QMetaObject::invokeMethod(this, [this, selfID]() {
        appendLog("Your Username (Node ID): " + selfID);
    });

    if (!isBootstrap) {
        DHTNode bootstrapNode{"bootstrap", bootstrapIP.toStdString(), bootstrapPort};

        // Send DISCOVER to bootstrap to request its routing table
        try {
            appendLog("Connecting to the bootstrap node...");
            dht->discoverNodes(bootstrapNode.ip, bootstrapNode.port);
            appendLog("Bootstrap node contacted successfully for routing table.");
        } catch (const std::exception &e) {
            appendLog("Bootstrap node discovery failed: " + QString::fromStdString(e.what()));
        }

        // Announce self to the bootstrap node
        try {
            dht->sendMessage("ANNOUNCE " + username.toStdString(), bootstrapNode.ip, bootstrapNode.port);
            appendLog("Announced self to the bootstrap node.");
        } catch (const std::exception &e) {
            appendLog("Failed to announce self to bootstrap: " + QString::fromStdString(e.what()));
        }
    } else {
        appendLog("Running as the bootstrap node.");
    }

    // Start listening for incoming messages
    try {
        peer.bind(publicPort);
        peer.startListening();
        QMetaObject::invokeMethod(this, [this]() {
            appendLog("Listening for incoming messages...");
        });
    } catch (const std::exception &e) {
        QMetaObject::invokeMethod(this, [this, e]() {
            appendLog("Error starting peer listener: " + QString::fromStdString(e.what()));
        });
        return;
    }

    // Log success
    QMetaObject::invokeMethod(this, [this, isBootstrap]() {
        appendLog(isBootstrap ? "Bootstrap node initialized successfully."
                              : "Node initialized and connected to bootstrap node.");
    });
}

void MainWindow::onSendButtonClicked() {
    appendLog("Current DHT Routing Table:");
    for (const auto& node : dht->getRoutingTable()) {
        QString nodeInfo = QString("Node ID: %1, IP: %2, Port: %3")
                .arg(QString::fromStdString(node.id))
                .arg(QString::fromStdString(node.ip))
                .arg(node.port);
        appendLog(nodeInfo);
    }

    QString peerID = ui->peerIDInput->text();
    QString peerIP = ui->peerIPInput->text();
    QString peerPortStr = ui->peerPortInput->text();
    QString message = ui->messageInput->text();

    if (message.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a message.");
        return;
    }

    try {
        if (!peerID.isEmpty()) {
            // Resolve Peer ID using DHT
            DHTNode peerNode = dht->findNode(peerID.toStdString());
            peer.sendMessage(message.toStdString(), peerNode.ip, peerNode.port);
            appendLog("You: " + message + " (via Peer ID)");
        } else if (!peerIP.isEmpty() && !peerPortStr.isEmpty()) {
            // Use Peer IP and Port directly
            int peerPort = peerPortStr.toInt();
            peer.sendMessage(message.toStdString(), peerIP.toStdString(), peerPort);
            appendLog("You: " + message + " (via IP and Port)");
        } else {
            QMessageBox::warning(this, "Invalid Input", "Please provide Peer ID or Peer IP and Port.");
            return;
        }

        ui->messageInput->clear();
    } catch (const std::exception &e) {
        appendLog("Error sending message: " + QString::fromStdString(e.what()));
    }
}
