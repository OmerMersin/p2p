//
// Created by Omer Mersin on 11/16/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QMutex>
#include "networking/peer.h"
#include "networking/dht_manager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Peer peer;
    DHTManager dhtManager;

    QString publicIP;
    int publicPort;
    QMutex logMutex;

    void appendLog(const QString &message);
    void initializeP2P();

private slots:
    void onSendButtonClicked();
};

#endif // MAINWINDOW_H