//
// Project: Simple Patch
// Date:    2013-03-13
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#include <cstdio>

#include <gtest/gtest.h>

#include "Patch.h"


//
//  Initialize
//

// TODO
/*class PatchEnv: public testing::Environment
{
public:
    static char         fillChar()      { return '0'; }
    static int          fileSize()      { return 1024; }
    static unsigned int oldFileCrc32()  { return 0xEFB5AF2E; }
    static const char*  oldFileName()   { return "oldFileName.bin"; }
    static const char*  newFileName()   { return "newFileName.bin"; }
    static const char*  patchFileName() { return "patchFileName.sdiff"; }

protected:
    void SetUp()
    {
        std::string s(fileSize(), fillChar());
        std::ofstream oldFile(oldFileName());
        oldFile.write(s.c_str(), s.length());
        s[2] = '0xAA';
        std::ofstream newFile(newFileName());
        newFile.write(s.c_str(), s.length());
    }

    void TearDown()
    {
        std::remove(oldFileName());
        std::remove(newFileName());
    }
};*/


//
//  Patcher apply
//


TEST(PatchApplyTest, todo)
{
    // TODO
}


//
//  Patcher parse
//


TEST(PatchParseTest, parse_ok_1)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE 1000000\n"
        "CRC 4092B71A\n"
        "\n"
        "   00F15D   30   00   00   \n"
        "\n"
        "00F2AF 30\n"
        "\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_TRUE(p.parse_(ss));
    ASSERT_EQ(p.fileSize_, 1000000);
    ASSERT_EQ(p.fileCrc32_, 0x4092B71A);
    ASSERT_EQ(p.diffData_.size(), 2);
    auto d = p.diffData_.cbegin();
    ASSERT_EQ(d->first, 0x00F15D);
    ASSERT_EQ(d->second.size(), 3);
    ASSERT_EQ(d->second[0], 0x30);
    ASSERT_EQ(d->second[1], 0);
    ASSERT_EQ(d->second[2], 0);
    ++d;
    ASSERT_EQ(d->first, 0x00F2AF);
    ASSERT_EQ(d->second.size(), 1);
    ASSERT_EQ(d->second[0], 0x30);
}


TEST(PatchParseTest, parse_ok_2)
{
    std::string s(

        "SIMPLEDIFF\n"
        "size   \n"
        "Crc\n"
        "   00F15D   30   00   00   \n"
        "00F2AF 30");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_TRUE(p.parse_(ss));
    ASSERT_EQ(p.fileSize_, 0);
    ASSERT_EQ(p.fileCrc32_, 0);
    ASSERT_EQ(p.diffData_.size(), 2);
    auto d = p.diffData_.cbegin();
    ASSERT_EQ(d->first, 0x00F15D);
    ASSERT_EQ(d->second.size(), 3);
    ASSERT_EQ(d->second[0], 0x30);
    ASSERT_EQ(d->second[1], 0);
    ASSERT_EQ(d->second[2], 0);
    ++d;
    ASSERT_EQ(d->first, 0x00F2AF);
    ASSERT_EQ(d->second.size(), 1);
    ASSERT_EQ(d->second[0], 0x30);
}


TEST(PatchParseTest, invalid_signature)
{
    std::string s("SIZE\n");
    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_SIGNATURE);
}


TEST(PatchParseTest, skip_size)
{
    std::string s(

        "SIMPLEDIFF\n"
        "00F2AF 30");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_SIZE);
}


TEST(PatchParseTest, invalid_size)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE AA\n"
        "00F2AF 30");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_SIZE);
}


TEST(PatchParseTest, skip_crc)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "00F2AF 30");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_CRC32);
}


TEST(PatchParseTest, invalid_crc)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC ZZ\n"
        "00F2AF 30");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_CRC32);
}


TEST(PatchParseTest, empty_patch_data_1)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::EMPTY_PATCH);
}


TEST(PatchParseTest, empty_patch_data_2)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::EMPTY_PATCH);
}


TEST(PatchParseTest, invalid_offset)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC\n"
        "ZZZZZZ 00 00 00\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_OFFSET_VALUE);
}


TEST(PatchParseTest, offset_larger_than_size)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE 250\n"
        "CRC\n"
        "FFFF 00 00 00\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_OFFSET_VALUE);
}


TEST(PatchParseTest, invalid_byte)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC\n"
        "FFFFFF ZZ\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_BYTE_VALUE);
}


TEST(PatchParseTest, byte_larger_than_FF)
{
    std::string s(

        "SIMPLEDIFF\n"
        "SIZE\n"
        "CRC\n"
        "FFFFFF 0100\n");

    std::istringstream ss(s);
    Patch::Patcher p;
    ASSERT_FALSE(p.parse_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::INVALID_BYTE_VALUE);
}


//
//  Patcher compare
//


TEST(PatchCompareTest, different_size)
{
    std::istringstream ss1(std::string("book1"));
    std::istringstream ss2(std::string("book11"));
    Patch::Patcher p;
    ASSERT_FALSE(p.compare_(ss1, ss2));
    ASSERT_EQ(p.getLastError(), Patch::Error::DIFFERENT_SIZE);
}


TEST(PatchCompareTest, compare_ok)
{
    std::istringstream ss1(std::string("book1"));
    std::istringstream ss2(std::string("baoz2"));
    Patch::Patcher p;
    ASSERT_TRUE(p.compare_(ss1, ss2));
    ASSERT_EQ(p.getLastError(), Patch::Error::OK);
    ASSERT_EQ(p.fileSize_, 5);
    ASSERT_EQ(p.fileCrc32_, 0xD2C90A2E);

    ASSERT_EQ(p.diffData_.size(), 2);
    auto d = p.diffData_.cbegin();
    ASSERT_EQ(d->first, 1);
    ASSERT_EQ(d->second.size(), 1);
    ASSERT_EQ(d->second[0], 'a');
    ++d;
    ASSERT_EQ(d->first, 3);
    ASSERT_EQ(d->second.size(), 2);
    ASSERT_EQ(d->second[0], 'z');
    ASSERT_EQ(d->second[1], '2');
}


//
//  Patcher save
//


TEST(PatchSaveTest, save_ok)
{
    std::ostringstream ss;
    Patch::Patcher p;
    Patch::Bytes b;
    b.push_back(0x30);
    b.push_back(0x00);
    p.diffData_[0xABCD] = b;
    ASSERT_TRUE(p.save_(ss));
    ASSERT_EQ(p.getLastError(), Patch::Error::OK);
    ASSERT_STREQ(

        "SIMPLEDIFF\n"
        "SIZE 0\n"
        "CRC 00000000\n"
        "ABCD 30 00\n", ss.str().c_str());
}


//
//  main
//


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    // testing::GTEST_FLAG(also_run_disabled_tests) = true;
    return RUN_ALL_TESTS();
}
