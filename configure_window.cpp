#include "configure_window.h"
#include <iostream>
#include <qdir>
#include <qfile>
#include "gitconf_parser.h"

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
    rootNode_ = new node(node_type::root,"root");
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
                            auto valNode = new node(node_type::comment, parseRst.text);
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