default: build

build: main.cpp crawler.cpp storage.cpp
	mkdir -p build
	g++ -std=c++11 main.cpp crawler.cpp storage.cpp -o build/crawl -lhtmlcxx -lpthread

run: build/crawl
	./build/crawl $(SEED)

crawler: crawler.cpp crawler.h
	g++ -std=c++11 -c crawler.cpp -o build/crawler.o

storage: storage.cpp storage.h
	g++ -std=c++11 -c storage.cpp -o build/storage.o

mockbuild: mockmain.cpp
	mkdir -p build
	g++ -std=c++11 mockmain.cpp -o build/mock -lhtmlcxx

mockrun: build/mock
	mkdir -p output
	./build/mock $(URL)

clean:
	rm -f build/*
	rm -f output/*
	rm -f url_log.txt

.PHONY: build
