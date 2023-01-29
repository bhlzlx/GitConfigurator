#pragma once
#include <qmainwindow>
#include <qwidget>
#include <string>
#include <qstring>
#include <qvariant.h>

#include <vector>
#include <string>

#include "ui_configure_window.h"


class node;

class ConfigureWindow :public QMainWindow {
    Q_OBJECT
private:
    Ui::ConfWindow                          ui_;
    node*                                   rootNode_;
public:
    ConfigureWindow(QWidget* parent = nullptr);
    ~ConfigureWindow();

    bool loadUserConfig();
};