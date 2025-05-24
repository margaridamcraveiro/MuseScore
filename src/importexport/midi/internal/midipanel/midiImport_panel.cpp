#include "midiImport_panel.h"

#include "midiImport_delegate.h"
#include "ui_midiImport_panel.h"

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_lyrics.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"
#include "importexport/midi/internal/midiimport/importmidi_inner.h"

#include "framework/uicomponents/view/menuitem.h"

#include "engraving/dom/masterscore.h"

#include "context/iglobalcontext.h"

#include <QFileInfo>
#include <QStringConverter>

namespace Ms {

MidiImportPanel::MidiImportPanel(QObject *parent)
      : AbstractMenuModel(parent)
      , _ui(new Ui::MidiImportPanel)
      , _updateUiTimer(new QTimer)
      , _preferredVisible(false)
      , _importInProgress(false)
      , _reopenInProgress(false)
      {
      //_ui->setupUi(this);

      _model = new mu::iex::midi::TracksModel();
      _delegate = new OperationsDelegate(nullptr, true);
      _ui->tracksView->setModel(_model);
      _ui->tracksView->setItemDelegate(_delegate);

      //setupUi();
      }

MidiImportPanel::~MidiImportPanel()
      {
      delete _ui;
      }

// void MidiImportPanel::load()
// {
//     MenuItemList items {
//         makeViewMenu()
//     };

//     setItems(items);
// }

QString MidiImportPanel::userName(){
    return m_userName;
}

void MidiImportPanel::setUserName(const QString &userName){
    if (userName == m_userName)
        return;

    m_userName = userName;
    emit userNameChanged();
}

bool MidiImportPanel::isVisible() const { return _visible; }

void MidiImportPanel::setVisible(bool visible) {
      if (_visible == visible)
      return;
      _visible = visible;
      emit visibleChanged();
}

void MidiImportPanel::setMidiFile(const QString &fileName)
      {
      if (_reopenInProgress)
            _reopenInProgress = false;
      if (_midiFile == fileName || _importInProgress)
            return;

      _midiFile = fileName;
      updateUi();
      if (!QFile(_midiFile).exists())
            return;

      mu::iex::midi::MidiOperations::Data &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      if (opers.data()->processingsOfOpenedFile == 1) {     // initial processing of MIDI file
            ++opers.data()->processingsOfOpenedFile;
            resetTableViewState();
            _model->clear();
            resetTableViewState();
            _model->reset(opers.data()->trackOpers,
                          mu::iex::midi::MidiLyrics::makeLyricsListForUI(),
                          opers.data()->trackCount,
                          _midiFile,
                          !opers.data()->humanBeatData.beatSet.empty(),
                          opers.data()->hasTempoText,
                          !opers.data()->chordNames.empty());
            saveTableViewState();
            }
      else {            // switch to already opened MIDI file
            _model->reset(opers.data()->trackOpers,
                          mu::iex::midi::MidiLyrics::makeLyricsListForUI(),
                          opers.data()->trackCount,
                          _midiFile,
                          !opers.data()->humanBeatData.beatSet.empty(),
                          opers.data()->hasTempoText,
                          !opers.data()->chordNames.empty());
            restoreTableViewState();
            }

      _ui->tracksView->setFrozenRowCount(_model->frozenRowCount());
      _ui->tracksView->setFrozenColCount(_model->frozenColCount());
      _ui->comboBoxCharset->setCurrentText(opers.data()->charset);
                  // tracks view has multiple headers (need for frozen rows/columns)
                  // so to set all headers special methods there have been implemented
      _ui->tracksView->setHHeaderResizeMode(QHeaderView::ResizeToContents);
      }

void MidiImportPanel::saveTableViewState()
      {
      mu::iex::midi::MidiOperations::Data &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      const QByteArray hData = _ui->tracksView->horizontalHeader()->saveState();
      const QByteArray vData = _ui->tracksView->verticalHeader()->saveState();
      opers.data()->HHeaderData = hData;
      opers.data()->VHeaderData = vData;
      }

void MidiImportPanel::restoreTableViewState()
      {
      mu::iex::midi::MidiOperations::Data &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      const QByteArray hData = opers.data()->HHeaderData;
      const QByteArray vData = opers.data()->VHeaderData;
      _ui->tracksView->restoreHHeaderState(hData);
      _ui->tracksView->restoreVHeaderState(vData);
      }

void MidiImportPanel::resetTableViewState()
      {
      _ui->tracksView->setFrozenRowCount(0);
      _ui->tracksView->setFrozenColCount(0);
      _ui->tracksView->resetHHeader();
      _ui->tracksView->resetVHeader();
      }

bool MidiImportPanel::isMidiFile(const QString &fileName)
      {
      const QString extension = QFileInfo(fileName).suffix().toLower();
      return (extension == "mid" || extension == "midi" || extension == "kar");
      }

void MidiImportPanel::setupUi()
      {
      connect(_updateUiTimer, SIGNAL(timeout()), this, SLOT(updateUi()));
      connect(_ui->pushButtonApply, SIGNAL(clicked()), SLOT(applyMidiImport()));
      connect(_ui->pushButtonCancel, SIGNAL(clicked()), SLOT(cancelChanges()));
      connect(_ui->pushButtonUp, SIGNAL(clicked()), SLOT(moveTrackUp()));
      connect(_ui->pushButtonDown, SIGNAL(clicked()), SLOT(moveTrackDown()));
      connect(_ui->toolButtonHideMidiPanel, SIGNAL(clicked()), SLOT(hidePanel()));

      // _ui->pushButtonDown->setIcon(*icons[int(Icons::arrowDown_ICON)]);
      
      _updateUiTimer->start(100);
      updateUi();
      _ui->tracksView->setVHeaderDefaultSectionSize(24);
      _ui->comboBoxCharset->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
      fillCharsetList();
      }

void MidiImportPanel::fillCharsetList()
      {
      QFontMetrics fm(_ui->comboBoxCharset->font());

      _ui->comboBoxCharset->clear();
      QList<QByteArray> charsets = {
            // add more encodings if necessary
            "UTF-8",
            "ISO-8859-1",
            "Windows-1252",
            "Shift-JIS",
            "GB18030"
      };
      std::sort(charsets.begin(), charsets.end());
      int idx = 0;
      int maxWidth = 0;
      for (const auto &charset: charsets) {
            _ui->comboBoxCharset->addItem(charset);
            if (charset == mu::iex::midi::MidiCharset::defaultCharset())
                  _ui->comboBoxCharset->setCurrentIndex(idx);
            int newWidth = fm.horizontalAdvance(charset);
            if (newWidth > maxWidth)
                  maxWidth = newWidth;
            ++idx;
            }
      _ui->comboBoxCharset->view()->setMinimumWidth(maxWidth);
      }

void MidiImportPanel::updateUi()
      {
      _ui->pushButtonApply->setEnabled(canImportMidi());
      _ui->pushButtonCancel->setEnabled(canTryCancelChanges());

      const int visualIndex = currentVisualIndex();
      _ui->pushButtonUp->setEnabled(canMoveTrackUp(visualIndex));
      _ui->pushButtonDown->setEnabled(canMoveTrackDown(visualIndex));
      }

void MidiImportPanel::hidePanel(){
      if (isVisible()) {
            setVisible(false);
            emit closeClicked();
            _preferredVisible = false;  
      }
}

void MidiImportPanel::setReorderedIndexes()
      {
      auto &opers = mu::iex::midi::midiImportOperations;
      for (int i = 0; i != _model->trackCount(); ++i) {
            const int trackRow = _model->rowFromTrackIndex(i);
            const int reorderedRow = _ui->tracksView->verticalHeader()->logicalIndex(trackRow);
            const int reorderedIndex = _model->trackIndexFromRow(reorderedRow);
            opers.data()->trackOpers.trackIndexAfterReorder.setValue(reorderedIndex, i);
            }
      }

void MidiImportPanel::applyMidiImport()
      {
      if (!canImportMidi())
            return;

      _importInProgress = true;

      auto &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);
                  // update charset
      if (opers.data()->charset != _ui->comboBoxCharset->currentText()) {
            opers.data()->charset = _ui->comboBoxCharset->currentText();
                        // need to update model because of charset change
            _model->updateCharset();
            }

      _model->notifyAllApplied();
      opers.data()->trackOpers = _model->trackOpers();
      setReorderedIndexes();
            // prevent from showing save request dialog on every 'apply MIDI import' call
      MasterScore* score =  (MasterScore*) globalContext()->currentNotation()->elements()->msScore();
      
      //openScore(_midiFile);
      saveTableViewState();
      _importInProgress = false;
      }

void MidiImportPanel::cancelChanges()
      {
      if (canTryCancelChanges())
            doCancelChanges();
      }

void MidiImportPanel::doCancelChanges()
      {
      auto &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      _model->reset(opers.data()->trackOpers,
                    mu::iex::midi::MidiLyrics::makeLyricsListForUI(),
                    opers.data()->trackCount,
                    _midiFile,
                    !opers.data()->humanBeatData.beatSet.empty(),
                    opers.data()->hasTempoText,
                    !opers.data()->chordNames.empty());

      restoreTableViewState();
      _ui->comboBoxCharset->setCurrentText(opers.data()->charset);
                  // tracks view has multiple headers (need for frozen rows/columns)
                  // so to set all headers special methods there have been implemented
      _ui->tracksView->setHHeaderResizeMode(QHeaderView::ResizeToContents);
      }

void MidiImportPanel::instrumentTemplatesChanged()
      {
      if (fileDataAvailable(_midiFile))
            doCancelChanges();
      }

bool MidiImportPanel::canImportMidi() const
      {
      return QFile(_midiFile).exists() && _model->trackCountForImport() > 0;
      }

bool MidiImportPanel::canTryCancelChanges() const
      {
      if (!_model->isAllApplied())
            return true;

      auto &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);
      if (!opers.data())
            return false;

      if (opers.data()->charset != _ui->comboBoxCharset->currentText())
            return true;

      const QByteArray vData = _ui->tracksView->verticalHeader()->saveState();
      return opers.data()->VHeaderData != vData;
      }

bool MidiImportPanel::fileDataAvailable(const QString& midiFile)
      {
      auto &opers = mu::iex::midi::midiImportOperations;
      mu::iex::midi::MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, midiFile);
      return bool(opers.data());
      }

bool MidiImportPanel::canMoveTrackUp(int visualIndex) const
      {
      return _model->trackCount() > 1 && visualIndex > 1;
      }

bool MidiImportPanel::canMoveTrackDown(int visualIndex) const
      {
      return _model->trackCount() > 1
                  && visualIndex < _model->trackCount() && visualIndex > 0;
      }

void MidiImportPanel::moveTrackUp()
      {
      int visIndex = currentVisualIndex();
      if (canMoveTrackUp(visIndex))
            _ui->tracksView->verticalHeader()->moveSection(visIndex, visIndex - 1);
      }

void MidiImportPanel::moveTrackDown()
      {
      const int visIndex = currentVisualIndex();
      if (canMoveTrackDown(visIndex))
            _ui->tracksView->verticalHeader()->moveSection(visIndex, visIndex + 1);
      }

int MidiImportPanel::currentVisualIndex() const
      {
      const auto selectedItems = _ui->tracksView->selectionModel()->selection().indexes();
      int curRow = -1;
      if (!selectedItems.isEmpty())
            curRow = selectedItems[0].row();
      const int visIndex = _ui->tracksView->verticalHeader()->visualIndex(curRow);

      return visIndex;
      }

void MidiImportPanel::excludeMidiFile(const QString &fileName)
      {
                  // because button "Apply" of MIDI import operations
                  // causes reopen of the current score
                  // we need to prevent MIDI import panel from closing at that moment
      if (_importInProgress || _reopenInProgress)
            return;

      auto &opers = mu::iex::midi::midiImportOperations;
      opers.excludeMidiFile(fileName);

      if (fileName == _midiFile) {
            _midiFile = "";
            resetTableViewState();
            _model->clear();
            resetTableViewState();
            }
      }

void MidiImportPanel::setPreferredVisible(bool visible)
      {
      _preferredVisible = visible;
      }

void MidiImportPanel::setReopenInProgress()
      {
      _reopenInProgress = true;
      }

} // namespace Ms
