#include "../navigation/UnivectorField.hpp"
#include <iostream>
#include <vector>

namespace
{
    UnivectorOutput runFrames(UnivectorField& field, const UnivectorInput& input, int frames)
    {
        UnivectorOutput output;
        for(int i = 0; i < frames; i++)
            output = field.compute(input);

        return output;
    }

    void printOutput(const UnivectorOutput& output)
    {
        std::cout << "  valid: " << output.valid
                  << " reverse: " << output.reverse
                  << " angle_error: " << output.angleError << '\n';
        std::cout << "  direction: (" << output.desiredDirection.x
                  << ", " << output.desiredDirection.y << ")\n";
        std::cout << "  waypoint: (" << output.waypoint.x
                  << ", " << output.waypoint.y << ")\n";
    }

    void printCase(const char* name, const UnivectorInput& input, int frames = 1)
    {
        UnivectorConfig config;
        UnivectorField field(config);
        const UnivectorOutput output = runFrames(field, input, frames);

        std::cout << name << '\n';
        printOutput(output);
    }

    UnivectorInput baseInput()
    {
        UnivectorInput input;
        input.robotPosition = Vector2D(0.0, 0.0);
        input.robotOrientation = Vector2D(1.0, 0.0);
        input.targetPosition = Vector2D(100.0, 0.0);
        input.ballPosition = Vector2D(100.0, 0.0);
        return input;
    }
}

int main()
{
    UnivectorInput noObstacle = baseInput();
    printCase("case 1: target without obstacle", noObstacle);

    UnivectorInput centerObstacle = baseInput();
    centerObstacle.obstacles.push_back(UnivectorObstacle(Vector2D(35.0, 0.0), 18.0));
    printCase("case 2: central obstacle", centerObstacle);

    UnivectorInput lateralObstacle = baseInput();
    lateralObstacle.obstacles.push_back(UnivectorObstacle(Vector2D(35.0, 25.0), 18.0));
    printCase("case 3: lateral obstacle", lateralObstacle);

    UnivectorInput symmetricObstacles = baseInput();
    symmetricObstacles.obstacles.push_back(UnivectorObstacle(Vector2D(38.0, 20.0), 18.0));
    symmetricObstacles.obstacles.push_back(UnivectorObstacle(Vector2D(38.0, -20.0), 18.0));
    printCase("case 4: two symmetric obstacles", symmetricObstacles);

    UnivectorConfig config;
    UnivectorField persistentField(config);
    UnivectorInput frontTarget = baseInput();
    UnivectorInput targetBehind = baseInput();
    targetBehind.targetPosition = Vector2D(-100.0, 0.0);
    targetBehind.ballPosition = Vector2D(-100.0, 0.0);

    std::cout << "case 5: target behind robot with reverse persistence\n";
    printOutput(persistentField.compute(frontTarget));
    printOutput(persistentField.compute(targetBehind));
    printOutput(persistentField.compute(targetBehind));
    printOutput(persistentField.compute(targetBehind));

    return 0;
}
