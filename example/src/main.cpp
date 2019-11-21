#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <qqml.h>
#include <QDebug>
#include <QTimer>
#include "qpointcloud.h"
#include "qpointcloudgeometry.h"
#include "qpointfield.h"
#include "qpointcloudreader.h"
#include "streamdata.h"
#include <cmath>

static Streamdata* g_streamdata = nullptr;
QObject* streamdata(QQmlEngine *, QJSEngine *)
{
    return g_streamdata;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<QPointCloudReader>("pcl", 1, 0, "PointcloudReader");
    qmlRegisterType<QPointcloud>("pcl", 1, 0, "Pointcloud");
    qmlRegisterType<QPointcloudGeometry>("pcl", 1, 0, "PointcloudGeometry");
    //qmlRegisterUncreatableType<QPointfield>("pcl", 1, 0, "Pointfield", "Can not yet be created in qml, use PointcloudReader.");
    qmlRegisterType<QPointfield>("pcl", 1, 0, "Pointfield");
    //qmlRegisterType<Streamdata>("pcl", 1, 0, "Streamdata");

    g_streamdata = new Streamdata{&app};
    g_streamdata->setCapacity(4*3*10000);
    auto dataGen = new QTimer{&app};
    dataGen->setInterval(10);
    dataGen->setSingleShot(false);
    float angle = 0.f;
    float dist = 3.f;
    QObject::connect(dataGen, &QTimer::timeout, g_streamdata, [&angle, &dist]() {
        float arr[15];
        for(int i=0; (i+2)<15 ; i+=3) {
            arr[i+0] = sin(angle) * dist;
            arr[i+1] = cos(angle) * dist;
            arr[i+2] = sin(angle*5.f) * 2;
            angle += 0.01f;
            dist = 5.f+sin(angle*0.01f)*2.f;
        }
        g_streamdata->consume(reinterpret_cast<const char*>(&arr[0]), 15*sizeof(float));
    });

    qmlRegisterSingletonType<Streamdata>("pcl", 1, 0, "Streamdata", &streamdata);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));

    dataGen->start();
    int result = app.exec();
    dataGen->stop();
    return result;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR argv, int argc)
{
//    int argc=1;
//    char *argv[] = {"temp"};
    return main(argc, &argv);
}
#endif
