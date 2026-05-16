#include "../vision/Kalman2D.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
    Kalman2D kalman;
    kalman.configure(5e-2, 9.0, 25.0, 1.0);

    std::cout << "frame,measured_x,measured_y,filtered_x,filtered_y,velocity_x,velocity_y\n";

    for(int frame = 0; frame < 40; frame++)
    {
        const double trueX = 50.0 + frame * 4.0;
        const double trueY = 100.0 + frame * 2.0;
        const double noiseX = (frame % 5 - 2) * 1.8;
        const double noiseY = std::sin(frame * 0.7) * 2.5;
        const double measuredX = trueX + noiseX;
        const double measuredY = trueY + noiseY;

        if(!kalman.isInitialized())
            kalman.init(measuredX, measuredY);
        else
        {
            kalman.predict();
            kalman.correct(measuredX, measuredY);
        }

        Vector2D filtered = kalman.position();
        Vector2D velocity = kalman.velocity();

        std::cout << frame << ','
                  << std::fixed << std::setprecision(3)
                  << measuredX << ','
                  << measuredY << ','
                  << filtered.x << ','
                  << filtered.y << ','
                  << velocity.x << ','
                  << velocity.y << '\n';
    }

    return 0;
}
