#pragma once

#include <QObject>
#include <QStringList>
#include <QTimer>

#include "importexport/midi/internal/midiimport/importmidi_model.h"
#include "importexport/midi/internal/midiimport/importmidi_operations.h"

#include "context/iglobalcontext.h"

#include "engraving/dom/masterscore.h"

namespace Ms {

class MidiImportPanelNew : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QString currentCharset READ currentCharset WRITE setCurrentCharset NOTIFY currentCharsetChanged)
    Q_PROPERTY(QStringList charsetList READ charsetList CONSTANT)
    QML_ELEMENT

public:
    explicit MidiImportPanelNew(QObject* parent = nullptr);
    ~MidiImportPanelNew();

    QString userName() const;
    void setUserName(const QString& userName);

    bool isVisible() const;
    void setVisible(bool visible);

    QString currentCharset() const;
    void setCurrentCharset(const QString& charset);

    QStringList charsetList() const;

    Q_INVOKABLE void apply();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void moveTrackUp(int index);
    Q_INVOKABLE void moveTrackDown(int index);
    Q_INVOKABLE void fillCharsetList();
    Q_INVOKABLE void setModel(mu::iex::midi::TracksModel* model);

    void setMidiFile(const QString& fileName);
    

signals:
    void userNameChanged();
    void visibleChanged();
    void currentCharsetChanged();
    void midiFileChanged();            
    void modelChanged();               
    void charsetListChanged();

private:
    bool canImportMidi() const;
    void setReorderedIndexes();
    void saveTableViewState();
    //QTableView* m_tracksView = nullptr;

private:
    QString m_userName;
    bool m_visible = false;
    QString m_currentCharset = "UTF-8";
    QStringList m_charsetList; 

    QString m_midiFile;
    mu::iex::midi::TracksModel* m_model = nullptr;

    bool m_importInProgress = false;

    //muse::Inject<mu::context::IGlobalContext> m_globalContext { this };
};

} // namespace Ms
