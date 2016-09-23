# cs3103-crawler

A web crawler written as part of CS3103 Computer Networks Practices assignment in National University of Singapore.

## Prerequisites

### htmlcxx

This program uses [htmlcxx](http://htmlcxx.sourceforge.net/) as HTML parser.

To build and install:

1. Download the package from the link above.

2. `./configure && make && [sudo] make install`

3. (May not be necessary) `sudo ldconfig`

Any problem with htmlcxx installations: go to their INSTALL file (in the package) or google it.


### Linux

The program was built on Ubuntu 15.04, and tested (ran) on [to be filled in]. Not guaranteed to work on any other platform. Definitely won't work on non-Unix environments as the socket codes are Unix(Linux)-specific.

## Building

Make sure you have all the prerequisites above before running the following.

    make

## Running

    make run [SEED=<path/to/seed>]

If left empty, the default `seed` file will be used.

## Testing a seed URL

Before putting a URL into the seed file, you can test if it will make the web crawler stuck (i.e. 404, 301)

    make mockbuild
    make mockrun [URL=<url>]

If left empty, url wi default to www.comp.nus.edu.sg.
