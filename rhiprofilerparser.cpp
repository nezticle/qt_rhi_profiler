#include "rhiprofilerparser.h"
#include "rhiprofilerglobals.h"

RhiProfilerParser::RhiProfilerParser(QObject *parent) : QObject(parent)
{

}

void RhiProfilerParser::feed(const QByteArray &line)
{
    const QList<QByteArray> elems = line.split(',');
    if (elems.count() < 4) {
        qWarning("Malformed line '%s'", line.constData());
        return;
    }
    bool ok = false;
    const int op = elems[0].toInt(&ok);
    if (!ok) {
        qWarning("Invalid op %s", elems[0].constData());
        return;
    }
    if (op < MIN_KNOWN_OP || op > MAX_KNOWN_OP) {
        qWarning("Unknown op %d", op);
        return;
    }

    Event e;
    e.op = QRhiProfiler::StreamOp(op);
    e.timestamp = elems[1].toLongLong();
    e.resource = elems[2].toULongLong();
    e.resourceName = elems[3];

    const int elemCount = elems.count();
    for (int i = 4; i < elemCount; i += 2) {
        if (i + 1 < elemCount && !elems[i].isEmpty() && !elems[i + 1].isEmpty()) {
            QByteArray key = elems[i];
            if (key.startsWith('F')) {
                key = key.mid(1);
                bool ok = false;
                const float value = elems[i + 1].toFloat(&ok);
                if (!ok) {
                    qWarning("Failed to parse float %s in line '%s'", elems[i + 1].constData(), line.constData());
                    continue;
                }
                Event::Param param;
                param.key = key;
                param.valueType = Event::Param::Float;
                param.floatValue = value;
                e.params.append(param);
            } else {
                const qint64 value = elems[i + 1].toLongLong();
                Event::Param param;
                param.key = key;
                param.valueType = Event::Param::Int64;
                param.intValue = value;
                e.params.append(param);
            }
        }
    }

    emit eventReceived(e);
}

const RhiProfilerParser::Event::Param *RhiProfilerParser::Event::param(const char *key) const {
    auto it = std::find_if(params.cbegin(), params.cend(), [key](const Param &p) {
        return !strcmp(p.key.constData(), key);
    });
    return it == params.cend() ? nullptr : &*it;
}
