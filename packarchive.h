#ifndef PACKARCHIVE_H
#define PACKARCHIVE_H

#include <string>
#include "Zip.h"
#include <QByteArray>
#include "ni_parser.h"

class PackArchive
{
public:
    PackArchive();

    void Load(const std::string &filePath);
    std::string OpenImage(const std::string &fileName);
    std::string CurrentImage();
    QByteArray CurrentSound();

private:
    Zip mZip;
    std::string mPackName;
    uint32_t mCurrentNode;
    node_info_t node;
    bool ParseNIFile(const std::string &root);
};

#endif // PACKARCHIVE_H
