CXX=g++
CXXFLAGS=-ansi -Wall -g
OBJ=generateinput.o

default: run

generateinput: generateinput.o
	$(CXX) $(CXXFLAGS) -o $@ $<

Feistel.class: Feistel.java
	javac Feistel.java

-include $(OBJ:.o=.d)

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< | sed 's/$*.o/& $@/g' > $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

input_feistel.txt: generateinput
	./generateinput | tee input_feistel.txt

run: Feistel.class input_feistel.txt
	java Feistel && cat output_feistel.txt | tee input_feistel.txt

clean:
	-rm -f generateinput input_feistel.txt output_feistel.txt *.o *.d *.class
