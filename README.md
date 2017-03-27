# A simple webserver

[![Build Status](https://travis-ci.org/rski/websevere.svg?branch=master)](https://travis-ci.org/rski/websevere)

## Features

- Now serving files of size of  up to 1024 * sizeof char!
- Suffers from the same vulnerability as Miele's dishwasher:
  https://www.theregister.co.uk/2017/03/26/miele_joins_internetofst_hall_of_shame/
- Has a malapropism as a title.
- Actually runs.

## Limitations

- Very rudimentary HTTP 1.1 implementation
- Single threaded
- Accepts one connection, tries to serve it and closes down
- Everything is dumped in one function, main
- Limited self-awareness in the areas it is limited in

## Using this thing

No.

Well, ok.

To compile and run it:

    make

To see it serve the index webpage, go on your favourite browser and in the url bar enter:

    localhost:2333

To see it fail in serving something, enter any other url:

    localhost:2333/doot

To see it 403, create a file that it doesn't have permissions to read:

    cd webeserver
    touch doot.html
    sudo chown root:root doot.html
    sudo chown 600 doot.html

    localhost:2333/doot.html
