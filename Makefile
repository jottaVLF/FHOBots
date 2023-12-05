CC = g++
CFLAGS = -I/usr/include/libserial -I/usr/include/opencv4 -I/usr/include/x86_64-linux-gnu -std=c++11 
LIBS = -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -L/usr/share/opencv4 -lopencv_core -lallegro -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_shape -lopencv_imgcodecs -lserial -pthread -L./sim/remoteAPI
SIMLIBS = -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DDO_NOT_USE_SHARED_MEMORY
DEPS = sim/extApi.c\
	   input/JoystickHandle.cpp\
	   input/JoystickSimulationInterface.cpp\
	   sim/extApiPlatform.c\
	   sim/SimDigitalTwin.cpp\
	   model/Robot.cpp\
	   model/AttackerRobot.cpp\
	   model/DefenderRobot.cpp\
	   model/GoalkeeperRobot.cpp\
	   model/Vector2D.cpp\
	   model/Area.cpp\
	   config/ConfigParser.cpp\
	   config/Token.cpp\
	   Global.cpp\
	   logging/Logger.cpp\
	   communication/Communication.cpp\
	   communication/CommunicationSim.cpp\
	   vision/Vision.cpp\
	   vision/VisionSim.cpp\
	   strategy/MachineState.cpp\
	   control/Control.cpp\
	   strategy/basic/StateIdle.cpp\
	   strategy/basic/StateExitArea.cpp\
	   strategy/attacker/AttackerStateAttacking.cpp\
	   strategy/attacker/AttackerStateWaiting.cpp\
	   strategy/attacker/AttackerStateSeeking.cpp\
	   vision/ColorDetection.cpp\
	   strategy/attacker/AttackerStateSpinning.cpp\
	   strategy/attacker/AttackerJoystickControl.cpp\
	   model/Field.cpp\
	   strategy/basic/StateBackOff.cpp\
	   vision/CustomTrackbar.cpp\
	   strategy/attacker/AttackerStateAlign.cpp\
	   strategy/defender/DefenderStateSeeking.cpp\
	   strategy/defender/DefenderStateAlign.cpp\
	   strategy/defender/DefenderStateWaiting.cpp\
	   strategy/defender/DefenderStateKicking.cpp\
	   strategy/goalkeeper/GoalkeeperStateWaiting.cpp\
	   strategy/goalkeeper/GoalkeeperStateKicking.cpp\
	   strategy/goalkeeper/GoalkeeperStateSpinning.cpp\
	   strategy/goalkeeper/GoalkeeperStateReturnToArea.cpp\
	   strategy/goalkeeper/GoalkeeperStateExitGoal.cpp\
	   strategy/goalkeeper/GoalkeeperStateSeeking.cpp\
	   strategy/goalkeeper/GoalkeeperStateMoveBack.cpp\
	   strategy/goalkeeper/GoalkeeperStateRetreating.cpp\
	   strategy/goalkeeper/GoalkeeperMoveForward.cpp\
	   vision/Mouse.cpp\

TARGET = fhobotsTeam

all: $(TARGET)

$(TARGET): main.cpp 
	$(CC) $(CFLAGS) -g $(DEPS) main.cpp -o $(TARGET) $(LIBS) $(SIMLIBS)

clean:
	$(RM) $(TARGET)
