/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rhiprofiler.h"

RhiProfiler::RhiProfiler(QObject *parent) : QObject(parent)
{
    connect(&m_parser, &RhiProfilerParser::eventReceived,
            &m_tracker, &RhiProfilerTracker::handleEvent);

    connect(&m_server, &RhiProfilerServer::clientConnected, this, &RhiProfiler::handleClientConnected);
    connect(&m_server, &RhiProfilerServer::clientDisconnected, this, &RhiProfiler::handleClientDisconnected);
    connect(&m_server, &RhiProfilerServer::lineReceived, this, &RhiProfiler::handleLineRecieved);

    connect(&m_tracker, &RhiProfilerTracker::buffersTouched, this, &RhiProfiler::handleBuffersUpdated);
    connect(&m_tracker, &RhiProfilerTracker::texturesTouched, this, &RhiProfiler::handleTexturesUpdated);
    connect(&m_tracker, &RhiProfilerTracker::swapchainsTouched, this, &RhiProfiler::handleSwapchainsUpdated);
    connect(&m_tracker, &RhiProfilerTracker::frameTimeTouched, this, &RhiProfiler::handleFrameTimeUpdated);
    connect(&m_tracker, &RhiProfilerTracker::gpuFrameTimeTouched, this, &RhiProfiler::handleGpuFrameTimeUpdated);
    connect(&m_tracker, &RhiProfilerTracker::gpuMemAllocStatsTouched, this, &RhiProfiler::handleGpuMemAllocStatsUpdated);

    if (!m_server.listen(QHostAddress::Any, 30667))
        qFatal("Failed to start server: %s", qPrintable(m_server.errorString()));
}

bool RhiProfiler::connected() const
{
    return m_connected;
}

void RhiProfiler::handleClientConnected()
{
    if (m_connected == true)
        return;
    m_connected = true;
    emit connectedChanged();
}

void RhiProfiler::handleClientDisconnected()
{
    if (m_connected == false)
        return;
    m_connected = false;
    m_tracker.reset();
    emit connectedChanged();
}

void RhiProfiler::handleLineRecieved(const QByteArray &line)
{
    m_parser.feed(line);
}

void RhiProfiler::handleBuffersUpdated()
{
    if (m_tracker.m_buffers.count() != m_bufferCount) {
        m_bufferCount = m_tracker.m_buffers.count();
        emit bufferCountChanged();
    }
    if (m_tracker.m_totalBufferApproxByteSize != m_bufferTotalSize) {
        m_bufferTotalSize = m_tracker.m_totalBufferApproxByteSize;
        emit bufferTotalSizeChanged();
    }
    if (m_tracker.m_peakBufferApproxByteSize != m_bufferPeakSize) {
        m_bufferPeakSize = m_tracker.m_peakBufferApproxByteSize;
        emit bufferPeakSizeChanged();
    }
    if (m_tracker.m_totalStagingBufferApproxByteSize != m_bufferStagingTotalSize) {
        m_bufferStagingTotalSize = m_tracker.m_totalStagingBufferApproxByteSize;
        emit bufferStagingTotalSizeChanged();
    }
    if (m_tracker.m_peakStagingBufferApproxByteSize != m_bufferStagingPeakSize) {
        m_bufferStagingPeakSize = m_tracker.m_peakStagingBufferApproxByteSize;
        emit bufferStagingPeakSizeChanged();
    }
}

void RhiProfiler::handleTexturesUpdated()
{
    if (m_tracker.m_textures.count() != m_textureCount) {
        m_textureCount = m_tracker.m_textures.count();
        emit textureCountChanged();
    }
    if (m_tracker.m_totalTextureApproxByteSize != m_textureTotalSize) {
        m_textureTotalSize = m_tracker.m_totalTextureApproxByteSize;
        emit textureTotalSizeChanged();
    }
    if (m_tracker.m_peakTextureApproxByteSize != m_texturePeakSize) {
        m_texturePeakSize = m_tracker.m_peakTextureApproxByteSize;
        emit texturePeakSizeChanged();
    }
    if (m_tracker.m_totalTextureStagingBufferApproxByteSize != m_textureStagingTotalSize) {
        m_textureStagingTotalSize = m_tracker.m_totalTextureApproxByteSize;
        emit textureStagingTotalSizeChanged();
    }
    if (m_tracker.m_peakTextureStagingBufferApproxByteSize != m_textureStagingPeakSize) {
        m_textureStagingPeakSize = m_tracker.m_peakTextureStagingBufferApproxByteSize;
        emit textureStagingPeakSizeChanged();
    }
}

void RhiProfiler::handleSwapchainsUpdated()
{
    if (m_tracker.m_swapchains.count() != m_swapchainCount) {
        m_swapchainCount = m_tracker.m_swapchains.count();
        emit swapchainCountChanged();
    }
    if (m_tracker.m_totalSwapChainApproxByteSize != m_swapchainTotalSize) {
        m_swapchainTotalSize = m_tracker.m_totalSwapChainApproxByteSize;
        emit swapchainTotalSizeChanged();
    }
    if (m_tracker.m_peakSwapChainApproxByteSize != m_swapchainPeakSize) {
        m_swapchainPeakSize = m_tracker.m_peakSwapChainApproxByteSize;
        emit swapchainPeakSizeChanged();
    }
}

void RhiProfiler::handleFrameTimeUpdated()
{
    if (m_tracker.m_lastFrameTime.framesSinceResize != m_framesSinceResize) {
        m_framesSinceResize = m_tracker.m_lastFrameTime.framesSinceResize;
        emit framesSinceResizeChanged();
    }
    if (m_tracker.m_lastFrameTime.minDelta != m_minFrameDelta) {
        m_minFrameDelta = m_tracker.m_lastFrameTime.minDelta;
        emit minFrameDeltaChanged();
    }
    if (m_tracker.m_lastFrameTime.maxDelta != m_maxFrameDelta) {
        m_maxFrameDelta = m_tracker.m_lastFrameTime.maxDelta;
        emit maxFrameDeltaChanged();
    }
    if (m_tracker.m_lastFrameTime.avgDelta != m_averageFrameDelta) {
        m_averageFrameDelta = m_tracker.m_lastFrameTime.avgDelta;
        emit averageFrameDeltaChanged();
    }
}

void RhiProfiler::handleGpuFrameTimeUpdated()
{
    if (m_tracker.m_lastGpuFrameTime.minTime != m_minGpuFrameTime) {
        m_minGpuFrameTime = m_tracker.m_lastGpuFrameTime.minTime;
        emit minGpuFrameTimeChanged();
    }
    if (m_tracker.m_lastGpuFrameTime.maxTime != m_maxGpuFrameTime) {
        m_maxGpuFrameTime = m_tracker.m_lastGpuFrameTime.maxTime;
        emit maxGpuFrameTimeChanged();
    }
    if (m_tracker.m_lastGpuFrameTime.avgTime != m_averageGpuFrameTime) {
        m_averageGpuFrameTime = m_tracker.m_lastGpuFrameTime.avgTime;
        emit averageGpuFrameTimeChanged();
    }
}

void RhiProfiler::handleGpuMemAllocStatsUpdated()
{
    if (m_tracker.m_lastGpuMemAllocStats.realAllocCount != m_realGpuAllocCount) {
        m_realGpuAllocCount = m_tracker.m_lastGpuMemAllocStats.realAllocCount;
        emit realGpuAllocCountChanged();
    }
    if (m_tracker.m_lastGpuMemAllocStats.subAllocCount != m_subGpuAllocCount) {
        m_subGpuAllocCount = m_tracker.m_lastGpuMemAllocStats.subAllocCount;
        emit subGpuAllocCountChanged();
    }
    if (m_tracker.m_lastGpuMemAllocStats.totalSize != m_totalGpuAllocSize) {
        m_totalGpuAllocSize = m_tracker.m_lastGpuMemAllocStats.totalSize;
        emit totalGpuAllocSizeChanged();
    }
    if (m_tracker.m_lastGpuMemAllocStats.unusedSize != m_unusedGpuAllocSize) {
        m_unusedGpuAllocSize = m_tracker.m_lastGpuMemAllocStats.unusedSize;
        emit unusedGpuAllocSizeChanged();
    }
}

int RhiProfiler::bufferCount() const
{
    return m_bufferCount;
}

int RhiProfiler::bufferTotalSize() const
{
    return m_bufferTotalSize;
}

int RhiProfiler::bufferPeakSize() const
{
    return m_bufferPeakSize;
}

int RhiProfiler::bufferStagingTotalSize() const
{
    return m_bufferStagingTotalSize;
}

int RhiProfiler::bufferStagingPeakSize() const
{
    return m_bufferStagingPeakSize;
}

int RhiProfiler::textureCount() const
{
    return m_textureCount;
}

int RhiProfiler::textureTotalSize() const
{
    return m_textureTotalSize;
}

int RhiProfiler::texturePeakSize() const
{
    return m_texturePeakSize;
}

int RhiProfiler::textureStagingTotalSize() const
{
    return m_textureStagingTotalSize;
}

int RhiProfiler::textureStagingPeakSize() const
{
    return m_textureStagingPeakSize;
}

int RhiProfiler::swapchainCount() const
{
    return m_swapchainCount;
}

int RhiProfiler::swapchainTotalSize() const
{
    return m_swapchainTotalSize;
}

int RhiProfiler::swapchainPeakSize() const
{
    return m_swapchainPeakSize;
}

int RhiProfiler::framesSinceResize() const
{
    return m_framesSinceResize;
}

int RhiProfiler::minFrameDelta() const
{
    return m_minFrameDelta;
}

int RhiProfiler::maxFrameDelta() const
{
    return m_maxFrameDelta;
}

float RhiProfiler::averageFrameDelta() const
{
    return m_averageFrameDelta;
}

float RhiProfiler::minGpuFrameTime() const
{
    return m_minGpuFrameTime;
}

float RhiProfiler::maxGpuFrameTime() const
{
    return m_maxGpuFrameTime;
}

float RhiProfiler::averageGpuFrameTime() const
{
    return m_averageGpuFrameTime;
}

int RhiProfiler::realGpuAllocCount() const
{
    return m_realGpuAllocCount;
}

int RhiProfiler::subGpuAllocCount() const
{
    return m_subGpuAllocCount;
}

int RhiProfiler::totalGpuAllocSize() const
{
    return m_totalGpuAllocSize;
}

int RhiProfiler::unusedGpuAllocSize() const
{
    return m_unusedGpuAllocSize;
}
