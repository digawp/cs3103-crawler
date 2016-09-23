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

    make run [ARGS=<url>]

Where `url` is any web URL of your choice.
