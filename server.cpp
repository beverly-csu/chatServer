#include "server.hpp"

Server::Server(int port){
    _server = new QTcpServer();
    if(!_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server can't start. Try one more time :)";
        _server->close();
        return;
    }
    else {
        qDebug() << "Server started.";
    }
    connect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection_slot()));
}


void Server::onNewConnection_slot() {
    QTcpSocket* clientSocket = _server->nextPendingConnection();

    qint64 descriptor = clientSocket->socketDescriptor();
    _clientsLogin[descriptor] = "QTUSERNAMEWAIT";
    _clientsSocket[descriptor] = clientSocket;

    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readyReadClient_slot()));

    qDebug() << "Desc: " << descriptor << " Login: " << _clientsLogin[descriptor] << " Socket: " << _clientsSocket[descriptor];

    sendMsgToClient(clientSocket, "You are online!");
}


void Server::sendMsgToClient(QTcpSocket *socket, const QString &msg) {
    socket->write(msg.toUtf8());
}

void Server::sendMsgToAll(int descriptor, const QString &msg)
{
    QMap<int, QTcpSocket*>::iterator it = _clientsSocket.begin();
    QString msg2All = "[" + _clientsLogin[descriptor] + "] " + msg;
    while(it != _clientsSocket.end()) {
        if (it.key() != descriptor)
            sendMsgToClient(it.value(), msg2All);
        it++;
    }
}


void Server::readyReadClient_slot() {
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QByteArray data = clientSocket->readAll();

    QString str(data);
    if (_clientsLogin[clientSocket->socketDescriptor()] == "QTUSERNAMEWAIT") {
        _clientsLogin[clientSocket->socketDescriptor()] = str;
    }
    qDebug() << "[Client" << clientSocket->socketDescriptor() << "\b]" << str;
    QString answer = "[Server Response]" + str + "\n";
    //sendMsgToClient(clientSocket, answer);
    sendMsgToAll(clientSocket->socketDescriptor(), str);
}

