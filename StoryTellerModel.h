#ifndef STORY_TELLER_MODEL_H
#define STORY_TELLER_MODEL_H

#include <QObject>
#include <QImage>
#include <QSettings>

#include "packarchive.h"
#include "soundplayer.h"

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

signals:
    void sigShowImage();

private:
    QImage mCurrentPix;
    QString mImage;
    QSettings settings;
    QString mPacksPath;
    PackArchive pack;
    SoundPlayer mSound;
};

#endif // STORY_TELLER_MODEL_H
