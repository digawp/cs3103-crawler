# cs3103-crawler

A web crawler written as part of CS3103 Computer Networks Practices assignment in National University of Singapore.

## Prerequisites

### htmlcxx

This program uses [htmlcxx](http://htmlcxx.sourceforge.net/) as HTML parser.

To build and install:

1. Download the package from the link above.

2. `./configure && make && [sudo] make install`

3. (May not be necessary) export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

4. (May not be necessary) `sudo ldconfig`

Any problem with htmlcxx installations: go to their INSTALL file (in the package) or google it. Note that the problem usually surface only during program compilation and/or runtime.

### Linux

The program was built on Ubuntu 15.04, and tested (ran) on [to be filled in]. Not guaranteed to work on any other platform. Definitely won't work on non-Unix environments as the socket codes are Unix(Linux)-specific.

### Regex

The program uses C++11 regex standard library. Make sure your C++ compiler supports it (for `gcc`, minimum version is 4.9).

## Building

Make sure you have all the prerequisites above before running the following.

    make

## Running

    make run [SEED=<path/to/seed> | LIMIT=<url log limit>]

If `SEED` left empty, the default `seed` file will be used.
If `LIMIT` left empty, the default `LIMIT` is 30.
The two arguments are optional.

Alternatively, run the executable generated under `build/` directory.

    <path/to/program> [<path/to/seed> | <url log limit>]

Same as its `make` counterpart, the arguments are optional and the order does not matter. Integers will be assumed as `url log limit`, otherwise it is assumed as `path/to/seed`.

## Testing a seed URL

Before putting a URL into the seed file, you can test if it will make the web crawler stuck (i.e. 404, 301)

    make mockbuild
    make mockrun [URL=<url>]

If left empty, url will default to www.comp.nus.edu.sg.
