#include "configure_window.h"
#include <iostream>
#include <qdir>
#include <qfile>
#include <qmenu>
#include <qcursor.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include "gitconf_parser.h"
#include "gitconf_tree_model.h"

char const conf_sample[] = R"([user]
	name = kusugawa
	email = bhlzlx@sina.com
[core]
	autocrlf = true
[https]
    proxy = http://127.0.0.1:58591
[http]
    proxy = http://127.0.0.1:58591
[socks5]
;proxy = http://127.0.0.1:58591
)";

char const* catagories[] = {
    "user",
    "http",
    "https",
    "socks"
};

ConfigureWindow::ConfigureWindow(QWidget* parent) {
    ui_.setupUi(this);
    rootNode_ = new node(node_type::root,"root");
    treeModel_ = new GitconfTreeModel(rootNode_, this);
    treeView_ = ui_.content_treeview;
    treeView_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView_, &QTreeView::customContextMenuRequested, this, &ConfigureWindow::onTreeViewMenu);
    connect(ui_.saveButton, &QPushButton::clicked, this, &ConfigureWindow::saveConfig);
    connect(ui_.selectButton, &QPushButton::clicked, this, &ConfigureWindow::onOpenGitProject);
    connect(ui_.loadDefaultButton, &QPushButton::clicked, this, &ConfigureWindow::onLoadDefault);
    //
    QString homePath = QDir::homePath();
    loadConfig(homePath + "/.gitconfig");
}


void ConfigureWindow::onLoadDefault() {
    QString homePath = QDir::homePath();
    loadConfig(homePath + "/.gitconfig");
}

void ConfigureWindow::onTreeViewMenu(QPoint const& pt) {
    auto index = treeView_->indexAt(pt);
    QMenu menu;
    if(!index.isValid()) {
        auto addMenu = menu.addMenu(u8"add catagory");
        for(auto i = 0; i<_countof(catagories); ++i) {
            auto action = addMenu->addAction(catagories[i], this, SLOT(onAddCatagory(bool)));
        }
    } else if(index.isValid()) {
        auto ptr = (node*)index.internalPointer();
        if(ptr->type == node_type::catagory) {
            // add item
            menu.addAction("add value", this, SLOT(onAddValue(bool)));
        }
        // delete
        menu.addAction("remove", this, SLOT(onRemove(bool)));
        if(ptr->type == node_type::keyvalue && index.column() == 1) {
            menu.addAction("comment", this, SLOT(onComment(bool)));
            menu.addAction("uncomment", this, SLOT(onUncomment(bool)));
        }
    }
    menu.exec(QCursor::pos());
}

void ConfigureWindow::onOpenGitProject() {
    auto path = QFileDialog::getExistingDirectory(this, QString("open a git repo"));
    path += "/.git/config";
    if(!loadConfig(path)) {
        QMessageBox mb(this);
        mb.setText("Invalid git repo");
        mb.exec();
        return ;
    }
}

void ConfigureWindow::onAddValue(bool triggered) {
    auto current = treeView_->currentIndex();
    treeModel_->insertRows(0, 1, current);
    auto item = treeModel_->index(0, 0, current);
    auto ptr = (node*)item.internalPointer();
    ptr->type = node_type::keyvalue;
    ptr->caption = "#";
    ptr->val = "#";
    treeModel_->dataChanged(item, item);
    treeView_->expand(current);
    treeView_->edit(item);
}

void ConfigureWindow::onRemove(bool triggered) {
    auto current = treeView_->currentIndex();
    auto parent = treeModel_->parent(current);
    treeModel_->removeRows(current.row(), 1, parent);
    treeModel_->dataChanged(parent, parent);
}

void ConfigureWindow::onComment(bool triggered) {
    auto current = treeView_->currentIndex();
    auto ptr = (node*)current.internalPointer();
    if(ptr->val.size() && ptr->val[0] != ';') {
        ptr->val = ";" + ptr->val;
    }
    treeModel_->dataChanged(current, current);
}

void ConfigureWindow::onUncomment(bool triggered) {
    auto current = treeView_->currentIndex();
    auto ptr = (node*)current.internalPointer();
    if(ptr->val.size() && ptr->val[0] == ';') {
        ptr->val = std::string(ptr->val.begin() + 1, ptr->val.end());
    }
    treeModel_->dataChanged(current, current);
}

void ConfigureWindow::onAddCatagory(bool triggered) {
    auto action = (QAction*)sender();
    auto pos = rootNode_->children.size();
    treeModel_->insertRows(pos, 1, QModelIndex());
    auto item = treeModel_->index(pos, 0, QModelIndex());
    auto ptr = (node*)item.internalPointer();
    ptr->type = node_type::catagory;
    ptr->caption = action->text().toStdString();
    treeModel_->dataChanged(item, item);
}

ConfigureWindow::~ConfigureWindow() {
}

void ConfigureWindow::saveConfig() {
    auto bytes = treeModel_->serialize();
    QFile userConfFile(ui_.pathLabel->text());
    bool openRst = userConfFile.open(QIODevice::OpenModeFlag::ReadWrite|QIODevice::OpenModeFlag::Truncate);
    userConfFile.write(bytes);
    userConfFile.close();
}

bool ConfigureWindow::loadConfig(QString const& filepath) {
    QFile file(filepath);
    bool openRst = file.open(QIODevice::OpenModeFlag::ReadOnly);
    if(openRst) {
        treeView_->setModel(nullptr);
        rootNode_->clear();
        ui_.pathLabel->setText(filepath);
        GitConfParser parser;
        QByteArray data = file.readAll();
        node* catagory = nullptr;
        std::string currentKey;
        for(auto ch : data) {
            auto parseRst = parser.parse(ch);
            switch(parseRst.code) {
                case ParseCode::Token: {
                    switch(parseRst.state) {
                        case ParseState::Catagory: {
                            catagory = new node(node_type::catagory, parseRst.text);
                            rootNode_->addChild(catagory);
                            break;
                        }
                        case ParseState::Key: {
                            currentKey = parseRst.text;
                            break;
                        }
                        case ParseState::Value: {
                            auto valNode = new node(node_type::keyvalue, currentKey, parseRst.text);
                            catagory->addChild(valNode);
                            break;
                        }
                        case ParseState::Comment: {
                            auto valNode = new node(node_type::comment, "", parseRst.text);
                            catagory->addChild(valNode);
                            break;
                        }
                    }
                }
                case ParseCode::SyntaxError:
                case ParseCode::OK:
                default: {
                    break;
                }
            }
            if(parseRst.code == ParseCode::SyntaxError) {
                break;
            }
        }
        treeView_->setModel(treeModel_);
        treeView_->expandAll();
        return true;
    } else {
        return false;
    }
}