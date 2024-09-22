CPP = g++ src/main.cpp
ARGS = -std=c++20 -g -Wall -pedantic -Wextra

all: compile finish

compile:
	$(CPP) $(ARGS) -o MyEngine.exe

finish:
	@echo -e "\033[0;32m\nDone at $(shell date +%T)\n\e[0m"
