#ifndef RHIPROFILERSERVER_H
#define RHIPROFILERSERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class RhiProfilerServer : public QTcpServer
{
    Q_OBJECT

protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void clientConnected();
    void clientDisconnected();
    void receiveStarted();
    void lineReceived(const QByteArray &line);

private:
    bool m_valid = false;
    QTcpSocket m_socket;
    QByteArray m_buf;
};

#endif // RHIPROFILERSERVER_H
