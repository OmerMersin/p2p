//
// Created by Omer Mersin on 11/16/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include "networking/peer.h"
#include "networking/dht_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
//    void onMessageReceived(const QString &message, const QString &senderIP, int senderPort);
    void onSendButtonClicked();

private:
    Ui::MainWindow *ui;
    Peer peer;
    QMutex logMutex;

    QString publicIP;         // Add publicIP to store the user's public IP address
    int publicPort;           // Add publicPort to store the user's public port
    DHTManager dhtManager;    // Add dhtManager for DHT operations

    void appendLog(const QString &message);
    void initializeP2P();
};

#endif // MAINWINDOW_H