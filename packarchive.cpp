#include "packarchive.h"
#include "ni_parser.h"

#include <iostream>

#include <QDebug>
#include "Util.h"

PackArchive::PackArchive()
{
    QByteArray ba;
}

void PackArchive::Load(const std::string &filePath)
{
    mZip.Close();
    mCurrentNode = 0;

    std::string fileName = Util::GetFileName(filePath);
    std::string ext = Util::GetFileExtension(fileName);
    Util::EraseString(fileName, "." + ext); // on retire l'extension du pack
    mPackName = Util::ToUpper(fileName);

    std::cout << mPackName << std::endl;

    if (mZip.Open(filePath, true))
    {
        std::cout << mZip.NumberOfFiles() << std::endl;
        std::vector<std::string> lf = mZip.ListFiles();

        for (const auto &f : lf)
        {
            std::cout << f << std::endl;
        }

        if (ParseNIFile(mPackName))
        {
            std::cout << "Parse NI file success\r\n"  << std::endl;
            ni_dump();

            ni_get_node_info(mCurrentNode, &node);
        }
        else
        {
            std::cout << "Parse NI file error\r\n"  << std::endl;
        }
    }
}

std::string PackArchive::OpenImage(const std::string &fileName)
{
    std::string f;
    mZip.GetFile(fileName, f);
    return f;
}

std::string PackArchive::CurrentImage()
{
    //"C8B39950DE174EAA8E852A07FC468267/rf/000/05FB5530"
    std::string imagePath = mPackName + "/rf/" + std::string(node.ri_file);
    Util::ReplaceCharacter(imagePath, "\\", "/");

    std::cout << "Loading " + imagePath << std::endl;
    return OpenImage(imagePath);
}

QByteArray PackArchive::CurrentSound()
{
    //"C8B39950DE174EAA8E852A07FC468267/sf/000/05FB5530"
    std::string soundPath = mPackName + "/sf/" + std::string(node.si_file);
    std::cout << "Loading " + soundPath << std::endl;

    Util::ReplaceCharacter(soundPath, "\\", "/");

    std::string f;
    mZip.GetFile(soundPath, f);

    ni_decode_block512(reinterpret_cast<uint8_t *>(f.data()));
    QByteArray data(f.data(), f.size());
    return data;
}

bool PackArchive::ParseNIFile(const std::string &root)
{
    bool success = true;
    std::string f;
    if (mZip.GetFile(root + "/li", f))
    {
        ni_set_li_block(reinterpret_cast<const uint8_t *>(f.data()), f.size());
    }
    else
    {
        success = false;
        std::cout << "[PACK_ARCHIVE] Cannot find LI file" << std::endl;
    }

    if (mZip.GetFile(root + "/ri", f))
    {
        ni_set_ri_block(reinterpret_cast<const uint8_t *>(f.data()), f.size());
    }
    else
    {
        success = false;
        std::cout << "[PACK_ARCHIVE] Cannot find RI file" << std::endl;
    }

    if (mZip.GetFile(root + "/si", f))
    {
        ni_set_si_block(reinterpret_cast<const uint8_t *>(f.data()), f.size());
    }
    else
    {
        success = false;
        std::cout << "[PACK_ARCHIVE] Cannot find SI file" << std::endl;
    }

    if (mZip.GetFile(root + "/ni", f))
    {
        success = success & ni_parser(reinterpret_cast<const uint8_t *>(f.data()));
    }
    else
    {
        std::cout << "[PACK_ARCHIVE] Cannot find NI file" << std::endl;
    }
    return success;
}
