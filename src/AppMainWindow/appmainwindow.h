//
// Created by DED on 20.12.2025.
//

#ifndef COOLAPPNAME_APPMAINWINDOW_H
#define COOLAPPNAME_APPMAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE

namespace Ui
{
    class AppMainWindow;
}

QT_END_NAMESPACE

class AppMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppMainWindow(QWidget* parent = nullptr);
    ~AppMainWindow() override;

private:
    Ui::AppMainWindow* ui;
};


#endif //COOLAPPNAME_APPMAINWINDOW_H