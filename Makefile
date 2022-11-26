CC := g++ -Wno-pointer-arith -Wwrite-strings
SRC := main.cpp diftree.cpp differentiator.cpp text-sort.cpp recursivedescent.cpp
REC := main.cpp recursivedescent.cpp
DIR := C:\Users\USER\Documents\Differentiator

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $(DIR)\$@

rec: $(REC)
	$(CC) $^ -o $(DIR)\$@

clean:
	rm -rf *.png *.dot
