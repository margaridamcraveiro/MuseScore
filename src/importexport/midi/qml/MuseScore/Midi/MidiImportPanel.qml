import QtQuick 2.15

import Muse.Ui 1.0
import MuseScore.Midi 1.0

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Muse.UiComponents 1.0
import MuseScore.Palette 1.0

Item {    
    id: root

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

        // Top Bar estendida com botões e charset
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            spacing: 6

            // Fechar painel
            ToolButton {
                icon.source: "qrc:/data/icons/png/window-close.png"
                ToolTip.text: qsTr("Fechar painel de importação MIDI")
                onClicked: {
                    // lógica de fechar
                }
            }

            // Mover faixa para cima
            ToolButton {
                icon.source: "qrc:/data/icons/arrow_up.svg"
                ToolTip.text: qsTr("Mover faixa para cima")
                onClicked: {
                    // lógica para mover faixa para cima
                }
            }

            // Mover faixa para baixo
            ToolButton {
                icon.source: "qrc:/data/icons/arrow_down.svg"
                ToolTip.text: qsTr("Mover faixa para baixo")
                onClicked: {
                    // lógica para mover faixa para baixo
                }
            }

            Item { Layout.fillWidth: true } // expansor central

            // Charset label
            Label {
                text: qsTr("Text charset:")
                verticalAlignment: Label.AlignVCenter
            }

            // ComboBox de charset
            ComboBox {
                id: comboBoxCharset
                editable: true
                Layout.preferredWidth: 150
                model: ["UTF-8", "ISO-8859-1", "Windows-1252"]
                ToolTip.text: qsTr("Selecionar charset de texto")
                onAccepted: {
                    // lógica para charset
                }
            }

            // Botão Apply
            Button {
                text: qsTr("Apply")
                ToolTip.text: qsTr("Aplicar alterações de importação MIDI")
                onClicked: {
                    // lógica de aplicar
                }
            }

            // Botão Cancel
            Button {
                text: qsTr("Cancel")
                ToolTip.text: qsTr("Cancelar alterações")
                onClicked: {
                    // lógica de cancelar
                }
            }
        }

        // Placeholder para a TracksView
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

        // Simulação de tabela
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
                    spacing: 4
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
