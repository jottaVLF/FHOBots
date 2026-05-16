#ifndef FHOBOTS_IVISION_HPP
#define FHOBOTS_IVISION_HPP


class IVision{

    public:
    virtual void detectionColors() = 0; 
    virtual void adjustFieldPosition() = 0;
    virtual void calibration() = 0;
    virtual void calibratePreProcess() = 0;
};

#endif