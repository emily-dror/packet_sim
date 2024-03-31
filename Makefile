COMPILER := g++
COMPILER_FLAGS := --std=c++17 -DNDEBUG -Wall
SIM_BIN := simulator
HDRS := event.hpp sim.hpp
SRCS := main.cpp sim.cpp event.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
SUBMITTERS := 324934082-324142447

$(SIM_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

debug: clean
	$(COMPILER) -std=c++17 -o simulator *.cpp

zip: $(SRCS) $(HDRS)
	zip $(SUBMITTERS).zip $^ dry.pdf Makefile

clean:
	rm -rf $(SIM_BIN) $(OBJS)
	rm -rf $(SUBMITTERS).zip
