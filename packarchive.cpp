#include "packarchive.h"
#include "ni_parser.h"

#include <iostream>

#include <QDebug>
#include "Util.h"

PackArchive::PackArchive()
{

}

std::vector<std::string> PackArchive::GetImages()
{
    std::vector<std::string> imgList;

    for (uint32_t i = 0; i < ni_get_number_of_images(); i++)
    {
        char buffer[13];
        ni_get_image(buffer, i);
        imgList.push_back(buffer);
    }

    return imgList;
}

bool PackArchive::Load(const std::string &filePath)
{
    bool success = false;
    mZip.Close();
    mCurrentNodeId = 0;

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
            success = true;
            std::cout << "Parse NI file success\r\n"  << std::endl;
            ni_dump();

            ni_get_node_info(mCurrentNodeId, &mCurrentNode);
        }
        else
        {
            std::cout << "Parse NI file error\r\n"  << std::endl;
        }
    }
    return success;
}

std::string PackArchive::OpenImage(const std::string &fileName)
{
    std::string f;
    mZip.GetFile(fileName, f);
    return f;
}

std::string PackArchive::GetImage(const std::string &fileName)
{
    //"C8B39950DE174EAA8E852A07FC468267/rf/000/05FB5530"
    std::string imagePath = mPackName + "/rf/" + fileName;
    Util::ReplaceCharacter(imagePath, "\\", "/");

    std::cout << "Loading " + imagePath << std::endl;
    return OpenImage(imagePath);
}

std::string PackArchive::CurrentImage()
{
    return GetImage(std::string(mCurrentNode.ri_file));
}

QByteArray PackArchive::CurrentSound()
{
    //"C8B39950DE174EAA8E852A07FC468267/sf/000/05FB5530"
    std::string soundPath = mPackName + "/sf/" + std::string(mCurrentNode.si_file);
    Util::ReplaceCharacter(soundPath, "\\", "/");

    std::cout << "Loading " + soundPath << std::endl;

    std::string f;
    if (mZip.GetFile(soundPath, f))
    {
        ni_decode_block512(reinterpret_cast<uint8_t *>(f.data()));
        QByteArray data(f.data(), f.size());
        return data;
    }
    else
    {
        std::cout << "Cannot load file from ZIP" << std::endl;
    }
    return QByteArray();
}

std::string PackArchive::CurrentSoundName()
{
    return std::string(mCurrentNode.si_file);
}

bool PackArchive::AutoPlay()
{
    return mCurrentNode.current->auto_play;
}

bool PackArchive::IsRoot() const
{
    return mCurrentNodeId == 0;
}

bool PackArchive::IsWheelEnabled() const
{
    return mCurrentNode.current->wheel;
}

void PackArchive::Next()
{
    // L'index de circulation dans le tableau des transitions commence à 1 (pas à zéro ...)
    uint32_t index = 1;
    if (mCurrentNode.current->ok_transition_selected_option_index < mNodeForChoice.current->ok_transition_number_of_options)
    {
        index = mCurrentNode.current->ok_transition_selected_option_index + 1;
    }
    // sinon on revient à l'index 0 (début du tableau des transitions)

    mCurrentNodeId = ni_get_node_index_in_li(mNodeForChoice.current->ok_transition_action_node_index_in_li, index - 1);
    ni_get_node_info(mCurrentNodeId, &mCurrentNode);
}

void PackArchive::Previous()
{
    // L'index de circulation dans le tableau des transitions commence à 1 (pas à zéro ...)
    uint32_t index = 1;
    if (mCurrentNode.current->ok_transition_selected_option_index > 1)
    {
        index = mCurrentNode.current->ok_transition_selected_option_index - 1;
    }
    else
    {
        index = mNodeForChoice.current->ok_transition_number_of_options;
    }

    mCurrentNodeId = ni_get_node_index_in_li(mNodeForChoice.current->ok_transition_action_node_index_in_li, index - 1);
    ni_get_node_info(mCurrentNodeId, &mCurrentNode);
}

void PackArchive::OkButton()
{
    if (mCurrentNode.current->home_transition_number_of_options > 0)
    {
        // On doit faire un choix!
        // On sauvegarde ce noeud car il va servir pour naviguer dans les choix
        mNodeIdForChoice = mCurrentNodeId;
        ni_get_node_info(mNodeIdForChoice, &mNodeForChoice);
    }
    mCurrentNodeId = ni_get_node_index_in_li(mCurrentNode.current->ok_transition_action_node_index_in_li, mCurrentNode.current->ok_transition_selected_option_index);
    ni_get_node_info(mCurrentNodeId, &mCurrentNode);
}

bool PackArchive::HasImage()
{
    return std::string(mCurrentNode.ri_file).size() > 1;
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
