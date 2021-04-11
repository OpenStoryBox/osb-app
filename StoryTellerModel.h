#ifndef STORY_TELLER_MODEL_H
#define STORY_TELLER_MODEL_H

#include <QObject>
#include <QImage>
#include <QSettings>
#include <QBuffer>
#include <QByteArray>
#include <QtMultimedia/QMediaPlayer>

#include "packarchive.h"

class StoryTellerModel : public QObject
{
    Q_OBJECT
public:
    explicit StoryTellerModel(QObject *parent = nullptr);

    void SetImage(const std::string &bytes);

    Q_INVOKABLE QString getImage();
    Q_INVOKABLE void saveSettings(const QString &packPath);
    Q_INVOKABLE void openFile();
    Q_INVOKABLE void okButton();

    void ClearScreen();
signals:
    void sigShowImage();
    void sigClearScreen();

private:
    QImage mCurrentPix;
    QString mImage;
    QSettings settings;
    QString mPacksPath;
    PackArchive pack;
    QMediaPlayer *player;
    QBuffer buffer;

    void ShowResources();
    void Play(const std::string &fileName, const QByteArray &ar);

private slots:
    void slotPlayerStateChanged(QMediaPlayer::State newState);
};

#endif // STORY_TELLER_MODEL_H
