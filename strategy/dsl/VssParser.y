%{
#include "VssScript.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

int yylex();
void yyerror(const char* message);

namespace {
vssscript::Strategy* parsedStrategy = nullptr;
std::string* parseError = nullptr;

std::string toLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return value;
}
}
%}

%code requires {
#include "VssScript.hpp"
#include <string>
#include <vector>
}

%locations

%union {
    std::string* text;
    vssscript::RobotRole robot;
    vssscript::FieldArea area;
    vssscript::Condition* condition;
    vssscript::Action* action;
    std::vector<vssscript::Action>* actions;
    vssscript::Statement* statement;
    std::vector<vssscript::Statement>* statements;
}

%token STRATEGY IF THEN END
%token BALL ROBOT GOAL KEY SPACE
%token IN HAS_BALL NEAR
%token USE GO_TO KICK DEFEND SUPPORT HOLD MARK
%token ATTACKER DEFENDER GOALKEEPER
%token ATTACK_AREA DEFENSE_AREA DEFENCE_AREA CENTER_AREA
%token LEFT RIGHT CENTER
%token <text> IDENTIFIER NUMBER

%type <statements> statement_list
%type <statement> statement if_statement action_statement
%type <condition> condition
%type <actions> action_list
%type <action> action
%type <robot> robot
%type <area> field_area

%destructor { delete $$; } <text>
%destructor { delete $$; } <condition>
%destructor { delete $$; } <action>
%destructor { delete $$; } <actions>
%destructor { delete $$; } <statement>
%destructor { delete $$; } <statements>

%%

program:
    STRATEGY IDENTIFIER statement_list
    {
        parsedStrategy->name = *$2;
        parsedStrategy->statements = *$3;
        delete $2;
        delete $3;
    }
;

statement_list:
    statement
    {
        $$ = new std::vector<vssscript::Statement>();
        $$->push_back(*$1);
        delete $1;
    }
    | statement_list statement
    {
        $$ = $1;
        $$->push_back(*$2);
        delete $2;
    }
;

statement:
    if_statement
    {
        $$ = $1;
    }
    | action_statement
    {
        $$ = $1;
    }
;

if_statement:
    IF condition THEN action_list END
    {
        $$ = new vssscript::Statement();
        $$->condition = *$2;
        $$->condition.location = { @1.first_line, @1.first_column };
        $$->actions = *$4;
        delete $2;
        delete $4;
    }
;

action_statement:
    action
    {
        $$ = new vssscript::Statement();
        $$->condition.type = vssscript::ConditionType::Always;
        $$->actions.push_back(*$1);
        delete $1;
    }
;

action_list:
    action
    {
        $$ = new std::vector<vssscript::Action>();
        $$->push_back(*$1);
        delete $1;
    }
    | action_list action
    {
        $$ = $1;
        $$->push_back(*$2);
        delete $2;
    }
;

condition:
    KEY SPACE
    {
        $$ = new vssscript::Condition();
        $$->type = vssscript::ConditionType::KeySpace;
        $$->location = { @1.first_line, @1.first_column };
    }
    | BALL IN field_area
    {
        $$ = new vssscript::Condition();
        $$->type = vssscript::ConditionType::BallInArea;
        $$->area = $3;
        $$->location = { @1.first_line, @1.first_column };
    }
    | robot NEAR BALL
    {
        $$ = new vssscript::Condition();
        $$->type = vssscript::ConditionType::RobotNearBall;
        $$->robot = $1;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot HAS_BALL
    {
        $$ = new vssscript::Condition();
        $$->type = vssscript::ConditionType::RobotHasBall;
        $$->robot = $1;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot IN field_area
    {
        $$ = new vssscript::Condition();
        $$->type = vssscript::ConditionType::RobotInArea;
        $$->robot = $1;
        $$->area = $3;
        $$->location = { @2.first_line, @2.first_column };
    }
;

action:
    robot USE IDENTIFIER
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::UseState;
        $$->value = toLower(*$3);
        $$->location = { @2.first_line, @2.first_column };
        delete $3;
    }
    | robot GO_TO BALL
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::GoToBall;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot GO_TO '(' NUMBER ',' NUMBER ')'
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::GoToPosition;
        $$->x = std::stoi(*$4);
        $$->y = std::stoi(*$6);
        $$->location = { @2.first_line, @2.first_column };
        delete $4;
        delete $6;
    }
    | robot KICK GOAL
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::KickGoal;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot KICK CENTER
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::KickCenter;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot DEFEND GOAL
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::DefendGoal;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot HOLD DEFENSE_AREA
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::HoldDefense;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot HOLD DEFENCE_AREA
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::HoldDefense;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot HOLD IDENTIFIER
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::HoldDefense;
        $$->location = { @2.first_line, @2.first_column };
        delete $3;
    }
    | robot MARK BALL
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::MarkBall;
        $$->location = { @2.first_line, @2.first_column };
    }
    | robot SUPPORT support_area
    {
        $$ = new vssscript::Action();
        $$->robot = $1;
        $$->type = vssscript::ActionType::Support;
        $$->location = { @2.first_line, @2.first_column };
    }
;

support_area:
    LEFT
    | RIGHT
    | CENTER
;

robot:
    ATTACKER
    {
        $$ = vssscript::RobotRole::Attacker;
    }
    | DEFENDER
    {
        $$ = vssscript::RobotRole::Defender;
    }
    | GOALKEEPER
    {
        $$ = vssscript::RobotRole::Goalkeeper;
    }
    | ROBOT NUMBER
    {
        if(*$2 == "1")
            $$ = vssscript::RobotRole::Goalkeeper;
        else if(*$2 == "2")
            $$ = vssscript::RobotRole::Defender;
        else if(*$2 == "3")
            $$ = vssscript::RobotRole::Attacker;
        else {
            yyerror("invalid robot number: use ROBOT 1, ROBOT 2 or ROBOT 3");
            YYERROR;
        }
        delete $2;
    }
;

field_area:
    ATTACK_AREA
    {
        $$ = vssscript::FieldArea::Attack;
    }
    | DEFENSE_AREA
    {
        $$ = vssscript::FieldArea::Defense;
    }
    | DEFENCE_AREA
    {
        $$ = vssscript::FieldArea::Defense;
    }
    | CENTER_AREA
    {
        $$ = vssscript::FieldArea::Center;
    }
;

%%

void yyerror(const char* message)
{
    if(parseError != nullptr){
        std::ostringstream out;
        out << "line " << yylloc.first_line << ", column " << yylloc.first_column
            << ": " << message;
        *parseError = out.str();
    }
}

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char* source);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yyparse();
extern int yylineno;
extern int vss_yycolumn;

namespace vssscript {

bool parseSource(const std::string& source, Strategy& strategy, std::string& error)
{
    strategy = Strategy();
    error.clear();

    parsedStrategy = &strategy;
    parseError = &error;
    yylineno = 1;
    vss_yycolumn = 1;

    YY_BUFFER_STATE buffer = yy_scan_string(source.c_str());
    int result = yyparse();
    yy_delete_buffer(buffer);

    parsedStrategy = nullptr;
    parseError = nullptr;

    return result == 0 && error.empty();
}

}
