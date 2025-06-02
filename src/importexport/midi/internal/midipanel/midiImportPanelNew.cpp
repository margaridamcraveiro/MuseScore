#include "midiImportPanelNew.h"

#include "midiImport_delegate.h"

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_lyrics.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"
#include "importexport/midi/internal/midiimport/importmidi_inner.h"

#include "framework/uicomponents/view/menuitem.h"

#include "engraving/dom/masterscore.h"

#include "context/iglobalcontext.h"

namespace Ms {

MidiImportPanelNew::MidiImportPanelNew(QObject* parent)
    : QObject(parent)
{}

MidiImportPanelNew::~MidiImportPanelNew() = default;

QString MidiImportPanelNew::userName() const { return m_userName; }
void MidiImportPanelNew::setUserName(const QString& userName) {
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}

bool MidiImportPanelNew::isVisible() const { return m_visible; }
void MidiImportPanelNew::setVisible(bool visible) {
    if (m_visible != visible) {
        m_visible = visible;
        emit visibleChanged();
    }
}

QString MidiImportPanelNew::currentCharset() const { return m_currentCharset; }
void MidiImportPanelNew::setCurrentCharset(const QString& charset) {
    if (m_currentCharset != charset) {
        m_currentCharset = charset;
        emit currentCharsetChanged();
    }
}

QStringList MidiImportPanelNew::charsetList() const {
    return m_charsetList;
}

void MidiImportPanelNew::setMidiFile(const QString& fileName) {
    m_midiFile = fileName;
}

void MidiImportPanelNew::setModel(mu::iex::midi::TracksModel* model) {
    m_model = model;
}

bool MidiImportPanelNew::canImportMidi() const {
    return (m_model != nullptr) && (!m_midiFile.isEmpty());
}

void MidiImportPanelNew::setReorderedIndexes() {
}

void MidiImportPanelNew::saveTableViewState() {
}

void MidiImportPanelNew::apply() {
    if (!canImportMidi())
        return;

    m_importInProgress = true;

    auto& opers = mu::iex::midi::midiImportOperations;
    mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, m_midiFile.toStdString());

    if (opers.data()->charset != m_currentCharset.toStdString()) {
        opers.data()->charset = m_currentCharset.toStdString();
        m_model->updateCharset();
    }

    m_model->notifyAllApplied();
    opers.data()->trackOpers = m_model->trackOpers();

    setReorderedIndexes();

    MasterScore* score =  (MasterScore*) m_globalContext->currentNotation()->elements()->msScore();


    saveTableViewState();

    m_importInProgress = false;
}

void MidiImportPanelNew::cancel() {
}

void MidiImportPanelNew::moveTrackUp(int index) {
}

void MidiImportPanelNew::moveTrackDown(int index) {
}

} // namespace Ms
