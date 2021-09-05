import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import io.qad.rhiprofiler

Item {

    required property var profiler

    Label {
        visible: !profiler.connected
        anchors.centerIn: parent
        text: "Launch a Qt Quick application with environment
QSG_RHI_PROFILE=1
QSG_RHI_PROFILE_HOST=(ipaddress)
(resource memory usage reporting works best with the Vulkan backend)"
        font.pointSize: 24
    }

    ColumnLayout {
        visible: profiler.connected
        Label {
            text: "RHI Statistics"
        }

        GroupBox {
            title: "Buffers"
            ColumnLayout {
                Label {
                    text: "Count: " + profiler.bufferCount
                }
                Label {
                    text: "Total Size: " + profiler.bufferTotalSize + " bytes"
                }
                Label {
                    text: "Peak Size: " + profiler.bufferPeakSize + " bytes"
                }
                Label {
                    text: "Total Staging Size: " + profiler.bufferStagingTotalSize + " bytes"
                }
                Label {
                    text: "Peak Staging Size: " + profiler.bufferStagingPeakSize + " bytes"
                }
            }
        }
        GroupBox {
            title: "Textures"
            ColumnLayout {
                Label {
                    text: "Count: " + profiler.textureCount
                }
                Label {
                    text: "Total Size: " + profiler.textureTotalSize + " bytes"
                }
                Label {
                    text: "Peak Size: " + profiler.texturePeakSize + " bytes"
                }
                Label {
                    text: "Total Staging Size: " + profiler.textureStagingTotalSize + " bytes"
                }
                Label {
                    text: "Peak Staging Size: " + profiler.textureStagingPeakSize + " bytes"
                }
            }
        }
        GroupBox {
            title: "Swapchains"
            ColumnLayout {
                Label {
                    text: "Count: " + profiler.swapchainCount
                }
                Label {
                    text: "Total Size: " + profiler.swapchainTotalSize + " bytes"
                }
                Label {
                    text: "Peak Size: " + profiler.swapchainPeakSize + " bytes"
                }
            }
        }

        GroupBox {
            title: "CPU Frame Time"
            ColumnLayout {
                Label {
                    text: "Frames since Resize: " + profiler.framesSinceResize
                }
                Label {
                    text: "Min Frame Delta: " + profiler.minFrameDelta + " ms"
                }
                Label {
                    text: "Max Frame Delta: " + profiler.maxFrameDelta + " ms"
                }
                Label {
                    text: "Avg Frame Delta: " + profiler.averageFrameDelta + " ms"
                }
            }
        }
        GroupBox {
            title: "GPU Frame Time"
            ColumnLayout {
                Label {
                    text: "Min Frame Delta: " + profiler.minGpuFrameDelta + " ms"
                }
                Label {
                    text: "Max Frame Delta: " + profiler.maxGpuFrameDelta + " ms"
                }
                Label {
                    text: "Avg Frame Delta: " + profiler.averageGpuFrameDelta + " ms"
                }
            }
        }

        GroupBox {
            title: "GPU Memory"
            ColumnLayout {
                Label {
                    text: "Real Alloc Count: " + profiler.realGpuAllocCount
                }
                Label {
                    text: "Sub Alloc Count: " + profiler.subGpuAllocCount
                }
                Label {
                    text: "Total Alloc Size: " + profiler.totalGpuAllocSize + " bytes"
                }
                Label {
                    text: "Unused Alloc Size: " + profiler.unusedGpuAllocSize + " bytes"
                }
            }
        }
    }
}
