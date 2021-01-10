#include "lcdscreen.h"

#include "packarchive.h"

#include <QPainter>
#include "Util.h"
#include "Base64.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>

LcdScreen::LcdScreen(QObject *parent) : QObject(parent)
{

}

QString LcdScreen::getImage()
{
    // Always save the image to the filesystem
    mCurrentPix.save("output.png");

    std::ifstream input("output.png", std::ios::in | std::ios::binary );

    std::string buffer;

    input.seekg(0, std::ios::end);
    buffer.reserve(input.tellg());
    input.seekg(0, std::ios::beg);

    // Copy all data into a buffer
    // note the extra parentheses around the first argument to the string constructor.
    // These are essential. They prevent the problem known as the "most vexing parse",
    // which in this case won't actually give you a compile error like it usually does,
    // but will give you interesting (read: wrong) results.
    buffer.assign((std::istreambuf_iterator<char>(input)),
                  (std::istreambuf_iterator<char>()));
    input.close();

    // Build a simple JSON object as a string so that the script can manipulate it
    std::stringstream ss;
    std::string base64_str = Base64::Encode(buffer);

    QString image("data:image/bmp;base64,");

    image.append(QString::fromLatin1(base64_str.data()));
    return image;
}

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

void btea(uint32_t *v, int n, uint32_t const key[4]) {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n > 1) {          /* Coding Part */
        rounds = 6 + 52/n;
        sum = 0;
        z = v[n-1];
        do {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p=0; p < (n-1); p++) {
                y = v[p+1];
                z = v[p] += MX;
            }
            y = v[0];
            z = v[n-1] += MX;
        } while (--rounds);
    } else if (n < -1) {  /* Decoding Part */
        n = -n;
        //rounds = 6 + 52/n;
        rounds = 0x34 / (n >>1) +1;
        sum = rounds*DELTA;
        y = v[0];
        do {
            e = (sum >> 2) & 3;
            for (p=n-1; p>0; p--) {
                z = v[p-1];
                y = v[p] -= MX;
            }
            z = v[n-1];
            y = v[0] -= MX;
            sum -= DELTA;
        } while (--rounds);
    }
}

const uint32_t key[4] = {0x91bd7a0a, 0xa75440a9, 0xbbd49d6c, 0xe0dcc0e3};

void LcdScreen::SetImage(const std::string &bytes)
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



    {
        memcpy(bmpImage + 512, compressed, compressedSize);
        std::ofstream outfile ("final.bmp", std::ofstream::binary);
        outfile.write (reinterpret_cast<char *>(bmpImage), 512 + 320*240);
        outfile.close();
    }

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
