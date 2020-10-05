#ifndef LCDSCREEN_H
#define LCDSCREEN_H

#include <QObject>
#include <QImage>

class LcdScreen : public QObject
{
    Q_OBJECT
public:
    explicit LcdScreen(QObject *parent = nullptr);

    void SetImage(const std::string &bytes);

    Q_INVOKABLE QString getImage();

signals:
    void sigShowImage();

private:
    QImage mCurrentPix;
};

#endif // LCDSCREEN_H
