%option noyywrap nounput noinput caseless yylineno

%{
#include "VssParser.hpp"

#include <string>

int vss_yycolumn = 1;

#define YY_USER_ACTION                                      \
    yylloc.first_line = yylineno;                          \
    yylloc.first_column = vss_yycolumn;                    \
    for(int i = 0; i < yyleng; ++i){                       \
        if(yytext[i] == '\n')                              \
            vss_yycolumn = 1;                              \
        else                                               \
            ++vss_yycolumn;                                \
    }                                                      \
    yylloc.last_line = yylineno;                           \
    yylloc.last_column = vss_yycolumn > 1 ? vss_yycolumn - 1 : 1;
%}

%%

[ \t\r\n]+              ;
"#".*                   ;

"STRATEGY"              return STRATEGY;
"IF"                    return IF;
"THEN"                  return THEN;
"END"                   return END;

"BALL"                  return BALL;
"ROBOT"                 return ROBOT;
"GOAL"                  return GOAL;
"KEY"                   return KEY;
"SPACE"                 return SPACE;

"IN"                    return IN;
"HAS_BALL"              return HAS_BALL;
"NEAR"                  return NEAR;

"USE"                   return USE;
"GO_TO"                 return GO_TO;
"KICK"                  return KICK;
"DEFEND"                return DEFEND;
"SUPPORT"               return SUPPORT;
"HOLD"                  return HOLD;
"MARK"                  return MARK;

"ATTACKER"              return ATTACKER;
"DEFENDER"              return DEFENDER;
"GOALKEEPER"            return GOALKEEPER;

"ATTACK_AREA"           return ATTACK_AREA;
"DEFENSE_AREA"          return DEFENSE_AREA;
"DEFENCE_AREA"          return DEFENCE_AREA;
"CENTER_AREA"           return CENTER_AREA;

"LEFT"                  return LEFT;
"RIGHT"                 return RIGHT;
"CENTER"                return CENTER;

[0-9]+                  {
                            yylval.text = new std::string(yytext);
                            return NUMBER;
                        }

[A-Za-z_][A-Za-z0-9_]*  {
                            yylval.text = new std::string(yytext);
                            return IDENTIFIER;
                        }

.                       return yytext[0];

%%
