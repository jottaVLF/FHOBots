#include "Token.hpp"

Token::Token(std::string lexeme, TOKEN_TYPE type){
    this->lexeme = lexeme;
    this->type = type;
}