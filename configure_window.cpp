#include "configure_window.h"
#include <iostream>
#include <qdir>
#include <qfile>

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

ConfigureWindow::ConfigureWindow(QWidget* parent) {
    ui_.setupUi(this);
    // for(auto ch : conf_sample) {
    //     auto rst = parser.parse(ch);
    //     if(rst.code == ParseCode::Token) {
    //         std::cout<<rst.text<<std::endl;
    //     }
    // }
    loadUserConfig();
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
        size_t catagory = 0;
        std::string currentKey;
        for(auto ch : data) {
            auto parseRst = parser.parse(ch);
            switch(parseRst.code) {
                case ParseCode::Token: {
                    switch(parseRst.state) {
                        case ParseState::Catagory: {
                            auto cataNode = new catagory_node();
                            cataNode->name = parseRst.text;
                            confData_.push_back(cataNode);
                            catagory = confData_.size() - 1;
                            break;
                        }
                        case ParseState::Key: {
                            currentKey = parseRst.text;
                            break;
                        }
                        case ParseState::Value: {
                            auto valNode = new value_node();
                            valNode->parent = confData_[catagory];
                            valNode->key = currentKey;
                            valNode->value = parseRst.text;
                            confData_[catagory]->children.push_back(valNode);
                            break;
                        }
                        case ParseState::Comment: {
                            auto valNode = new value_node();
                            valNode->parent = confData_[catagory];
                            valNode->key = "#";
                            valNode->value = parseRst.text;
                            confData_[catagory]->children.push_back(valNode);
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