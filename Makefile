#compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 -pthread

COMMON= core/utils.h core/cxxopts.h core/get_time.h core/problemInput.h
SERIAL= knapsack_serial
PROBLEM_GENERATOR = knapsack_generator
PARALLEL= knapsack_parallel
ALL= $(SERIAL) $(PROBLEM_GENERATOR) $(PARALLEL)

all : $(ALL)

% : %.cpp $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $<
generate_test_input0:
	./knapsack_generator 10 1000 1 5 1000

generate_test_input1: 
	./knapsack_generator 1000 1000 1 5 1000
generate_test_input2:
	./knapsack_generator 10000 1000 1 5 1000
generate_test_input3:
	./knapsack_generator 100000 1000 1 5 1000

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)
	rm -r input_files/input.in