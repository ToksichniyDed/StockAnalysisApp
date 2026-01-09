//
// Created by DED on 20.12.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AppMainWindow.h" resolved

#include "appmainwindow.h"
#include "ui_AppMainWindow.h"


AppMainWindow::AppMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::AppMainWindow) {
    ui->setupUi(this);
}

AppMainWindow::~AppMainWindow() {
    delete ui;
}
