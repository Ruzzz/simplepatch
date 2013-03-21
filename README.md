Simple Patch
============

**\[EN\]**

Simple diff and patch command line utilities for files with the same size.

## Usage ##

    sdiff oldfile newfile patchfile
    spatch targetfile patchfile[.sdiff|.simplediff] [-nobackup]

## Format of \*.sdiff (\*.simplediff) file ##

Text file, all numbers - hexadecimal:

    <- header start
    SIMPLEDIFF
    SIZE file_size
    CRC file_crc32
    <- header end
    offset byte ...
    offset byte ...
    ...

- `file_size` - Decimal (not hexadecimal). If value is zero or empty then not used.
- `file_crc32` - If value is zero or empty then not used.

Examples:

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

----------

**\[RU\]**

Простые утилиты командной строки diff и patch для работы с файлами одинакового размера.

## Использование ##

    sdiff старый_файл новый_файл патч_файл
    spatch целевой_файл патч_файл[.sdiff|.simplediff] [-nobackup]

## Формат файлов \*.sdiff (\*.simplediff) ##

Это текстовый файл, в котором все числа представлены в шестнадцатеричном исчислении:

    <- начала заголовка
    SIMPLEDIFF
    SIZE размер_файла
    CRC crc32_сумма_файла
    <- конец заголовка
    смещение байт ...
    смещение байт ...
    ...

- `размер_файла` - Десятичное число (не шестнадцатеричное). Если число отсутствует или равно нулю - то не используется.
- `crc32_сумма_файла` - Если число отсутствует или равно нулю - то не используется.

Примеры:

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

## Скачать ##

[Исполняемые файлы для Windows 32bit](http://ge.tt/7Etqtra)

