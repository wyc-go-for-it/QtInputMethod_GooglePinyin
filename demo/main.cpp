#include "mainwindow.h"

#include "qdebug.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("tgtsml"));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
