//
// Project: Simple Patch
// Date:    2013-03-07
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#pragma once


class Crc32
{
public:
    Crc32() : value_(0xFFFFFFFF)
    {
    }

    void reset()
    {
        value_ = 0xFFFFFFFF;
    }

    void compute(const void *data, const unsigned int size)
    {
        char *p = reinterpret_cast<char *>(const_cast<void *>(data));
        unsigned int i = size;
        for (; i--; ++p)
            compute(*p);
    }

    void compute(const char c)
    {
        unsigned char uc = c;
        value_ = (value_ >> 8) ^ TABLE[(value_ ^ uc) & 0xFF];
    }

    const unsigned int value() const
    {
        return ~value_;
    }

    static bool calc(std::istream &f, unsigned int &resultCrc);

private:
    static const unsigned int TABLE[256];
    unsigned int value_;
};
