CC = g++
INCLUDE = -I/usr/include/opencv4 -I/usr/include/x86_64-linux-gnu -std=c++11
LIBS = -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -L/usr/local/share/opencv4 -lopencv_core -lopencv_video -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_shape -lopencv_imgcodecs -lopencv_calib3d -lopencv_features2d -pthread
CXXFLAGS=-std=c++17 -pedantic -O3

ifeq ($(OS),Windows_NT)
define MKDIR_P
if not exist "$(1)" mkdir "$(1)"
endef
else
define MKDIR_P
mkdir -p $(1)
endef
endif

SRC_ALL=$(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp)
SRC=$(filter-out tests/% main_sim.cpp,$(SRC_ALL))
OBJ=$(patsubst %.cpp,bin/%.o,$(SRC))
BIN=fhobotsTeam

SRC_SIM=$(filter-out tests/% main.cpp,$(SRC_ALL))
OBJ_SIM=$(patsubst %.cpp,bin/%.o,$(SRC_SIM))
BIN_SIM=fhobotsTeam_sim
 
all: $(BIN) 
 
$(BIN): $(OBJ) 
	$(CC) $^ -o $@ $(LIBS)

bin/%.o : %.cpp
	$(call MKDIR_P,$(dir $@))
	$(CC) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)

help:
	@echo "$(OBJ)\n"
	@echo "$(SRC)\n"
 
clean: 
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\,$(OBJ)) $(BIN) $(BIN).exe 2>NUL
else
	rm -f $(OBJ)
	rm -f $(BIN)
endif

sim: $(OBJ_SIM)
	$(CC) $^ -o $(BIN_SIM) $(LIBS)

run-sim: sim
	./$(BIN_SIM)

run:
	./$(BIN)

kalman-test:
	$(CC) $(CXXFLAGS) tests/kalman2d_simulation.cpp vision/Kalman2D.cpp model/Vector2D.cpp -o bin/kalman2d_simulation $(INCLUDE) -lopencv_core -lopencv_video

univector-test:
	$(call MKDIR_P,bin)
	$(CC) $(CXXFLAGS) tests/univector_simulation.cpp navigation/UnivectorField.cpp model/Vector2D.cpp -o bin/univector_simulation
