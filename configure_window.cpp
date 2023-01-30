#include "configure_window.h"
#include <iostream>
#include <qdir>
#include <qfile>
#include <qmenu>
#include <qcursor.h>
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
    // for(auto ch : conf_sample) {
    //     auto rst = parser.parse(ch);
    //     if(rst.code == ParseCode::Token) {
    //         std::cout<<rst.text<<std::endl;
    //     }
    // }
    loadUserConfig();
    treeModel_ = new GitconfTreeModel(rootNode_, this);
    treeView_ = ui_.content_treeview;
    treeView_->setModel(treeModel_);
    treeView_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView_, &QTreeView::customContextMenuRequested, this, &ConfigureWindow::onTreeViewMenu);
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
    }
    menu.exec(QCursor::pos());
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
}

void ConfigureWindow::onRemove(bool triggered) {
    auto current = treeView_->currentIndex();
    auto parent = treeModel_->parent(current);
    treeModel_->removeRows(current.row(), 1, parent);
    treeModel_->dataChanged(parent, parent);
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

bool ConfigureWindow::loadUserConfig() {
    QString homePath = QDir::homePath();
    QFile userConfFile(homePath + "/.gitconfig");
    bool openRst = userConfFile.open(QIODevice::OpenModeFlag::ReadOnly);
    if(openRst) {
        GitConfParser parser;
        QByteArray data = userConfFile.readAll();
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
        return true;
    } else {
        return false;
    }
}