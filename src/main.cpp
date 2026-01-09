#include <spdlog/spdlog.h>
#include <QApplication>
#include "appmainwindow.h"

import Logger;


int main(int argc, char* argv[]) {
    Logger::init(true, true, spdlog::level::debug, "logs/{:%Y%m%d}.log", 50, 10);

    QApplication app(argc, argv);

    AppMainWindow w;
    w.show();

    return app.exec();
}
