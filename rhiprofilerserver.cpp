#include "rhiprofilerserver.h"

void RhiProfilerServer::incomingConnection(qintptr socketDescriptor)
{
    if (m_valid)
        return;

    m_socket.setSocketDescriptor(socketDescriptor);
    m_valid = true;
    emit clientConnected();
    connect(&m_socket, &QAbstractSocket::readyRead, this, [this] {
        bool receiveStartedSent = false;
        m_buf += m_socket.readAll();
        while (m_buf.contains('\n')) {
            const int lfpos = m_buf.indexOf('\n');
            const QByteArray line = m_buf.left(lfpos).trimmed();
            m_buf = m_buf.mid(lfpos + 1);
            if (!receiveStartedSent) {
                receiveStartedSent = true;
                emit receiveStarted();
            }
            emit lineReceived(line);
        }
    });
    connect(&m_socket, &QAbstractSocket::disconnected, this, [this] {
        if (m_valid) {
            m_valid = false;
            emit clientDisconnected();
        }
    });
}
