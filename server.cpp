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
    _clientsLogin[descriptor] = "gay";
    _clientsSocket[descriptor] = clientSocket;

    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readyReadClient_slot()));

    qDebug() << "Desc: " << descriptor << " Login: " << _clientsLogin[descriptor] << " Socket: " << _clientsSocket[descriptor];

    sendMsgToClient(clientSocket, "You a gay!\n");
}


void Server::sendMsgToClient(QTcpSocket *socket, const QString &msg) {
    QByteArray byteArray;
    QDataStream outcomingStream(&byteArray, QIODevice::WriteOnly);
    outcomingStream.setVersion(QDataStream::Qt_6_0);
    outcomingStream << "[Server to client] " << msg;
    socket->write(byteArray);
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

    QString str = clientSocket->readAll();
    qDebug() << "[Client" << clientSocket->socketDescriptor() << "]" << str;
    QString answer = "[Server Response] Received " + str + "\n";
    sendMsgToClient(clientSocket, answer);
    sendMsgToAll(clientSocket->socketDescriptor(), str);
}

