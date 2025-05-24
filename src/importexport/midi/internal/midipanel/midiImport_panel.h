#ifndef IMPORTMIDI_PANEL_H
#define IMPORTMIDI_PANEL_H

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"
#include "importexport/midi/internal/midiimport/importmidi_inner.h"

#include "framework/uicomponents/view/abstractmenumodel.h"

#include "context/iglobalcontext.h"

#include <QObject>
#include <QString>
#include <qqml.h>

namespace Ui {
      class MidiImportPanel;
      }

namespace mu::iex::midi {
    class TracksModel;
    class OperationsModel;
   }
   
namespace Ms {
      
class OperationsDelegate;

class MidiImportPanel : public muse::uicomponents::AbstractMenuModel
      {
      Q_OBJECT
      Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
      Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
      QML_ELEMENT

   public:
      explicit MidiImportPanel(QObject *parent = nullptr);
      ~MidiImportPanel();
      
      //Q_INVOKABLE void load() override;  
      QString userName();
      void setUserName(const QString &userName);
      void setMidiFile(const QString &fileName);
      bool isVisible() const;
      void setVisible(bool visible);
      void excludeMidiFile(const QString &fileName);
      bool isPreferredVisible() const { return _preferredVisible; }
      void setPreferredVisible(bool visible);
      void setReopenInProgress();

      static bool isMidiFile(const QString &fileName);

      void instrumentTemplatesChanged();

   signals:
      void visibleChanged();
      void closeClicked();
      void userNameChanged();

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

      Ui::MidiImportPanel *_ui;
      QTimer *_updateUiTimer;

      mu::iex::midi::TracksModel *_model;
      OperationsDelegate *_delegate;
      bool _preferredVisible;
      bool _importInProgress;
      bool _reopenInProgress;
      QString _midiFile;
      muse::Inject<mu::context::IGlobalContext> globalContext = { this };

      QString m_userName;
      bool _visible = false;
    };

} // namespace Ms


#endif // IMPORTMIDI_PANEL_H
