#include "VssScript.hpp"

#include "../../Global.hpp"
#include "../../model/WorldModel.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

namespace vssscript {

namespace {

std::string locationPrefix(SourceLocation location)
{
    std::ostringstream out;
    out << "line " << location.line << ", column " << location.column << ": ";
    return out.str();
}

std::string robotName(RobotRole role)
{
    switch(role){
        case RobotRole::Attacker:
            return "ATTACKER";
        case RobotRole::Defender:
            return "DEFENDER";
        case RobotRole::Goalkeeper:
            return "GOALKEEPER";
    }

    return "UNKNOWN";
}

std::string areaName(FieldArea area)
{
    switch(area){
        case FieldArea::Attack:
            return "ATTACK_AREA";
        case FieldArea::Defense:
            return "DEFENSE_AREA";
        case FieldArea::Center:
            return "CENTER_AREA";
    }

    return "UNKNOWN_AREA";
}

Robot& robotFor(RobotRole role)
{
    switch(role){
        case RobotRole::Attacker:
            return Global::attacker;
        case RobotRole::Defender:
            return Global::deffender;
        case RobotRole::Goalkeeper:
            return Global::goalkeeper;
    }

    return Global::attacker;
}

bool isInArea(Vector2D position, FieldArea area)
{
    switch(area){
        case FieldArea::Attack:
            return WorldModel::isInAttackArea(position);
        case FieldArea::Defense:
            return WorldModel::isInDeffenseArea(position);
        case FieldArea::Center:
            return position.x >= 3 * Global::fieldRect.width / 8 &&
                   position.x <= 5 * Global::fieldRect.width / 8;
    }

    return false;
}

bool evaluate(const Condition& condition)
{
    switch(condition.type){
        case ConditionType::Always:
            return true;
        case ConditionType::KeySpace:
            return Global::bufferKeyboard == 32;
        case ConditionType::BallInArea:
            return isInArea(Global::ball, condition.area);
        case ConditionType::RobotHasBall: {
            Robot& robot = robotFor(condition.robot);
            Vector2D robotToBall = Global::ball - robot.getPosition();
            return WorldModel::isNearOf(robot.getPosition(), Global::ball) &&
                   WorldModel::isAlignedWith(robot.getOrientation(), robotToBall);
        }
        case ConditionType::RobotNearBall:
            return WorldModel::isNearOf(robotFor(condition.robot).getPosition(), Global::ball);
        case ConditionType::RobotInArea:
            return isInArea(robotFor(condition.robot).getPosition(), condition.area);
    }

    return false;
}

std::string mappedState(const Action& action)
{
    if(action.type == ActionType::UseState)
        return action.value;

    switch(action.type){
        case ActionType::GoToBall:
        case ActionType::GoToPosition:
        case ActionType::MarkBall:
            return "seeking";
        case ActionType::KickGoal:
        case ActionType::KickCenter:
            return action.robot == RobotRole::Attacker ? "attacking" : "kicking";
        case ActionType::DefendGoal:
        case ActionType::HoldDefense:
        case ActionType::Support:
            return "waiting";
        case ActionType::UseState:
            return action.value;
    }

    return "";
}

void executeAction(const Action& action)
{
    if(action.type == ActionType::GoToPosition){
        Vector2D destination(action.x, action.y);
        Robot& robot = robotFor(action.robot);
        robot.calculatePwmUnivector(destination);
        Global::communication->writeMessage(robot.getPosMessage(), robot.getPwmLeft(), robot.getPwmRight());
        return;
    }

    std::string state = mappedState(action);
    if(!state.empty())
        robotFor(action.robot).setState(state);
}

}

bool validateStrategy(const Strategy& strategy, std::string& error)
{
    if(strategy.name.empty()){
        error = "strategy name cannot be empty";
        return false;
    }

    const std::array<std::string, 10> attackerStates = {
        "idle", "backoff", "attacking", "seeking", "waiting", "spinning", "align", "exit", "", ""
    };
    const std::array<std::string, 10> defenderStates = {
        "idle", "backoff", "seeking", "waiting", "kicking", "spinning", "align", "exit", "", ""
    };
    const std::array<std::string, 12> goalkeeperStates = {
        "idle", "seeking", "kicking", "backoff", "moveback", "moveforward",
        "spinning", "align", "waiting", "return", "exit", "retreating"
    };

    auto stateExists = [&](RobotRole role, const std::string& state) {
        if(role == RobotRole::Attacker)
            return std::find(attackerStates.begin(), attackerStates.end(), state) != attackerStates.end();
        if(role == RobotRole::Defender)
            return std::find(defenderStates.begin(), defenderStates.end(), state) != defenderStates.end();
        return std::find(goalkeeperStates.begin(), goalkeeperStates.end(), state) != goalkeeperStates.end();
    };

    for(const Statement& statement : strategy.statements){
        if(statement.actions.empty()){
            error = locationPrefix(statement.condition.location) + "statement has no actions";
            return false;
        }

        for(const Action& action : statement.actions){
            if(action.type == ActionType::UseState && !stateExists(action.robot, action.value)){
                error = locationPrefix(action.location) + robotName(action.robot) +
                        " does not have state '" + action.value + "'";
                return false;
            }

            if(action.type == ActionType::KickCenter && action.robot == RobotRole::Goalkeeper){
                error = locationPrefix(action.location) + "GOALKEEPER cannot KICK CENTER in current semantic rules";
                return false;
            }
        }
    }

    return true;
}

std::string describeStrategy(const Strategy& strategy)
{
    std::ostringstream out;
    out << "[OK] Strategy: " << strategy.name << "\n";

    for(const Statement& statement : strategy.statements){
        out << "[IF] ";
        switch(statement.condition.type){
            case ConditionType::Always:
                out << "ALWAYS";
                break;
            case ConditionType::KeySpace:
                out << "KEY SPACE";
                break;
            case ConditionType::BallInArea:
                out << "BALL IN " << areaName(statement.condition.area);
                break;
            case ConditionType::RobotHasBall:
                out << robotName(statement.condition.robot) << " HAS_BALL";
                break;
            case ConditionType::RobotNearBall:
                out << robotName(statement.condition.robot) << " NEAR BALL";
                break;
            case ConditionType::RobotInArea:
                out << robotName(statement.condition.robot) << " IN " << areaName(statement.condition.area);
                break;
        }
        out << "\n";

        for(const Action& action : statement.actions){
            out << "  [ACTION] " << robotName(action.robot) << " -> ";
            switch(action.type){
                case ActionType::UseState:
                    out << "use state " << action.value;
                    break;
                case ActionType::GoToBall:
                    out << "go to ball";
                    break;
                case ActionType::GoToPosition:
                    out << "go to (" << action.x << ", " << action.y << ")";
                    break;
                case ActionType::KickGoal:
                    out << "kick to goal";
                    break;
                case ActionType::KickCenter:
                    out << "kick to center";
                    break;
                case ActionType::DefendGoal:
                    out << "defend goal";
                    break;
                case ActionType::HoldDefense:
                    out << "hold defense";
                    break;
                case ActionType::MarkBall:
                    out << "mark ball";
                    break;
                case ActionType::Support:
                    out << "support";
                    break;
            }
            out << "\n";
        }
    }

    return out.str();
}

bool VssScript::loadFromFile(const std::string& path)
{
    _loaded = false;
    _error.clear();
    _strategy = Strategy();

    std::ifstream file(path);
    if(!file.is_open()){
        _error = "Could not open DSL script: " + path;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return loadSource(buffer.str());
}

bool VssScript::loadSource(const std::string& source)
{
    _loaded = false;
    _error.clear();
    _strategy = Strategy();

    if(!parseSource(source, _strategy, _error))
        return false;

    if(!validateStrategy(_strategy, _error))
        return false;

    _loaded = true;
    return true;
}

void VssScript::execute() const
{
    if(!_loaded)
        return;

    for(const Statement& statement : _strategy.statements){
        if(evaluate(statement.condition)){
            for(const Action& action : statement.actions)
                executeAction(action);
        }
    }
}

std::string VssScript::describe() const
{
    if(!_loaded)
        return "";

    return describeStrategy(_strategy);
}

const std::string& VssScript::error() const
{
    return _error;
}

const std::string& VssScript::name() const
{
    return _strategy.name;
}

bool VssScript::isLoaded() const
{
    return _loaded;
}

}
