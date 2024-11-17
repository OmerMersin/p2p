//
// Created by Omer Mersin on 11/16/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QThread>
#include "networking/peer.h"
#include "networking/dht.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendButtonClicked();

private:
    Ui::MainWindow *ui;
    Peer peer;
    DHT *dht;             // Pointer to the DHT instance
    QMutex logMutex;

    QString publicIP;     // To store the public IP of the user
    int publicPort;       // To store the public port of the user

    void appendLog(const QString &message);
    void initializeP2P();
};

#endif // MAINWINDOW_H