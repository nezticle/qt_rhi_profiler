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
