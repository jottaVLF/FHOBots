CC = g++
INCLUDE = -I/usr/include/libserial -I/usr/include/opencv4 -I/usr/include/x86_64-linux-gnu -std=c++11 
LIBS = -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -L/usr/share/opencv4 -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_shape -lopencv_imgcodecs -lserial -pthread
CXXFLAGS=-std=c++17 -pedantic -O3

SRC=$(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp) 
OBJ=$(patsubst %.cpp,bin/%.o,$(SRC)) 
BIN=fhobotsTeam
 
all: $(BIN) 
 
$(BIN): $(OBJ) 
	$(CC) $^ -o $@ $(LIBS)

bin/%.o : %.cpp
	$(CC) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)

help:
	@echo "$(OBJ)\n"
	@echo "$(SRC)\n"
 
clean: 
	rm -f $(OBJ)
	rm -f $(BIN)