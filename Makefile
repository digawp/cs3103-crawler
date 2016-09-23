default: build

build: main.cpp
	mkdir -p build
	g++ -std=c++11 main.cpp -o build/connect -lhtmlcxx

run: build/connect
	mkdir -p output
	./build/connect $(ARGS)

clean:
	rm build/*
	rm output/*

.PHONY: build
