#ifndef FHOBOTS_VSSSCRIPT_HPP
#define FHOBOTS_VSSSCRIPT_HPP

#include <string>
#include <vector>

namespace vssscript {

enum class RobotRole {
    Attacker,
    Defender,
    Goalkeeper
};

enum class FieldArea {
    Attack,
    Defense,
    Center
};

enum class ConditionType {
    Always,
    KeySpace,
    BallInArea,
    RobotHasBall,
    RobotNearBall,
    RobotInArea
};

enum class ActionType {
    UseState,
    GoToBall,
    GoToPosition,
    KickGoal,
    KickCenter,
    DefendGoal,
    HoldDefense,
    MarkBall,
    Support
};

struct SourceLocation {
    int line = 1;
    int column = 1;
};

struct Condition {
    ConditionType type = ConditionType::Always;
    RobotRole robot = RobotRole::Attacker;
    FieldArea area = FieldArea::Center;
    SourceLocation location;
};

struct Action {
    RobotRole robot = RobotRole::Attacker;
    ActionType type = ActionType::UseState;
    std::string value;
    int x = 0;
    int y = 0;
    SourceLocation location;
};

struct Statement {
    Condition condition;
    std::vector<Action> actions;
};

struct Strategy {
    std::string name;
    std::vector<Statement> statements;
};

bool parseSource(const std::string& source, Strategy& strategy, std::string& error);
bool validateStrategy(const Strategy& strategy, std::string& error);
std::string describeStrategy(const Strategy& strategy);

class VssScript {
public:
    bool loadFromFile(const std::string& path);
    bool loadSource(const std::string& source);
    void execute() const;
    std::string describe() const;
    const std::string& error() const;
    const std::string& name() const;
    bool isLoaded() const;

private:
    Strategy _strategy;
    std::string _error;
    bool _loaded = false;
};

}

#endif
