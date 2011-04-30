CXX=g++-4.3.2
CXXFLAGS=-ansi -Wall -g -O3
OBJ=generate-pc-pairs.o cryptanalyze.o find-best-approx.o

default: run

generate-pc-pairs: generate-pc-pairs.o
	$(CXX) $(CXXFLAGS) -o $@ $<

cryptanalyze: cryptanalyze.o
	$(CXX) $(CXXFLAGS) -o $@ $<

find-best-approx: find-best-approx.o
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
	-rm -f cryptanalyze generate-pc-pairs find-best-approx pc-pairs.txt *.o *.d
