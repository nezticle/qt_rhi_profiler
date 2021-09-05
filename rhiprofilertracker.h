#ifndef RHIPROFILERTRACKER_H
#define RHIPROFILERTRACKER_H

#include <QObject>
#include "rhiprofilerparser.h"

class RhiProfilerTracker : public QObject
{
    Q_OBJECT
public:
    explicit RhiProfilerTracker(QObject *parent = nullptr);

    static const int MAX_STAGING_SLOTS = 3;

    struct Buffer {
        Buffer()
        {
            memset(stagingExtraSize, 0, sizeof(stagingExtraSize));
        }
        quint64 lastTimestamp;
        QByteArray resourceName;
        qint64 effectiveSize = 0;
        int backingGpuBufCount = 1;
        qint64 stagingExtraSize[MAX_STAGING_SLOTS];
    };
    QHash<qint64, Buffer> m_buffers;
    qint64 m_totalBufferApproxByteSize;
    qint64 m_peakBufferApproxByteSize;
    qint64 m_totalStagingBufferApproxByteSize;
    qint64 m_peakStagingBufferApproxByteSize;

    struct Texture {
        Texture()
        {
            memset(stagingExtraSize, 0, sizeof(stagingExtraSize));
        }
        quint64 lastTimestamp;
        QByteArray resourceName;
        qint64 approxByteSize = 0;
        bool ownsNativeResource = true;
        qint64 stagingExtraSize[MAX_STAGING_SLOTS];
    };
    QHash<qint64, Texture> m_textures;
    qint64 m_totalTextureApproxByteSize;
    qint64 m_peakTextureApproxByteSize;
    qint64 m_totalTextureStagingBufferApproxByteSize;
    qint64 m_peakTextureStagingBufferApproxByteSize;

    struct SwapChain {
        quint64 lastTimestamp;
        QByteArray resourceName;
        qint64 approxByteSize = 0;
    };
    QHash<qint64, SwapChain> m_swapchains;
    qint64 m_totalSwapChainApproxByteSize;
    qint64 m_peakSwapChainApproxByteSize;

    struct FrameTime {
        qint64 framesSinceResize = 0;
        int minDelta = 0;
        int maxDelta = 0;
        float avgDelta = 0;
    };
    FrameTime m_lastFrameTime;

    struct GpuFrameTime {
        float minTime = 0;
        float maxTime = 0;
        float avgTime = 0;
    };
    GpuFrameTime m_lastGpuFrameTime;

    struct GpuMemAllocStats {
        qint64 realAllocCount;
        qint64 subAllocCount;
        qint64 totalSize;
        qint64 unusedSize;
    };
    GpuMemAllocStats m_lastGpuMemAllocStats;

    void reset();

public slots:
    void handleEvent(const RhiProfilerParser::Event &e);

signals:
    void buffersTouched();
    void texturesTouched();
    void swapchainsTouched();
    void frameTimeTouched();
    void gpuFrameTimeTouched();
    void gpuMemAllocStatsTouched();

};

Q_DECLARE_TYPEINFO(RhiProfilerTracker::Buffer, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(RhiProfilerTracker::Texture, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(RhiProfilerTracker::FrameTime, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(RhiProfilerTracker::GpuFrameTime, Q_RELOCATABLE_TYPE);

#endif // RHIPROFILERTRACKER_H
