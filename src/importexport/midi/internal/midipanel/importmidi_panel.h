#ifndef IMPORTMIDI_PANEL_H
#define IMPORTMIDI_PANEL_H

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"
#include "importexport/midi/internal/midiimport/importmidi_inner.h"

#include "context/iglobalcontext.h"

namespace Ui {
      class ImportMidiPanel;
      }

namespace mu::iex::midi {
    class TracksModel;
    class OperationsModel;
   }
   
namespace Ms {
      
class OperationsDelegate;

class ImportMidiPanel : public QWidget, public muse::Injectable
      {
      Q_OBJECT

   public:
      explicit ImportMidiPanel(QWidget *parent = 0);
      ~ImportMidiPanel();

      void setMidiFile(const QString &fileName);
      void excludeMidiFile(const QString &fileName);
      bool isPreferredVisible() const { return _preferredVisible; }
      void setPreferredVisible(bool visible);
      void setReopenInProgress();

      static bool isMidiFile(const QString &fileName);

      void instrumentTemplatesChanged();

   signals:
      void closeClicked();

   private slots:
      void updateUi();
      void hidePanel();
      void applyMidiImport();
      void moveTrackUp();
      void moveTrackDown();
      void cancelChanges();

   private:
      void setupUi();
      bool canImportMidi() const;
      bool canTryCancelChanges() const;
      bool canMoveTrackUp(int visualIndex) const;
      bool canMoveTrackDown(int visualIndex) const;
      void setReorderedIndexes();
      int currentVisualIndex() const;
      void saveTableViewState();
      void restoreTableViewState();
      void resetTableViewState();
      void fillCharsetList();

      void doCancelChanges();
      static bool fileDataAvailable(const QString& midiFile);

      Ui::ImportMidiPanel *_ui;
      QTimer *_updateUiTimer;

    mu::iex::midi::TracksModel *_model;
    OperationsDelegate *_delegate;
    bool _preferredVisible;
    bool _importInProgress;
    bool _reopenInProgress;
    QString _midiFile;
    muse::Inject<mu::context::IGlobalContext> globalContext = { this };
    };

} // namespace Ms


#endif // IMPORTMIDI_PANEL_H
