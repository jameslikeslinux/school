CC=gcc
CFLAGS=-Wall -g
CXX=g++
CXXFLAGS=-ansi -Wall -g
LDFLAGS=-lGL -lGLU -lglut
PROJ=proj3
OBJ=draw.o motion.o proj3.o view.o Draw.o Point.o Triangle.o

$(PROJ): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $(PROJ)

-include $(OBJ:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -M $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -M $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -f $(PROJ) $(OBJ) $(OBJ:.o=.d)
