#include <QApplication>
#include "appmainwindow.h"

#include "Logger/Logger.h"
#include <NetworkTools/BoostBeast/BoostBeastHttpClientRegistration.h>

void prepareApp();

int main(int argc, char* argv[]) {
    Logger::init(true, true, spdlog::level::debug, "logs/{:%Y%m%d}.log", 50, 10);

    QApplication app(argc, argv);

    AppMainWindow w;
    w.show();

    return app.exec();
}

void prepareApp() {
    registerBoostBeastHttpClient();
}