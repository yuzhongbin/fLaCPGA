#include "bitreader.hpp"
#include "crc.hpp"


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <iostream>
#include <ios>
#include <fstream>
#include <memory>

#include "gtest/gtest.h"

/* Test File Contents */
/* test1.bin
 * 9    6    8    C    B    0    0    A
 * 1001 0110 1000 1100 1011 0000 0000 1010 
 *         | ||    |        |*/

class BitReaderTestSeek: public ::testing::TestWithParam<std::vector<int> *> {
public: 
    std::shared_ptr<std::fstream> f;
    std::unique_ptr<BitReader> fr;
    
    BitReaderTestSeek() { 
        f = std::make_shared<std::fstream>("test1.bin", std::ios::in | std::ios::binary);
        fr = std::make_unique<BitReader>(f);
    } 

    ~BitReaderTestSeek(){
        f->close();
    }
};

class BitReaderTestReadBits: public ::testing::TestWithParam<std::vector<int> *> {
public: 
    std::shared_ptr<std::fstream> f;
    std::unique_ptr<BitReader> fr;
    
    BitReaderTestReadBits() { 
        f = std::make_shared<std::fstream>("test1.bin", std::ios::in | std::ios::binary);
        fr = std::make_unique<BitReader>(f);
    } 

    ~BitReaderTestReadBits(){
        f->close();
    }
};

TEST_P(BitReaderTestSeek, SeekBits){
    std::vector<int> * const& p = GetParam();
    uint8_t bits_to_seek = p->at(0);
    fr->seek_bits(bits_to_seek);
    EXPECT_EQ(bits_to_seek, fr->get_current_bit());
    EXPECT_EQ(bits_to_seek / 8, fr->get_current_byte());
    
    std::cerr << "Expecting: " << (int) bits_to_seek << " Got: " << fr->get_current_bit() << "\n";
}

INSTANTIATE_TEST_CASE_P(SeekBits, BitReaderTestSeek, ::testing::Values(
    new std::vector<int>{0},
    new std::vector<int>{8},
    new std::vector<int>{9},
    new std::vector<int>{15},
    new std::vector<int>{37}));

TEST_P(BitReaderTestReadBits, ReadBits_UINT8){
    //Tuple_uint8_uint8 const& p = GetParam();
    std::vector<int> * const& p = GetParam();
    uint8_t bits_read;
    fr->seek_bits(p->at(2));
    EXPECT_EQ(p->at(2), fr->get_current_bit());
    EXPECT_EQ(p->at(2) / 8, fr->get_current_byte());
    fr->read_bits(&bits_read, p->at(0));
    EXPECT_EQ(p->at(1), bits_read);
    
    std::cerr << "Read " << p->at(0) << " bits. Got " << (int) bits_read << "\n";
}

INSTANTIATE_TEST_CASE_P(ReadBits_UINT8, BitReaderTestReadBits, ::testing::Values(
    new std::vector<int>{8, 0x96, 0},
    new std::vector<int>{1, 0x01, 8},
    new std::vector<int>{1, 0x00, 9},
    new std::vector<int>{4, 0x03, 10},
    new std::vector<int>{7, 0x16, 14}, 
    new std::vector<int>{24, 0x15486, 21}));

int main(int argc, char **argv){
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}


