#include "StoryTellerModel.h"

#include "packarchive.h"

#include <QPainter>
#include <QDir>
#include "Util.h"
#include "Base64Util.h"
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

void StoryTellerModel::ScanPacks()
{
    QDir dir(mPacksPath);
    QStringList filters;
    filters << "*.pk";
    dir.setNameFilters(filters);
    if (dir.exists())
    {
        // scan all pack files
        mListOfPacks = dir.entryList();

        for (const auto & p : mListOfPacks)
        {
            std::cout << "Found pack: " << p.toStdString() << std::endl;
        }

        if (mListOfPacks.size() == 0)
        {
            SetMessage("Mauvais emplacement des packs");
        }
        else
        {
            openFile();
        }
    }
    else
    {
        SetMessage("Mauvais emplacement des packs");
    }
}

QString StoryTellerModel::getImage()
{
    std::stringstream ss;
    std::string base64_str = Base64::Encode(std::string(reinterpret_cast<char*>(bmpImage), sizeof(bmpImage)));

    QString image = "data:image/bmp;base64,";
    image.append(QString::fromLatin1(base64_str.data()));
    return image;
}

void StoryTellerModel::openFile()
{
    if (mListOfPacks.size() > 0)
    {
        QString packName = mListOfPacks[mCurrentPackIndex];
        QString fullPackPath = mPacksPath + Util::DIR_SEPARATOR + packName;
        if (pack.Load(fullPackPath.toStdString()))
        {
            SetMessage("");
            /*
         * std::vector<std::string> imgs = pack.GetImages();
            uint32_t index = 0;
            for (const auto & im : imgs)
            {
                std::string image = pack.GetImage(im);
                DecryptImage(image);

                std::string fileName = std::to_string(index) + "_" + im + std::string(".bmp");
                Util::ReplaceCharacter(fileName, "\\", "_");
                SaveImage(fileName);
                index++;
            }*/

            ShowResources();
        }
        else
        {
            SetMessage("Pack load error: " + packName);
        }
    }
}

void StoryTellerModel::okButton()
{
    pack.OkButton();
    ShowResources();
}

void StoryTellerModel::previous()
{
    if (pack.IsWheelEnabled() && !pack.IsRoot())
    {
        pack.Previous();
        ShowResources();
    }
    else
    {
        if (mCurrentPackIndex > 0)
        {
            mCurrentPackIndex--;
        }
        else
        {
            mCurrentPackIndex = mListOfPacks.size() - 1;
        }

        openFile();
    }
}

void StoryTellerModel::next()
{
    if (pack.IsWheelEnabled() && !pack.IsRoot())
    {
        pack.Next();
        ShowResources();
    }
    else
    {
        mCurrentPackIndex++;
        if (mCurrentPackIndex >= static_cast<uint32_t>(mListOfPacks.size()))
        {
            mCurrentPackIndex = 0;
        }

        openFile();
    }
}

void StoryTellerModel::initialize()
{
    ScanPacks();
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

    std::string fileName = pack.CurrentSoundName();
    Util::ReplaceCharacter(fileName, "\\", "_");

    Play(fileName, pack.CurrentSound());
}

void StoryTellerModel::saveSettings(const QString &packPath)
{
    mPacksPath = QUrl(packPath).toLocalFile();
    settings.setValue("packs/path", mPacksPath);
    ScanPacks();
}

void StoryTellerModel::ClearScreen()
{
    emit sigClearScreen();
}

void StoryTellerModel::DecryptImage(const std::string &bytes)
{
    uint32_t compressedSize = bytes.length() - 512;

    memcpy(bmpImage, bytes.data(), 512);
    ni_decode_block512(bmpImage);

    memcpy(bmpImage + 512, bytes.data() + 512, compressedSize);
}

void StoryTellerModel::SaveImage(const std::string &fileName)
{
    std::ofstream outfile (fileName, std::ofstream::binary);
    outfile.write (reinterpret_cast<char *>(bmpImage), 512 + 320*240);
    outfile.close();
}

void StoryTellerModel::SetImage(const std::string &bytes)
{
    DecryptImage(bytes);
    emit sigShowImage();
}

void StoryTellerModel::Play(const std::string &fileName, const QByteArray &ar)
{
    buffer.close();
    buffer.setData(ar);
    buffer.open(QIODevice::ReadOnly);

    player->setMedia(QUrl(fileName.c_str()), &buffer);
    player->setVolume(60);
    player->play();
}

void StoryTellerModel::SetMessage(const QString &newMessage)
{
    mCurrentMessage = newMessage;
    emit sigMessageChanged();
}

void StoryTellerModel::slotPlayerStateChanged(QMediaPlayer::State newState)
{
    if (newState == QMediaPlayer::StoppedState)
    {
        // next action!
        std::cout << "Sound ended" << std::endl;

        if (pack.AutoPlay())
        {
            // équivalent de Ok button
            okButton();
        }
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
