#include "soundplayer.h"

SoundPlayer::SoundPlayer()
{
    player = new QMediaPlayer;
}

void SoundPlayer::Play(const QByteArray &ar)
{
    buffer.setData(ar);
    buffer.open(QIODevice::ReadOnly);

    player->setMedia(QUrl("/Users/me/Music/coolsong.mp3"), &buffer);
    player->setVolume(50);
    player->play();
}
