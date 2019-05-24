OUT_FILE = sim.out
STD = c99

all: $(OUT_FILE)

$(OUT_FILE): main.o simulator.o
	g++ -o $(OUT_FILE) main.o simulator.o -std=$(STD)

main.o: main.cpp
	g++ -c main.cpp -std=$(STD)

simulator.o: simulator.cpp simulator.h
	g++ -c simulator.cpp -std=$(STD)

clean:
	rm -f main.o simulator.o