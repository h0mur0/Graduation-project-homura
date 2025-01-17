CXX = g++
CXXFLAGS = -std=c++11 -O2
LDFLAGS = 

SRC = public_function.cpp .cpp client.cpp leader.cpp database.cpp channel.cpp main.cpp
OBJ = $(SRC:.cpp=.o)

all: main

main: $(OBJ)
	$(CXX) $(OBJ) -o main $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main
