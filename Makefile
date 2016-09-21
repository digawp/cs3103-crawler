default: build

build: main.cpp
	mkdir -p build
	g++ main.cpp -o build/connect

run: build/connect
	./build/connect $(ARGS)

clean:
	rm build/*

.PHONY: build
