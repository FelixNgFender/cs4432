# CS 4432 Project 1: Buffer Manager

Author: Thinh Nguyen - 670714455

## Design notes

A table = A directory of files

A data block = A file under the table directory

A DB record = A line in the file

All files same size

Each file: 100 records (100 lines), each 40 bytes -> roughly 4KB/file = mimics a
disk block. NO EOL (`\n`) on each record, just concatenated to form a very long
line.

Each file is the unit of operation for operations between RAM and disk

Record format: A record #j in file #i will have (for i use two digits like 01,
and for j use three digits like 001):

Fi-Recj, Namej, addressj, agej.

Our numbering system will start from 1 for both files (index i) and records
(index j). Index j resets and start with 001 in each file.

Example: the 23rd record in the 3rd file will have the format of (exactly 40
bytes) F03-Rec023, Name023, address023, age023.

Frame struct

BufferPool struct

## Get started

`TODO: how to compile and execute your code.`

`make test` will compile the `scheduler` executable and move it to the expected
location of `run_tests.sh` (i.e., top-level directory).run_tests.sh
`./run_tests.sh` to run the tests.

`make` alone compiles the `scheduler` executable in `./bin`.

`make clean` to clean up compiled files and test output files.

`make workloads` to test all 5 novel workloads.

`make workload-[1..5]` to test a specific workload.

## Test results

Section II: section on test results. A test case will be provided along with the
expected output. This test case will help you testing your code. In the
Readme.txt file state which of the test case commands is successfully working
and which ones are failing. This will help TAs to better test your code and give
you fair grades

## Design decisions

Section III: section describes any design decisions that you do beyond the
design guidelines given in this document. For example, any additional variables
or methods or classes that you add.
