#ifndef SUNII_H
#define SUNII_H

#include <QObject>
#include "packarchive.h"
#include "lcdscreen.h" // LCD adapter from C to QML

class Osb : public QObject
{
    Q_OBJECT
public:
    Osb(LcdScreen &lcd);

    Q_INVOKABLE void openFile();

private:
    PackArchive pack;
    LcdScreen &mLcd;

};

#endif // SUNII_H
