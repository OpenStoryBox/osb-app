#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QBuffer>
#include <QByteArray>
#include <QtMultimedia/QMediaPlayer>

class SoundPlayer
{
public:
    SoundPlayer();

    void Play(const QByteArray &ar);

private:
    QMediaPlayer *player;
    QBuffer buffer;
};

#endif // SOUNDPLAYER_H
