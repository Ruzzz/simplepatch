simple patch
============

Simple diff and patch command line utilities for files with the same size.

## Format of \*.sdiff (\*.simplediff) file. ##

Text file, all numbers - hexadecimal:

    SIMPLEDIFF
    SIZE filesize
    CRC crc32
    offset byte ...
    offset byte ...
    ...

- `filesize` - Decimal (not hexadecimal). If value is zero or empty then not used.
- `crc32` - If value is zero or empty then not used.

Example:

    SIMPLEDIFF
    SIZE 1000000
    CRC 4092B71A
    00F15D 30 00 00
    00F2AF 30

    SIMPLEDIFF
    size 0
    crc
    00F15D 30 00 00
    00F2AF 30

## Download ##

[Win32 exe files](http://ge.tt/9jj1ara)
