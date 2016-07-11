.PHONY: all clean install uninstall

all: src/main.cpp 
	g++ -g src/main.cpp -o task_generator -lintelib

clean:
	rm -rf task_generator *.o
	rm -rf task_generator *.dSYM