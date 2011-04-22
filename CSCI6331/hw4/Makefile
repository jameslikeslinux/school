CXX=g++-4.3.2
CXXFLAGS=-ansi -Wall -g
OBJ=generate-pc-pairs.o cryptanalyze.o

default: run

generate-pc-pairs: generate-pc-pairs.o
	$(CXX) $(CXXFLAGS) -o $@ $<

cryptanalyze: cryptanalyze.o
	$(CXX) $(CXXFLAGS) -o $@ $<

-include $(OBJ:.o=.d)

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< | sed 's/$*.o/& $@/g' > $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

pc-pairs.txt: generate-pc-pairs
	./generate-pc-pairs

run: cryptanalyze pc-pairs.txt
	./cryptanalyze

clean:
	-rm -f cryptanalyze generate-pc-pairs pc-pairs.txt *.o *.d
