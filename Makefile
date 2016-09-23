default: build

build: main.cpp crawler.cpp
	mkdir -p build
	g++ -std=c++11 main.cpp crawler.cpp -o build/crawl -lhtmlcxx

run: build/crawl
	./build/crawl $(ARGS)

mockbuild: mockmain.cpp
	mkdir -p build
	g++ -std=c++11 mockmain.cpp -o build/mock -lhtmlcxx

mockrun: build/mock
	mkdir -p output
	./build/mock $(ARGS)

clean:
	rm build/*
	rm output/*

.PHONY: build
