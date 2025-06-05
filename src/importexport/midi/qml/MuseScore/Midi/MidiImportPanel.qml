import QtQuick 2.15

import Muse.Ui 1.0
import MuseScore.Midi 1.0

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Muse.UiComponents 1.0
import MuseScore.Palette 1.0

Item {    
    id: root

    TracksModel {
        id: tracksModel
    }
    
    MidiImportPanelNew {
        id: midiPanel
        Component.onCompleted: {
            midiPanel.fillCharsetList()
            midiPanel.setModel(tracksModel)
        }
    }


    property alias navigationSection: navPanel.section
    property alias contentNavigationPanelOrderStart: navPanel.order

    NavigationPanel {
        id: navPanel
        name: "MidiImportPanel"
        direction: NavigationPanel.Vertical
        enabled: root.enabled && root.visible
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        anchors.margins: 12

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            spacing: 6

            Item { Layout.fillWidth: true }

            Label {
                text: qsTr("Text charset:")
                verticalAlignment: Label.AlignVCenter
            }

            ComboBox {
                id: comboBoxCharset
                editable: true
                Layout.preferredWidth: 150
                model: midiPanel.charsetList
                currentIndex: midiPanel.charsetList.indexOf(midiPanel.currentCharset)
                ToolTip.text: qsTr("Select text Charset")

                onCurrentTextChanged: {
                    midiPanel.currentCharset = currentText
                }
            }

            Button {
                text: qsTr("Apply")
                ToolTip.text: qsTr("Apply")
                onClicked: midiPanel.apply()
            }

            Button {
                text: qsTr("Clear Changes")
                ToolTip.text: qsTr("Clear Changes")
                onClicked: midiPanel.cancel()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#f9f9f9"
            border.color: "#cccccc"
            border.width: 1
            radius: 6

            Text {
                anchors.centerIn: parent
                text: "TracksView (placeholder)"
                font.italic: true
                color: "#999"
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            contentWidth: tableRow.implicitWidth
            clip: true

            Rectangle {
                id: tableRow
                height: 40
                width: parent.width
                color: "#ffffff"
                border.color: "#ddd"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    spacing: 10
                    Repeater {
                        model: ["Channel", "Instrument", "Sound", "Quantisation", "Voices", "Tuplets", "Human", "Clef", "Staccato", "Swing"]
                        delegate: Rectangle {
                            width: 100
                            height: 40
                            color: "#f0f0f0"
                            border.color: "#ccc"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: modelData
                                font.pointSize: 10
                            }
                        }
                    }
                }
            }
        }
    }
}
