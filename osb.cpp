#include "osb.h"

Osb::Osb(LcdScreen &lcd)
    : QObject(nullptr)
    , mLcd(lcd)

{

}

void Osb::openFile()
{
    pack.Load("/home/anthony/.local/share/Luniitheque/packs/c8b39950de174eaa8e852a07fc468267.pk");
    mLcd.SetImage(pack.OpenImage("C8B39950DE174EAA8E852A07FC468267/rf/000/05FB5530"));
}
