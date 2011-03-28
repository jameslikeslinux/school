CXX=g++
CXXFLAGS=-ansi -Wall -g
OBJ=spn.o generateinput.o

default: run

spn: spn.o
	$(CXX) $(CXXFLAGS) -o $@ $<

generateinput: generateinput.o
	$(CXX) $(CXXFLAGS) -o $@ $<

-include $(OBJ:.o=.d)

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< | sed 's/$*.o/& $@/g' > $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

input_spn.txt: generateinput
	./generateinput | tee input_spn.txt

run: spn input_spn.txt
	./spn && cat output_spn.txt | tee input_spn.txt

clean:
	-rm -f spn generateinput input_spn.txt output_spn.txt *.o *.d
