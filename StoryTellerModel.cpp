#include "StoryTellerModel.h"

#include "packarchive.h"

#include <QPainter>
#include "Util.h"
#include "Base64.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "ni_parser.h"

StoryTellerModel::StoryTellerModel(QObject *parent) : QObject(parent)
{
    mPacksPath = settings.value("packs/path").toString();

    player = new QMediaPlayer;
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(slotPlayerStateChanged(QMediaPlayer::State)));
}

QString StoryTellerModel::getImage()
{
    return mImage;
}

void StoryTellerModel::openFile()
{
    pack.Load("/home/anthony/lunii_packs/c8b39950de174eaa8e852a07fc468267.pk");

    ShowResources();
//    pack.Load("/home/anthony/lunii_packs/3ade540306254fffa22b9025ac3678d9.pk");
    // mLcd.SetImage(pack.OpenImage("C8B39950DE174EAA8E852A07FC468267/rf/000/05FB5530"));
}

void StoryTellerModel::okButton()
{
    pack.OkButton();
    ShowResources();
}

void StoryTellerModel::ShowResources()
{
    if (pack.HasImage())
    {
        SetImage(pack.CurrentImage());
    }
    else
    {
        ClearScreen();
    }
    Play(pack.CurrentSoundName(), pack.CurrentSound());
}

void StoryTellerModel::saveSettings(const QString &packPath)
{
    settings.setValue("packs/path", packPath);
}

void StoryTellerModel::ClearScreen()
{
    emit sigClearScreen();
}

void StoryTellerModel::SetImage(const std::string &bytes)
{
    uint8_t bmpImage[512 + 320*240];
    uint32_t compressedSize = bytes.length() - 512;

    memcpy(bmpImage, bytes.data(), 512);
    ni_decode_block512(bmpImage);

    memcpy(bmpImage + 512, bytes.data() + 512, compressedSize);

//    std::ofstream outfile ("final.bmp", std::ofstream::binary);
//    outfile.write (reinterpret_cast<char *>(bmpImage), 512 + 320*240);
//    outfile.close();

    mCurrentPix = QImage(bmpImage, 320, 240, QImage::Format_RGB16);

    std::stringstream ss;
    std::string base64_str = Base64::Encode(std::string(reinterpret_cast<char*>(bmpImage), sizeof(bmpImage)));

    mImage = "data:image/bmp;base64,";

    mImage.append(QString::fromLatin1(base64_str.data()));

    emit sigShowImage();
}

void StoryTellerModel::Play(const std::string &fileName, const QByteArray &ar)
{
    buffer.close();
    buffer.setData(ar);
    buffer.open(QIODevice::ReadOnly);

    player->setMedia(QUrl(fileName.c_str()), &buffer);
    player->setVolume(50);
    player->play();
}

void StoryTellerModel::slotPlayerStateChanged(QMediaPlayer::State newState)
{
    if (newState == QMediaPlayer::StoppedState)
    {
        // next action!
        std::cout << "Sound ended" << std::endl;
    }
}


/*
// Code de décompression
void StoryTellerModel::SetImage(const std::string &bytes)
{
    uint32_t width = 320;
    uint32_t height = 240;

    if (bytes.size() <= 512)
    {
        return;
    }

    uint8_t decompressed[320 * 240];

    memset(decompressed, 0, 320*240);
    uint32_t compressedSize = bytes.length() - 512;

    uint8_t compressed[compressedSize];

    uint8_t bmpImage[512 + 320*240];

    memcpy(bmpImage, bytes.data(), 512);
    btea((uint32_t*) bmpImage, -128, key);

    memcpy(compressed, bytes.data() + 512, compressedSize);

//    std::cout << Util::HexDump("Image", compressed, compressedSize) << std::endl;

    QImage img(width, height, QImage::Format_RGB32);
    img.fill(QColor("#38A0A2"));
    QPainter painter;

    painter.begin(&img);

    uint32_t pixel = 0; // specify the pixel offset
    uint32_t i = 0;
    do
    {
        uint8_t rleCmd = compressed[i];
        if (rleCmd > 0)
        {
            uint8_t val = compressed[i + 1];
            // repeat number of pixels
            for (uint32_t j = 0; j < rleCmd; j++)
            {
                uint32_t byte_index = pixel / 2;
                if ((pixel & 1) == 0)
                {
                    decompressed[byte_index] = (val & 0xF0);
                    pixel++;
                }
                else
                {
                    decompressed[byte_index] |= (val & 0x0F);
                    pixel++;
                }
            }
            i += 2; // jump pair instruction
        }
        else
        {
            uint8_t second = compressed[i + 1];
            if (second == 0)
            {
                if (pixel % width)
                {
                    // end of line
                    uint32_t lines = pixel / width;
                    uint32_t remaining = width - (pixel - (lines * width));

                    pixel += remaining;
                }
                i += 2;
            }
            else if (second == 1)
            {
                std::cout << "End of bitmap" << std::endl;
                i += 2;
                goto end;
            }
            else if (second == 2)
            {
                // delta N pixels and M lines
                pixel += compressed[i + 2] + compressed[i + 3] * width;
                i += 4;
            }
            else
            {
                // absolute mode
                uint8_t *ptr = &compressed[i + 2];
                // repeat number of pixels
                for (uint32_t j = 0; j < second; j++)
                {
                    uint32_t byte_index = pixel / 2;
                    if ((pixel & 1) == 0)
                    {
                        decompressed[byte_index] = (*ptr & 0xF0);
                        pixel++;
                    }
                    else
                    {
                        decompressed[byte_index] |= (*ptr & 0x0F);
                        ptr++;
                        pixel++;
                    }
                }
                i += 2 + (second / 2);

                // padded in word boundary, jump if necessary
                if ((second / 2) % 2)
                {
                    i++;
                }
            }
        }

        if (pixel >= (width * height))
        {
            std::cout << "Error!" << std::endl;
        }
    }
    while( i < compressedSize);


end:


    const QString palette[16] = {
        "#F0F0F0", "#DCDCDC", "#D3D3D3", "#C8C8C8",
        "#BEBEBE", "#B0B0B0", "#A8A8A8", "#989898",
        "#808080", "#707070", "#606060", "#505050",
        "#404040", "#303030", "#181818", "#000000"
    };

    uint32_t x = 0, y = 0;
    for (uint32_t i = 0; i < pixel; i++)
    {
        uint32_t byte_index = i / 2;
        uint8_t val = decompressed[byte_index];

        if ((pixel & 1) == 0)
        {
            val = (val & 0xF0) >> 4;
        }
        else
        {
            val = val & 0x0F;
        }

        if (val > 0)
        {
            painter.setPen(QColor(palette[val]));
            painter.drawPoint(x, y);
        }
        x++;
        if (x >= width)
        {
            x = 0;
            y++;
        }
    }


    painter.end();

//    std::ofstream outfile ("new.txt", std::ofstream::binary);
//    outfile.write (reinterpret_cast<const char *>(decompressed), pixel / 2 );
//    outfile.close();

    mCurrentPix = img.mirrored();

    emit sigShowImage();
}
*/
