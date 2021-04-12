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

    bool Load(const std::string &filePath);
    std::string OpenImage(const std::string &fileName);
    std::string CurrentImage();
    QByteArray CurrentSound();
    std::string CurrentSoundName();
    bool AutoPlay();
    void OkButton();
    bool HasImage();
    std::vector<std::string> GetImages();
    std::string GetImage(const std::string &fileName);
    bool IsRoot() const;
    bool IsWheelEnabled() const;
    void Next();
    void Previous();

private:
    Zip mZip;
    std::string mPackName;
    uint32_t mCurrentNodeId = 0;
    uint32_t mNodeIdForChoice = 0;
    node_info_t mNodeForChoice;
    node_info_t mCurrentNode;
    bool ParseNIFile(const std::string &root);
};

#endif // PACKARCHIVE_H
