spatch
======

Simple diff and patch command line utilities for files with the same size.

## Format of \*.sdiff(\*.simplediff) file. ##

Text file, all numbers - hex:

    SIMPLEDIFF
    crc32
    offset byte ...
    offset byte ...
    ...

Example:

    SIMPLEDIFF
    4092B71A
    00F15D 30 00 00
    00F2AF 30
