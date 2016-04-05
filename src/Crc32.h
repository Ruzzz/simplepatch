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

    void compute(const void *ptr, size_t size)
    {
        const uint8_t *p = static_cast<const uint8_t*>(ptr);
        for (; size--; ++p)
            compute(*p);
    }

    void compute(const uint8_t b)
    {
        value_ = (value_ >> 8) ^ TABLE[(value_ ^ b) & 0xFF];
    }

    const uint32_t value() const
    {
        return ~value_;
    }

    static bool calc(std::istream &f, unsigned int &resultCrc);

private:
    static const uint32_t TABLE[256];
    uint32_t value_;
};
