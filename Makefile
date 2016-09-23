default: build

build: main.cpp
	mkdir -p build
	g++ -std=c++11 main.cpp -o build/connect -lhtmlcxx

run: build/connect
	./build/connect $(ARGS)

clean:
	rm build/*

.PHONY: build
