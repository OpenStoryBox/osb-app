#include "packarchive.h"


#include <iostream>

PackArchive::PackArchive()
{

}

void PackArchive::Load(const std::string &filePath)
{
    mZip.Close();

    if (mZip.Open(filePath, true))
    {
        std::cout << mZip.NumberOfFiles() << std::endl;
        std::vector<std::string> lf = mZip.ListFiles();

        for (auto f : lf)
        {
            std::cout << f << std::endl;
        }
    }

//    QuaZip zip(filePath);
//    zip.open(QuaZip::mdUnzip);

//    QStringList files = zip.getFileNameList();

//    for (auto f : files)
//    {
//        qDebug() << f;
//    }

//    QuaZipFile file(&zip);

//    for(bool f=zip.goToFirstFile(); f; f=zip.goToNextFile()) {
//        file.open(QIODevice::ReadOnly);
//        //same functionality as QIODevice::readData() -- data is a char*, maxSize is qint64
//        file.readData(data,maxSize);
//        //do something with the data
//        file.close();
//    }

//    zip.close();
}

std::string PackArchive::OpenImage(const std::string &fileName)
{
    std::string f;
    mZip.GetFile(fileName, f);
    return f;
}
