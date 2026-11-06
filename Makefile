CC = g++
OPT = -O3
WARN = -Wall -Wextra
CFLAGS = $(OPT) $(WARN) $(INC) -Iinclude

SIM_SRC = src/sim_bp.cc src/BranchPredictor.cc src/Hybrid.cc

SIM_OBJ = $(SIM_SRC:.cc=.o)

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm

clean:
	rm -f sim src/*.o
