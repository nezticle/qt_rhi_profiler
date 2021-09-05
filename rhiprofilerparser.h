#ifndef RHIPROFILERPARSER_H
#define RHIPROFILERPARSER_H

#include <QObject>
#include <QtGui/private/qrhiprofiler_p.h>

class RhiProfilerParser : public QObject
{
    Q_OBJECT
public:
    explicit RhiProfilerParser(QObject *parent = nullptr);
    void feed(const QByteArray &line);

    struct Event {
        QRhiProfiler::StreamOp op;
        qint64 timestamp;
        quint64 resource;
        QByteArray resourceName;

        struct Param {
            enum ValueType {
                Int64,
                Float
            };
            QByteArray key;
            ValueType valueType;
            union {
                qint64 intValue;
                float floatValue;
            };
        };

        QList<Param> params;

        const Param *param(const char *key) const;
    };

signals:
    void eventReceived(const Event &e);

private:


};

#endif // RHIPROFILERPARSER_H
