#ifndef PACKARCHIVE_H
#define PACKARCHIVE_H

#include <string>
#include "Zip.h"

class PackArchive
{
public:
    PackArchive();

    void Load(const std::string &filePath);
    std::string OpenImage(const std::string &fileName);

private:
    Zip mZip;
};

#endif // PACKARCHIVE_H
