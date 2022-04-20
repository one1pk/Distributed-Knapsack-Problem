#compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 -pthread

COMMON= core/utils.h core/cxxopts.h core/get_time.h core/problemInput.h
SERIAL = knapsack_serial
PARALLEL = knapsack_parallel
PROBLEM_GENERATOR = knapsack_generator
PARALLEL_THREADS= knapsack_parallel
PARALLEL_MPI = knapsack_mpi

ALL= $(SERIAL) $(PROBLEM_GENERATOR) $(PARALLEL_THREADS) $(PARALLEL_MPI)


all : $(ALL)
$(PROBLEM_GENERATOR):  %: %.cpp
	@mkdir input_files
	$(CXX) $(CXXFLAGS) -o $@ $<
$(SERIAL): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
$(PARALLEL_THREADS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
$(PARALLEL_MPI): %: %.cpp
	$(MPICXX) $(CXXFLAGS) -o $@ $<

generate_test_input0: 
	@echo "Generating 500 items with uncorrelated weights and values..." 
	./knapsack_generator 500 1000 1 5 1000
	@echo "finished generating input."
generate_test_input1: 
	@echo "Generating 1000 items with uncorrelated weights and values..." 
	./knapsack_generator 1000 1000 1 5 1000
	@echo "finished generating input."
generate_test_input2:
	@echo "Generating 10000 items with uncorrelated weights and values..." 
	./knapsack_generator 10000 1000 1 5 1000
	@echo "finished generating input."
generate_test_input3:
	@echo "Generating 50000 items with uncorrelated weights and values..." 
	./knapsack_generator 50000 1000 1 5 1000
	@echo "finished generating input."

.PHONY : clean
clean :
	rm -f *.o *.obj $(ALL)
	rm -r input_files
