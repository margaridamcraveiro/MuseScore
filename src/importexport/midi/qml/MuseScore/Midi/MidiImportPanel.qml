import QtQuick 2.15

import Muse.Ui 1.0
import MuseScore.Midi 1.0

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
}