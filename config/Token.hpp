#ifndef FHOBOTS_TOKEN_HPP
#define FHOBOTS_TOKEN_HPP

#include <string>

enum TOKEN_TYPE{
    BEGIN,
    PROPERTY,
    VALUE,
    PROPERTY_SEPARATOR,
    SEPARATOR,
    END
};


class Token{
public:
    std::string lexeme;
    TOKEN_TYPE type; 
    Token(std::string lexeme, TOKEN_TYPE type);
};

#endif