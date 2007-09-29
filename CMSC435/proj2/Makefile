CXX=g++
CXXFLAGS=-ansi -Wall -g
LDFLAGS=
PROJ=Proj2
OBJ=Proj2.o Point.o Triangle.o

$(PROJ): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o $(PROJ)

-include $(OBJ:.o=.d)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -M $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -f $(PROJ) $(OBJ) $(OBJ:.o=.d)
