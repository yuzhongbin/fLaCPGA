/**************************************/
/* frames.cpp - Read in a FLAC Frame */
/************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "bitreader.hpp"

#define READSIZE 1024

/******** Classes for storing FLAC metadata *******/

class FLACFrameHeader {
public:
    FLACFrameHeader();
    int isLast();
    int getBlockType();
    int getBlockLength();
    int getSampleSize();
    uint64_t getBlockSize();
    void print(FILE *f);
    int read(FileReader *fr);
    int read_footer(FileReader *fr);
    int write(FILE *f);
private:
    uint16_t syncCode; 
    uint8_t reserved1;
    uint8_t blockingStrategy;  
    uint8_t blockSizeHint; 
    uint8_t sampleRateHint;
    uint8_t channelAssign;
    uint16_t sampleSize;
    uint8_t reserved2;
    uint64_t sampleNumber;
    uint32_t frameNumber;
    uint16_t blockSize;
    uint32_t sampleRate;
    uint8_t CRC8Poly;
    uint16_t frameFooter;
};

void FLACFrameHeader::print(FILE *f){
    fprintf(f, "\
Sync Code: %x\n\
reserved1: %d\n\
Blocking Strategy: %d\n\
Block Size Indicator: %x\n\
Sample Rate Indicator: %x\n\
Block Size: %d\n\
Sample Rate: %d\n\
Channel Assignment: %d\n\
Sample Size: %d\n\
reserved2: %d\n\
Frame Number: %d\n\
CRC Code: %x\n\n", this->syncCode, this->reserved1, this->blockingStrategy, 
        this->blockSizeHint, this->sampleRateHint, this->blockSize, this->sampleRate,
        this->channelAssign, this->sampleSize, this->reserved2, this->frameNumber, 
        this->CRC8Poly);
}

FLACFrameHeader::FLACFrameHeader(){    
    syncCode = 0;
    reserved1 = 0;
    blockingStrategy = 0;
    blockSizeHint = 0;
    sampleRateHint = 0;
    channelAssign = 0;
    sampleSize = 0;
    reserved2 = 0;
    sampleNumber = 0;
    frameNumber  = 0;
    blockSize = 0;
    sampleRate = 0;
    CRC8Poly = 0;
    frameFooter = 0;
}

int FLACFrameHeader::getSampleSize(){
    return this->sampleSize;
}

uint64_t FLACFrameHeader::getBlockSize(){
    return this->blockSize;
}

int FLACFrameHeader::read(FileReader *fr){
    fr->read_bits_uint16(&this->syncCode, 14);
    fr->read_bits_uint8(&this->reserved1, 1);
    fr->read_bits_uint8(&this->blockingStrategy, 1);
    fr->read_bits_uint8(&this->blockSizeHint, 4);
    fr->read_bits_uint8(&this->sampleRateHint, 4);
    fr->read_bits_uint8(&this->channelAssign, 4);
    fr->read_bits_uint16(&this->sampleSize, 3);
    
    
    /* Interpret sample size */
    switch (this->sampleSize){
        case 0b000:
            // GET FROM STREAMINFO...
            break;
        case 0b001:
            this->sampleSize = 8;
            break;
        case 0b010:
            this->sampleSize = 12;
            break;
        case 0b011:
            //reserved...
            break;
        case 0b100:
            this->sampleSize = 16;
            break;
        case 0b101:
            this->sampleSize = 20;
            break;
        case 0b110:
            this->sampleSize = 24;
            break;
        case 0b111:
            //reserved;
            break;
            
    }
    
    /* Read one reserved bit, should be zero ... */
    fr->read_bits_uint8(&this->reserved2, 1);
    
    /* Read sample or frame number.... */
    if (blockingStrategy){
        uint64_t xx = 0;
        fr->read_utf8_uint64(&xx);
        this->sampleNumber = xx;
    } else {
        uint32_t x = 0;
        fr->read_utf8_uint32(&x);
        this->frameNumber = x;
    }
    
    /* Read in the block size ... */
    switch (blockSizeHint){
        case 0b0110:
            fr->read_bits_uint16(&this->blockSize, 8);
            break;
        case 0b0111:
            fr->read_bits_uint16(&this->blockSize, 16);
            break;
        case 0b0001:
            this->blockSize = 192;
            break;
    }
    
    if (this->blockSizeHint >= 0b0010 && this->blockSizeHint <= 0b0101){
        this->blockSize = 576 * (2 << ((this->blockSizeHint - 2) - 1));
    } else if (this->blockSizeHint >= 0b1000 && this->blockSizeHint <= 0b1111){
        this->blockSize = 256 * (2 << ((this->blockSizeHint - 8) - 1));
    }
    
    /* Read in the sample rate */
    switch (this->sampleRateHint){
        case 0b0000:
            // get from STREAMINFO 
            break;
        case 0b0001:
            this->sampleRate = 88200;
            break;
        case 0b0010:
            this->sampleRate = 176400;
            break;
        case 0b0011: 
            this->sampleRate = 192000;
            break;
        case 0b0100:
            this->sampleRate = 8000;
            break;
        case 0b0101:
            this->sampleRate = 16000;
            break;
        case 0b0110:
            this->sampleRate = 22050;
            break;
        case 0b0111:
            this->sampleRate = 24000;
            break;
        case 0b1000:
            this->sampleRate = 32000;
            break;
        case 0b1001:
            this->sampleRate = 44100;
            break;
        case 0b1010:
            this->sampleRate = 48000;
            break;
        case 0b1011:
            this->sampleRate = 96000;
            break;
        case 0b1100:
            fr->read_bits_uint32(&this->sampleRate, 8) * 1000;
            break;
        case 0b1101:
            fr->read_bits_uint32(&this->sampleRate, 16);
            break;
        case 0b1110:
            fr->read_bits_uint32(&this->sampleRate, 16) * 10;
            break;
        case 0b1111:
            //ERROR!!!
            break;
    }
    
    fr->read_bits_uint8(&this->CRC8Poly, 4);
    return 1; // Add error handling
}

int FLACFrameHeader::read_footer(FileReader *fr){
    fr->read_bits_uint16(&this->frameFooter, 16);
}