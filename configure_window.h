﻿#pragma once
#include <qmainwindow>
#include <qwidget>
#include <string>
#include <qstring>
#include <qvariant.h>

#include <vector>
#include <string>

#include "ui_configure_window.h"


class node;
class GitconfTreeModel;
class QTreeView;

class ConfigureWindow :public QMainWindow {
    Q_OBJECT
private:
    Ui::ConfWindow                          ui_;
    node*                                   rootNode_;
    GitconfTreeModel*                       treeModel_;
    QTreeView*                              treeView_;
private:
    Q_SLOT void onTreeViewMenu(QPoint const& pt);

public:
    ConfigureWindow(QWidget* parent = nullptr);
    ~ConfigureWindow();

    bool loadUserConfig();
};