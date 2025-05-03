FLAGS := -g -Wall
EXECUTABLES := client server1 server2 server3

all: $(EXECUTABLES)

$(EXECUTABLES): %: cpp/%.cpp
	g++ $(FLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)