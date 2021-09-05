import QtQuick
import io.qad.rhiprofiler

Window {
    width: 1280
    height: 720
    visible: true
    title: qsTr("Qt RHI Profiler")

    RhiProfiler {
        id: profilerBackend
    }

    ProfilerView {
        anchors.fill: parent
        profiler: profilerBackend
    }
}
