#include <QApplication>

#include "CanvasWidget.h"
#include "globals.h"
#include "PageManager.h"

PageManager PAGE_MANAGER;
Settings SETTINGS;

int main(int argc, char *argv[]) {
    qmlRegisterType<CanvasWidget>("CustomWidgets", 1, 0, "CanvasWidget");
    qRegisterMetaType<GraphParameters>("GraphParameters");

    QApplication application(argc, argv);

    PAGE_MANAGER.Initialize();

    return application.exec();
}
