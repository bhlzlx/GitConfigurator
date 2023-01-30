#pragma once
#include <string>
#include <vector>

enum class ParseState {
    None,
    Catagory,
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
    ParseResult     rst_;
    bool            inCatagory_;
    bool            valueStart_;
    bool            escape_;
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
        , text_{}
        , rst_{}
        , inCatagory_(false)
        , valueStart_(false)
        , escape_(false)
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
                if(!valueStart_) {
                    if(!isBlank(ch)) {
                        valueStart_ = true;
                        text_.clear();
                    } else {
                        break;
                    }
                }
                if(ch == '\r' || ch == '\n'){
                    changeState(ParseState::None);
                    valueStart_ = false;
                } else {
                    if(escape_) {
                        text_.push_back(ch);
                        escape_ = false;
                    } else {
                        if(ch == '\\') {
                            escape_ =  true;
                        } else {
                            text_.push_back(ch);
                        }
                    }
                }
                break;
            }
            case ParseState::Comment: {
                if(ch == '\r' ||ch=='\n') {
                    changeState(ParseState::None);
                } else {
                    if(escape_) {
                        text_.push_back(ch);
                        escape_ = false;
                    } else {
                        if(ch == '\\') {
                            escape_ =  true;
                        } else {
                            text_.push_back(ch);
                        }
                    }
                }
                break;
            }
        }
        checkEOL(ch);
        return rst_;
    }
};

enum class node_type {
    catagory,
    keyvalue,
    comment,
    root,
};

struct node {
    node_type           type;
    node*               parent;
    std::vector<node*>  children;
    //
    std::string         caption;
    std::string         val;
    size_t              index;
    //
    node(node_type t, std::string const& cap, std::string const& v = "")
        : type(t)
        , parent(nullptr)
        , children()
        , caption(cap)
        , val(v)
        , index(0)
    {
    }

    void addChild(node* node) {
        node->index = children.size();
        children.push_back(node);
        node->parent = this;
    }

    bool insertChild(size_t pos, node* node) {
        if(pos>children.size()) {
            return false;
        }
        children.push_back(node);
        for(size_t i = pos; i < children.size() - 1; ++i) {
            std::swap(children[i], children.back());
            children[i]->index = i;
            children.back()->index = children.size() - 1;
        }
        node->parent = this;
        return true;
    }

    bool removeChild(int index) {
        if(index<children.size()) {
            auto ptr = children[index];
            delete ptr;
            for(size_t i = index; i<children.size()-1; ++i) {
                children[i] = children[i+1];
                children[i]->index = i;
            }
            children.pop_back();
            return true;
        }
        return false;
    }
};