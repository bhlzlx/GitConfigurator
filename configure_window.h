#pragma once
#include <qmainwindow>
#include <qwidget>
#include <string>
#include "ui_configure_window.h"

enum class ParseState {
    None,
    Catagory, //
    Items,
    Key,
    KeyEnd,
    Value,
    Comment,
};

enum class ParseCode {
    OK,
    Token,
    SyntaxError,
};

struct ParseResult {
    ParseCode code;
    ParseState state;
    std::string text;
};

class GitConfParser {
private:
    ParseState      state_;
    std::string     text_;
    bool            inCatagory_;
    ParseResult     rst_;
private:
    void checkEOL(char ch) {
        if(ch == '\r' || ch == '\n') {
            state_ = ParseState::None;
            text_.clear();
        }
    }
    bool isBlank(char ch) {
        if(ch == ' ' || ch == '\t') {
            return true;
        }
        return false;
    }
    void changeState(ParseState state) {
        rst_.code = ParseCode::Token;
        rst_.state = state_;
        rst_.text = std::move(text_);
        state_ = state;
    }
public:
    GitConfParser()
        : state_(ParseState::None)
    {}

    void reset() {
        state_ = ParseState::None;
        text_.clear();
        inCatagory_ = false;
        rst_ = {};
    }

    ParseResult parse(char ch) {
        rst_.code = ParseCode::OK;
        switch(state_) {
            case ParseState::None: {
                if(isBlank(ch)) {
                    break;
                }
                if(ch == ';') {
                    changeState(ParseState::Comment);
                } else if(ch == '[') {
                    changeState(ParseState::Catagory);
                    inCatagory_ = true;
                } else { // 这里简单处理
                    if(!inCatagory_) {
                        rst_.code = ParseCode::SyntaxError;
                    }
                    text_.push_back(ch);
                    state_ = ParseState::Key;
                }
                break;
            }
            case ParseState::Catagory: {
                if(ch != ']') {
                    text_.push_back(ch);
                } else {
                    changeState(ParseState::None);
                }
                break;
            }
            case ParseState::Key: {
                if(isBlank(ch)) {
                    changeState(ParseState::KeyEnd);
                } else if(ch == '=') {
                    changeState(ParseState::Value);
                } else {
                    text_.push_back(ch);
                }
                break;
            }
            case ParseState::KeyEnd: {
                if(ch == '=') {
                    changeState(ParseState::Value);
                }
                break;
            }
            case ParseState::Value: {
                if(isBlank(ch)) {
                } else if(ch == '\r' || ch == '\n'){
                    changeState(ParseState::None);
                } else {
                    text_.push_back(ch);
                }
                break;
            }
        }
        checkEOL(ch);
        return rst_;
    }
};

class ConfigureWindow :public QMainWindow {
    Q_OBJECT
private:
    Ui::ConfWindow      ui_;
public:
    ConfigureWindow(QWidget* parent = nullptr);
    ~ConfigureWindow();
};