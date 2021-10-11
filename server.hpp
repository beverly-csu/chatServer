#ifndef SERVER_H
#define SERVER_H

#include <QCoreApplication>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class Server;

class Server : public QObject{
    Q_OBJECT
private:
    QTcpServer* _server;
    QMap<int, QTcpSocket*> _clientsSocket;
    QMap<int, QString> _clientsLogin;

public:
    Server(int);
    void sendMsgToClient(QTcpSocket* socket, const QString& msg);
    void sendMsgToAll(int descriptor, const QString& msg);


public slots:
    virtual void onNewConnection_slot();
    void readyReadClient_slot();
};

#endif // SERVER_H
