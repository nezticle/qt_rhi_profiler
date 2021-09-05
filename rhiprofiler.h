#ifndef RHIPROFILER_H
#define RHIPROFILER_H

#include <QObject>
#include <QtQml/QQmlEngine>

#include "rhiprofilertracker.h"
#include "rhiprofilerparser.h"
#include "rhiprofilerserver.h"

class RhiProfiler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    // Buffers
    Q_PROPERTY(int bufferCount READ bufferCount NOTIFY bufferCountChanged)
    Q_PROPERTY(int bufferTotalSize READ bufferTotalSize NOTIFY bufferTotalSizeChanged)
    Q_PROPERTY(int bufferPeakSize READ bufferPeakSize NOTIFY bufferPeakSizeChanged)
    Q_PROPERTY(int bufferStagingTotalSize READ bufferStagingTotalSize NOTIFY bufferStagingTotalSizeChanged)
    Q_PROPERTY(int bufferStagingPeakSize READ bufferStagingPeakSize NOTIFY bufferStagingPeakSizeChanged)

    // Textures
    Q_PROPERTY(int textureCount READ textureCount NOTIFY textureCountChanged)
    Q_PROPERTY(int textureTotalSize READ textureTotalSize NOTIFY textureTotalSizeChanged)
    Q_PROPERTY(int texturePeakSize READ texturePeakSize NOTIFY texturePeakSizeChanged)
    Q_PROPERTY(int textureStagingTotalSize READ textureStagingTotalSize NOTIFY textureStagingTotalSizeChanged)
    Q_PROPERTY(int textureStagingPeakSize READ textureStagingPeakSize NOTIFY textureStagingPeakSizeChanged)

    // SwapChains
    Q_PROPERTY(int swapchainCount READ swapchainCount NOTIFY swapchainCountChanged)
    Q_PROPERTY(int swapchainTotalSize READ swapchainTotalSize NOTIFY swapchainTotalSizeChanged)
    Q_PROPERTY(int swapchainPeakSize READ swapchainPeakSize NOTIFY swapchainPeakSizeChanged)

    // CPU Frame Time
    Q_PROPERTY(int framesSinceResize READ framesSinceResize NOTIFY framesSinceResizeChanged)
    Q_PROPERTY(int minFrameDelta READ minFrameDelta NOTIFY minFrameDeltaChanged)
    Q_PROPERTY(int maxFrameDelta READ maxFrameDelta NOTIFY maxFrameDeltaChanged)
    Q_PROPERTY(float averageFrameDelta READ averageFrameDelta NOTIFY averageFrameDeltaChanged)

    // GPU Frame Time
    Q_PROPERTY(float minGpuFrameTime READ minGpuFrameTime NOTIFY minGpuFrameTimeChanged)
    Q_PROPERTY(float maxGpuFrameTime READ maxGpuFrameTime NOTIFY maxGpuFrameTimeChanged)
    Q_PROPERTY(float averageGpuFrameTime READ averageGpuFrameTime NOTIFY averageGpuFrameTimeChanged)

    // GPU Memory
    Q_PROPERTY(int realGpuAllocCount READ realGpuAllocCount NOTIFY realGpuAllocCountChanged)
    Q_PROPERTY(int subGpuAllocCount READ subGpuAllocCount NOTIFY subGpuAllocCountChanged)
    Q_PROPERTY(int totalGpuAllocSize READ totalGpuAllocSize NOTIFY totalGpuAllocSizeChanged)
    Q_PROPERTY(int unusedGpuAllocSize READ unusedGpuAllocSize NOTIFY unusedGpuAllocSizeChanged)


    QML_ELEMENT
public:
    explicit RhiProfiler(QObject *parent = nullptr);

    bool connected() const;
    int bufferCount() const;
    int bufferTotalSize() const;
    int bufferPeakSize() const;
    int bufferStagingTotalSize() const;
    int bufferStagingPeakSize() const;
    int textureCount() const;
    int textureTotalSize() const;
    int texturePeakSize() const;
    int textureStagingTotalSize() const;
    int textureStagingPeakSize() const;
    int swapchainCount() const;
    int swapchainTotalSize() const;
    int swapchainPeakSize() const;
    int framesSinceResize() const;
    int minFrameDelta() const;
    int maxFrameDelta() const;
    float averageFrameDelta() const;
    float minGpuFrameTime() const;
    float maxGpuFrameTime() const;
    float averageGpuFrameTime() const;
    int realGpuAllocCount() const;
    int subGpuAllocCount() const;
    int totalGpuAllocSize() const;
    int unusedGpuAllocSize() const;

signals:
    void connectedChanged();
    void bufferCountChanged();
    void bufferTotalSizeChanged();
    void bufferPeakSizeChanged();
    void bufferStagingTotalSizeChanged();
    void bufferStagingPeakSizeChanged();
    void textureCountChanged();
    void textureTotalSizeChanged();
    void texturePeakSizeChanged();
    void textureStagingTotalSizeChanged();
    void textureStagingPeakSizeChanged();
    void swapchainCountChanged();
    void swapchainTotalSizeChanged();
    void swapchainPeakSizeChanged();
    void framesSinceResizeChanged();
    void minFrameDeltaChanged();
    void maxFrameDeltaChanged();
    void averageFrameDeltaChanged();
    void minGpuFrameTimeChanged();
    void maxGpuFrameTimeChanged();
    void averageGpuFrameTimeChanged();
    void realGpuAllocCountChanged();
    void subGpuAllocCountChanged();
    void totalGpuAllocSizeChanged();
    void unusedGpuAllocSizeChanged();

private slots:
    void handleClientConnected();
    void handleClientDisconnected();
    void handleLineRecieved(const QByteArray &line);
    void handleBuffersUpdated();
    void handleTexturesUpdated();
    void handleSwapchainsUpdated();
    void handleFrameTimeUpdated();
    void handleGpuFrameTimeUpdated();
    void handleGpuMemAllocStatsUpdated();

private:
    RhiProfilerTracker m_tracker;
    RhiProfilerParser m_parser;
    RhiProfilerServer m_server;

    bool m_connected = false;
    int m_bufferCount = 0;
    int m_bufferTotalSize = 0;
    int m_bufferPeakSize = 0;
    int m_bufferStagingTotalSize = 0;
    int m_bufferStagingPeakSize = 0;
    int m_textureCount = 0;
    int m_textureTotalSize = 0;
    int m_texturePeakSize = 0;
    int m_textureStagingTotalSize = 0;
    int m_textureStagingPeakSize = 0;
    int m_swapchainCount = 0;
    int m_swapchainTotalSize = 0;
    int m_swapchainPeakSize = 0;
    int m_framesSinceResize = 0;
    int m_minFrameDelta = 0;
    int m_maxFrameDelta = 0;
    float m_averageFrameDelta = 0.0f;
    float m_minGpuFrameTime = 0.0f;
    float m_maxGpuFrameTime = 0.0f;
    float m_averageGpuFrameTime = 0.0f;
    int m_realGpuAllocCount = 0;
    int m_subGpuAllocCount = 0;
    int m_totalGpuAllocSize = 0;
    int m_unusedGpuAllocSize = 0;
};

#endif // RHIPROFILER_H
