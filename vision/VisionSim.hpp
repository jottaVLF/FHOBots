#ifndef FHOBOTS_VISIONSIM_HPP
#define FHOBOTS_VISIONSIM_HPP

#include "IVision.hpp"
#include "../sim/extApi.h"
#include "../Global.hpp"
#include "opencv2/opencv.hpp"
#include "../model/Vector2D.hpp"
#include <thread>

class VisionSim : public IVision{

    private:
        int _clientId;
        simxInt _ballHandle;
        simxInt _r0FhobotsHandle;
        simxInt _r1FhobotsHandle;
        simxInt _r2FhobotsHandle;
        int _xCamResolution;
        int _yCamResolution;
        bool debug;

        int toXPixelCoord(simxFloat x);
        int toYPixelCoord(simxFloat y);

        void updateRobotPosition(Robot & r, int handle);
        void updateRobotOrientation(Robot & r, int handle);
        void getPositionForObject(Vector2D & area, float centerX, float centerY);
        void getHeightForObject(Area & area, float minY, float maxY);
        void getWidthForObject(Area & area, float minX, float maxX);
        bool isEqualAngle(float angle, float angleRef);
        float simAngleToMath(simxFloat * angles);

    public:
        VisionSim(int clientId, int xCamResolution, int yCamResolution, bool debug = false);
        ~VisionSim();
        void detectionColors();
        void adjustFieldPosition();
        void calibration();

};

#endif