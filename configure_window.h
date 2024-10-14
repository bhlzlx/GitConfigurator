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
    Q_SLOT void onAddCatagory(bool triggered);
    Q_SLOT void onAddValue(bool triggered);
    Q_SLOT void onRemove(bool triggered);
    Q_SLOT void onComment(bool triggered);
    Q_SLOT void onUncomment(bool triggered);
    Q_SLOT void saveConfig();
    Q_SLOT void onOpenGitProject();
    Q_SLOT void onLoadDefault();

public:
    ConfigureWindow(QWidget* parent = nullptr);
    ~ConfigureWindow();

    bool loadConfig(QString const& filepath);
};