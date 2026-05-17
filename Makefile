CC = g++
INCLUDE = -I/usr/include/libserial -I/usr/include/opencv4 -I/usr/include/x86_64-linux-gnu
LIBS = -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -L/usr/share/opencv4 -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_shape -lopencv_imgcodecs -lserial -pthread -lprotobuf 
CXXFLAGS=-std=c++17 -pedantic -O3

DSL_GENERATED=strategy/dsl/VssParser.cpp strategy/dsl/VssLexer.cpp
CPP_SRC=$(filter-out $(DSL_GENERATED),$(wildcard *.cpp) $(wildcard */*.cpp) $(wildcard */*/*.cpp)) $(DSL_GENERATED)
PROTO_SRC=$(wildcard pb/*.cc)
SRC=$(CPP_SRC) $(PROTO_SRC)
OBJ=$(patsubst %.cpp,bin/%.o,$(CPP_SRC)) $(patsubst %.cc,bin/%.o,$(PROTO_SRC))
PROTOC  = protoc 
PROTO_DEP = $(wildcard pb/proto/*.proto) 
BIN=fhobotsTeam
 
ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

all: $(BIN) 
 
$(BIN): $(OBJ) 
	$(CC) $^ -o $@ $(LIBS)

bin/%.o : %.cpp
	$(CC) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)

bin/%.o : %.cc
	$(CC) $(CXXFLAGS) -c $^ -o $@ $(INCLUDE)

strategy/dsl/VssParser.cpp strategy/dsl/VssParser.hpp: strategy/dsl/VssParser.y
	bison -d -o strategy/dsl/VssParser.cpp strategy/dsl/VssParser.y

strategy/dsl/VssLexer.cpp: strategy/dsl/VssLexer.l strategy/dsl/VssParser.hpp
	flex -o strategy/dsl/VssLexer.cpp strategy/dsl/VssLexer.l

help:
	@echo "$(OBJ)\n"
	@echo "$(SRC)\n"
 
clean: 
	rm -f $(OBJ)
	rm -f $(BIN)
	rm -f strategy/dsl/VssParser.cpp strategy/dsl/VssParser.hpp strategy/dsl/VssLexer.cpp

run:
	./$(BIN) $(RUN_ARGS)

protobuf : $(PROTO_DEP)
	$(PROTOC) -Ipb/proto --cpp_out=pb/ $^

bin_dir:
	mkdir -p bin/communication bin/config bin/control bin/debug bin/logging bin/model/ bin/pb bin/strategy/attacker bin/strategy/defender bin/strategy/basic bin/strategy/goalkeeper bin/strategy/dsl bin/vision 
