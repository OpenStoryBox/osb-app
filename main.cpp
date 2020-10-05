#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine> // pour addImportPath()
#include <QString>
#include <QtGlobal> // qInstallMessageHandler()
#include <QtQml> // pour qmlRegisterType()
#include <QIcon>

#include "packarchive.h"
#include "lcdscreen.h" // LCD adapter from C to QML
#include "osb.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    LcdScreen lcd;
    Osb osb(lcd);

    QQmlApplicationEngine engine;

    QQmlContext * ctxt = engine.rootContext();
    ctxt->setContextProperty("lcd", &lcd); // on rend cet objet accessible en QML
    ctxt->setContextProperty("osb", &osb);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
