#include "midiImportPanelNew.h"

#include "midiImport_delegate.h"

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_lyrics.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"
#include "importexport/midi/internal/midiimport/importmidi_inner.h"

#include "framework/uicomponents/view/menuitem.h"

#include "engraving/dom/masterscore.h"

#include "context/iglobalcontext.h"


#include <QFile>
#include <QDebug>
#include <algorithm>


namespace Ms {

MidiImportPanelNew::MidiImportPanelNew(QObject* parent)
    : QObject(parent)
{
    fillCharsetList(); 
}

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
    if (m_midiFile == fileName)
        return;

    m_midiFile = fileName;
    emit midiFileChanged();

    if (!QFile(fileName).exists())
        return;

    auto& opers = mu::iex::midi::midiImportOperations;
    mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, fileName);

    if (opers.data()->processingsOfOpenedFile == 1) {
        ++opers.data()->processingsOfOpenedFile;
        if (m_model)
            m_model->clear();
    }

    if (m_model) {
        m_model->reset(
            opers.data()->trackOpers,
            mu::iex::midi::MidiLyrics::makeLyricsListForUI(),
            opers.data()->trackCount,
            fileName,
            !opers.data()->humanBeatData.beatSet.empty(),
            opers.data()->hasTempoText,
            !opers.data()->chordNames.empty()
        );
    }

    m_currentCharset = opers.data()->charset;
    emit currentCharsetChanged();
}

void MidiImportPanelNew::setModel(mu::iex::midi::TracksModel* model) {
    m_model = model;

    /*
    if (globalContext->currentNotation()) {
        const QString midiFilePath = globalContext->currentProject()->path();

        if (!midiFilePath.isEmpty()) {
            setMidiFile(midiFilePath); 
        }
    }*/
}


bool MidiImportPanelNew::canImportMidi() const {
    return (m_model != nullptr) && (!m_midiFile.isEmpty());
}

void MidiImportPanelNew::setReorderedIndexes() {
}

void MidiImportPanelNew::saveTableViewState() {
}

void MidiImportPanelNew::apply()
{
    if (!canImportMidi())
        return;

    m_importInProgress = true;

    auto &opers = mu::iex::midi::midiImportOperations;
    mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, m_midiFile);

    if (opers.data()->charset != m_currentCharset) {
        opers.data()->charset = m_currentCharset;
        if (m_model)
            m_model->updateCharset();
    }

    if (m_model) {
        m_model->notifyAllApplied();
        opers.data()->trackOpers = m_model->trackOpers();
    }

    setReorderedIndexes();

    saveTableViewState();

    m_importInProgress = false;
}
void MidiImportPanelNew::cancel() {
    try {
        qDebug() << "CANCEL PRESSED";
        auto& opers = mu::iex::midi::midiImportOperations;
        mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, m_midiFile);

        if (!m_model)
            return;

        m_model->reset(opers.data()->trackOpers,
                       mu::iex::midi::MidiLyrics::makeLyricsListForUI(),
                       opers.data()->trackCount,
                       m_midiFile,
                       !opers.data()->humanBeatData.beatSet.empty(),
                       opers.data()->hasTempoText,
                       !opers.data()->chordNames.empty());

        m_currentCharset = opers.data()->charset;
        emit currentCharsetChanged();
    } catch (const std::exception& e) {
        qWarning() << "Exception in cancel(): " << e.what();
    } catch (...) {
        qWarning() << "Unknown exception in cancel()";
    }
}

void MidiImportPanelNew::fillCharsetList() {
    m_charsetList = {
        "UTF-8",
        "ISO-8859-1",
        "Windows-1252",
        "Shift-JIS",
        "GB18030"
    };

    std::sort(m_charsetList.begin(), m_charsetList.end());
    emit charsetListChanged();
}


} // namespace Ms
