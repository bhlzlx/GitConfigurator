#include "configure_window.h"
#include <iostream>

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
    //
    GitConfParser parser;
    for(auto ch : conf_sample) {
        auto rst = parser.parse(ch);
        if(rst.code == ParseCode::Token) {
            std::cout<<rst.text<<std::endl;
        }
    }
}

ConfigureWindow::~ConfigureWindow() {
}