default: build

build: main.cpp crawler.cpp
	mkdir -p build
	g++ -std=c++11 main.cpp crawler.cpp -o build/crawl -lhtmlcxx

run: build/crawl
	./build/crawl $(ARGS)

crawler: crawler.cpp crawler.h
	g++ -std=c++11 -c crawler.cpp -o build/crawler.o

storage: storage.cpp storage.h
	g++ -std=c++11 -c storage.cpp -o build/storage.o

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
