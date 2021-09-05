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

#include "rhiprofilertracker.h"

RhiProfilerTracker::RhiProfilerTracker(QObject *parent) : QObject(parent)
{
    reset();
}

void RhiProfilerTracker::reset() {
    m_buffers.clear();
    m_textures.clear();
    m_totalBufferApproxByteSize = 0;
    m_peakBufferApproxByteSize = 0;
    m_totalStagingBufferApproxByteSize = 0;
    m_peakStagingBufferApproxByteSize = 0;
    m_totalTextureApproxByteSize = 0;
    m_peakTextureApproxByteSize = 0;
    m_totalTextureStagingBufferApproxByteSize = 0;
    m_peakTextureStagingBufferApproxByteSize = 0;
    m_totalSwapChainApproxByteSize = 0;
    m_peakSwapChainApproxByteSize = 0;
    m_lastFrameTime = FrameTime();
    m_lastGpuFrameTime = GpuFrameTime();
    m_lastGpuMemAllocStats = GpuMemAllocStats();
    emit buffersTouched();
    emit texturesTouched();
    emit swapchainsTouched();
    emit frameTimeTouched();
    emit gpuFrameTimeTouched();
    emit gpuMemAllocStatsTouched();
}

void RhiProfilerTracker::handleEvent(const RhiProfilerParser::Event &e)
{
    switch (e.op) {
    case QRhiProfiler::NewBuffer:
    {
        Buffer b;
        b.lastTimestamp = e.timestamp;
        b.resourceName = e.resourceName;
        // type,0,usage,1,logical_size,84,effective_size,84,backing_gpu_buf_count,1,backing_cpu_buf_count,0
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("effective_size"))
                b.effectiveSize = p.intValue;
            else if (p.key == QByteArrayLiteral("backing_gpu_buf_count"))
                b.backingGpuBufCount = p.intValue;
        }
        m_totalBufferApproxByteSize += b.effectiveSize * b.backingGpuBufCount;
        m_peakBufferApproxByteSize = qMax(m_peakBufferApproxByteSize, m_totalBufferApproxByteSize);
        m_buffers.insert(e.resource, b);
        emit buffersTouched();
    }
        break;
    case QRhiProfiler::ReleaseBuffer:
    {
        auto it = m_buffers.find(e.resource);
        if (it != m_buffers.end()) {
            m_totalBufferApproxByteSize -= it->effectiveSize * it->backingGpuBufCount;
            m_buffers.erase(it);
            emit buffersTouched();
        }
    }
        break;

    case QRhiProfiler::NewBufferStagingArea:
    {
        qint64 slot = -1;
        qint64 size = 0;
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("slot"))
                slot = p.intValue;
            else if (p.key == QByteArrayLiteral("size"))
                size = p.intValue;
        }
        if (slot >= 0 && slot < MAX_STAGING_SLOTS) {
            auto it = m_buffers.find(e.resource);
            if (it != m_buffers.end()) {
                it->stagingExtraSize[slot] = size;
                m_totalStagingBufferApproxByteSize += size;
                m_peakStagingBufferApproxByteSize = qMax(m_peakStagingBufferApproxByteSize, m_totalStagingBufferApproxByteSize);
                emit buffersTouched();
            }
        }
    }
        break;
    case QRhiProfiler::ReleaseBufferStagingArea:
    {
        qint64 slot = -1;
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("slot"))
                slot = p.intValue;
        }
        if (slot >= 0 && slot < MAX_STAGING_SLOTS) {
            auto it = m_buffers.find(e.resource);
            if (it != m_buffers.end()) {
                m_totalStagingBufferApproxByteSize -= it->stagingExtraSize[slot];
                it->stagingExtraSize[slot] = 0;
                emit buffersTouched();
            }
        }
    }
        break;

    case QRhiProfiler::NewTexture:
    {
        Texture t;
        t.lastTimestamp = e.timestamp;
        t.resourceName = e.resourceName;
        // width,256,height,256,format,1,owns_native_resource,1,mip_count,9,layer_count,1,effective_sample_count,1,approx_byte_size,349524
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("approx_byte_size"))
                t.approxByteSize = p.intValue;
            else if (p.key == QByteArrayLiteral("owns_native_resource"))
                t.ownsNativeResource = p.intValue;
        }
        if (t.ownsNativeResource) {
            m_totalTextureApproxByteSize += t.approxByteSize;
            m_peakTextureApproxByteSize = qMax(m_peakTextureApproxByteSize, m_totalTextureApproxByteSize);
        }
        m_textures.insert(e.resource, t);
        emit texturesTouched();
    }
        break;
    case QRhiProfiler::ReleaseTexture:
    {
        auto it = m_textures.find(e.resource);
        if (it != m_textures.end()) {
            if (it->ownsNativeResource)
                m_totalTextureApproxByteSize -= it->approxByteSize;
            m_textures.erase(it);
            emit texturesTouched();
        }
    }
        break;

    case QRhiProfiler::NewTextureStagingArea:
    {
        qint64 slot = -1;
        qint64 size = 0;
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("slot"))
                slot = p.intValue;
            else if (p.key == QByteArrayLiteral("size"))
                size = p.intValue;
        }
        if (slot >= 0 && slot < MAX_STAGING_SLOTS) {
            auto it = m_textures.find(e.resource);
            if (it != m_textures.end()) {
                it->stagingExtraSize[slot] = size;
                m_totalTextureStagingBufferApproxByteSize += size;
                m_peakTextureStagingBufferApproxByteSize = qMax(m_peakTextureStagingBufferApproxByteSize, m_totalTextureStagingBufferApproxByteSize);
                emit texturesTouched();
            }
        }
    }
        break;
    case QRhiProfiler::ReleaseTextureStagingArea:
    {
        qint64 slot = -1;
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("slot"))
                slot = p.intValue;
        }
        if (slot >= 0 && slot < MAX_STAGING_SLOTS) {
            auto it = m_textures.find(e.resource);
            if (it != m_textures.end()) {
                m_totalTextureStagingBufferApproxByteSize -= it->stagingExtraSize[slot];
                it->stagingExtraSize[slot] = 0;
                emit texturesTouched();
            }
        }
    }
        break;

    case QRhiProfiler::ResizeSwapChain:
    {
        auto it = m_swapchains.find(e.resource);
        if (it != m_swapchains.end())
            m_totalSwapChainApproxByteSize -= it->approxByteSize;

        SwapChain s;
        s.lastTimestamp = e.timestamp;
        s.resourceName = e.resourceName;
        // width,1280,height,720,buffer_count,2,msaa_buffer_count,0,effective_sample_count,1,approx_total_byte_size,7372800
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("approx_total_byte_size"))
                s.approxByteSize = p.intValue;
        }
        m_totalSwapChainApproxByteSize += s.approxByteSize;
        m_peakSwapChainApproxByteSize = qMax(m_peakSwapChainApproxByteSize, m_totalSwapChainApproxByteSize);
        m_swapchains.insert(e.resource, s);
        emit swapchainsTouched();
    }
        break;
    case QRhiProfiler::ReleaseSwapChain:
    {
        auto it = m_swapchains.find(e.resource);
        if (it != m_swapchains.end()) {
            m_totalSwapChainApproxByteSize -= it->approxByteSize;
            m_swapchains.erase(it);
            emit swapchainsTouched();
        }
    }
        break;

    case QRhiProfiler::GpuFrameTime:
    {
        // Fmin_ms_gpu_frame_time,0.15488,Fmax_ms_gpu_frame_time,0.494592,Favg_ms_gpu_frame_time,0.33462
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("min_ms_gpu_frame_time"))
                m_lastGpuFrameTime.minTime = p.floatValue;
            else if (p.key == QByteArrayLiteral("max_ms_gpu_frame_time"))
                m_lastGpuFrameTime.maxTime = p.floatValue;
            else if (p.key == QByteArrayLiteral("avg_ms_gpu_frame_time"))
                m_lastGpuFrameTime.avgTime = p.floatValue;
        }
        emit gpuFrameTimeTouched();
    }
        break;
    case QRhiProfiler::FrameToFrameTime:
    {
        // frames_since_resize,121,min_ms_frame_delta,9,max_ms_frame_delta,33,Favg_ms_frame_delta,16.1167
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("frames_since_resize"))
                m_lastFrameTime.framesSinceResize = p.intValue;
            else if (p.key == QByteArrayLiteral("min_ms_frame_delta"))
                m_lastFrameTime.minDelta = p.intValue;
            else if (p.key == QByteArrayLiteral("max_ms_frame_delta"))
                m_lastFrameTime.maxDelta = p.intValue;
            else if (p.key == QByteArrayLiteral("avg_ms_frame_delta"))
                m_lastFrameTime.avgDelta = p.floatValue;
        }
        emit frameTimeTouched();
    }
        break;

    case QRhiProfiler::GpuMemAllocStats:
    {
        // real_alloc_count,2,sub_alloc_count,154,total_size,10752,unused_size,50320896
        for (const RhiProfilerParser::Event::Param &p : e.params) {
            if (p.key == QByteArrayLiteral("real_alloc_count"))
                m_lastGpuMemAllocStats.realAllocCount = p.intValue;
            else if (p.key == QByteArrayLiteral("sub_alloc_count"))
                m_lastGpuMemAllocStats.subAllocCount = p.intValue;
            else if (p.key == QByteArrayLiteral("total_size"))
                m_lastGpuMemAllocStats.totalSize = p.intValue;
            else if (p.key == QByteArrayLiteral("unused_size"))
                m_lastGpuMemAllocStats.unusedSize = p.intValue;
        }
        emit gpuMemAllocStatsTouched();
    }
        break;

    default:
        break;
    }
}
