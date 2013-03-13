simple patch
============

Simple diff and patch command line utilities for files with the same size.

## Usage ##

    sdiff oldfile newfile patchfile
    spatch targetfile patchfile[.sdiff|.simplediff] [-nobackup]

## Format of \*.sdiff (\*.simplediff) file. ##

Text file, all numbers - hexadecimal:

    <- header_start
    SIMPLEDIFF
    SIZE file_size
    CRC file_crc32
    <- header_end
    offset byte ...
    offset byte ...
    ...

- `file_size` - Decimal (not hexadecimal). If value is zero or empty then not used.
- `file_crc32` - If value is zero or empty then not used.

Example:

    SIMPLEDIFF
    SIZE 1000000
    CRC 4092B71A
    00F15D 30 00 00
    00F2AF 30

    SIMPLEDIFF
    SIZE
    CRC
    00F15D 30 00 00
    00F2AF 30

## Download ##

[Win32 exe files](http://ge.tt/7Etqtra)
