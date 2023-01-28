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
    QString homePath = QDir::homePath();
    QFile userConfFile(homePath + "\\.gitconfig");
    bool openRst = userConfFile.open(QIODevice::OpenModeFlag::ReadOnly);
    if(openRst) {
        GitConfParser parser;
        QByteArray data = userConfFile.readAll();
        auto catagoryIter = confData_.begin();
        for(auto ch : data) {
            auto parseRst = parser.parse(ch);
            QString currentKey;
            switch(parseRst.code) {
                case ParseCode::Token: {
                    switch(parseRst.state) {
                        case ParseState::Catagory: {
                            catagoryIter = confData_.insert(QString(parseRst.text.c_str()), {});
                            break;
                        }
                        case ParseState::Key: {
                            currentKey = QString(parseRst.text.c_str());
                            break;
                        }
                        case ParseState::Value: {
                            catagoryIter->insert(currentKey, parseRst.text.c_str());
                        }
                        case ParseState::Comment: {
                            size_t i = 0;
                            while(true) {
                                QString key = QString("#") + QVariant(i).toString();
                                auto iter = catagoryIter->find(key);
                                if(iter == catagoryIter->end()) {
                                    catagoryIter->insert(key, parseRst.text.c_str());
                                    break;
                                }
                                ++i;
                            }
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
    }

    // for(auto ch : conf_sample) {
    //     auto rst = parser.parse(ch);
    //     if(rst.code == ParseCode::Token) {
    //         std::cout<<rst.text<<std::endl;
    //     }
    // }
}

ConfigureWindow::~ConfigureWindow() {
}