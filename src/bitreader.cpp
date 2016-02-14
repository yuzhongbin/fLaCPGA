/* Implementation of a bitreader */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define READ_COUNT 512


class FileReader {
private:
    FILE *fin;
    uint64_t bit;
    
    int bytes_read;
    int bytes_consumed;
    
    int eof;
    
    uint8_t bitbuf[1];
    uint8_t buffer[READ_COUNT];
    
    uint8_t get_mask(uint8_t bits);
    int smemcpy(void *dst, int dst_off, uint8_t *src, int size, int nmemb);
    /* Use exceptions...*/

public:
    FileReader(FILE *f);
    int read_error();
    
    int set_input_file(FILE *f);
    int reset_bit_count();
    int read_bits_uint64(uint64_t *x, uint8_t bits);
    int read_bits_uint32(uint32_t *x, uint8_t bits);
    int read_bits_uint16(uint16_t *x, uint8_t bits);
    int read_bits_uint8(uint8_t *x, uint8_t bits);
    
    int read_bits_unary(uint16_t *x);
    int read_utf8_uint64(uint64_t *val);
    int read_utf8_uint32(uint32_t *val);
    
    int read_file(void *buf, int size, int nmemb);
    int reset_file();
};

FileReader::FileReader(FILE *f){
    this->fin = f;
    this->bit = 0;    
    this->bitbuf[0] = 0;
    this->bytes_read = READ_COUNT;
    this->bytes_consumed = READ_COUNT;
    this->eof = 0;
}

int FileReader::read_error(){
    fprintf(stderr, "Error reading file\n");
    fclose(this->fin);
    exit(1);
}

/* Copy from a uint8_t *src into a uintxx_t dest, 
   assuming big endian byte order..
   Note that dst should be zeroed for this to work. */
int FileReader::smemcpy(void *dst, int dst_off, uint8_t *src, int size, int nmemb){
    switch (size){
        case 1:
            memcpy( (uint8_t *)dst + dst_off, src, nmemb * size);
            break;
        case 2:
            memcpy((uint16_t *)dst + dst_off, src, nmemb * size);
            break;
        case 3: case 4:
            memcpy((uint32_t *)dst + dst_off, src, nmemb * size);
            break;
        case 5: case 6: case 7: case 8:
            memcpy((uint64_t *)dst + dst_off, src, nmemb * size);
            break;
    }
    return 1;
}

int FileReader::reset_file(){
    rewind(this->fin);
    this->bitbuf[0] = 0;
    this->bytes_consumed = READ_COUNT;
    this->bytes_read = READ_COUNT;
    this->bit = 0;
    this->eof = 0;
    return 1;
}

int FileReader::read_file(void *dst, int size, int nmemb){
    //fprintf(stderr, "FILEREADER -- Requested read %d of size %d\n", nmemb, size);
    int original_nmemb = nmemb;
    while (nmemb > 0){
        if (this->bytes_consumed >= this->bytes_read){
            this->bytes_read = fread(this->buffer, 1, READ_COUNT, this->fin);
            /* Check whether we have reached EOF or some file reading error */
            if (this->bytes_read != READ_COUNT){
                if (ferror(this->fin)){
                    this->read_error();
                } else if (feof(this->fin)){
                    //fprintf(stderr, "FILEREADER -- Reached EOF\n");
                    this->eof = 1;
                }
            }
            this->bytes_consumed = 0;
        }
        /* We are now guaranteed to have something in the buffer */
        
        int bytes_remaining = this->bytes_read - this->bytes_consumed;
        int dst_index = original_nmemb - nmemb;
        //fprintf(stderr, "FILEREADER -- Dest offset: %d\n", dst_index);
        
        if (bytes_remaining > nmemb*size){
            this->smemcpy(dst, dst_index, this->buffer + this->bytes_consumed, size, nmemb);
            this->bytes_consumed += nmemb*size;
            return original_nmemb;
        } else {
            this->smemcpy(dst, dst_index, this->buffer + this->bytes_consumed, size, bytes_remaining);
            this->bytes_consumed += bytes_remaining;
            nmemb -= (bytes_remaining / size);
        }
        nmemb = 0;
    }
    
    if (this->eof = 1){
        return -1;
    }
    
    return original_nmemb;
}

uint8_t FileReader::get_mask(uint8_t bits){
    switch (bits){
        case 0: return 0xff;
        case 1: return 0x80;
        case 2: return 0xc0;
        case 3: return 0xe0;
        case 4: return 0xf0;
        case 5: return 0xf8;
        case 6: return 0xfc;
        case 7: return 0xfe;
        case 8: return 0xff;
    }
}


int FileReader::reset_bit_count(){
    this->bit = 0;
    this->bitbuf[0] = 0;
    return 1;
}

int FileReader::read_bits_uint64(uint64_t *x, uint8_t bits){
    /* Convert this to big endian */
    int bits_left_in_byte;
    uint64_t t = 0;
    
    while (bits > 0){
        bits_left_in_byte = 8 - (this->bit % 8);
        if (bits_left_in_byte == 8)
            this->read_file(this->bitbuf, 1, 1);
        
        if (bits > bits_left_in_byte){
            t <<= bits_left_in_byte;
            t |= ((((uint64_t)bitbuf[0]) & this->get_mask(bits_left_in_byte)) >> (8-bits_left_in_byte));
            bits -= bits_left_in_byte;
            this->bit += bits_left_in_byte;
        } else {
            t <<= bits;
            t |= ((((uint64_t)bitbuf[0]) & this->get_mask(bits)) >> (8 - bits));
            bitbuf[0] <<= bits;
            this->bit += bits;
            bits = 0;
        }
    }
    
    *x = t;
    return 1;
}

int FileReader::read_bits_uint32(uint32_t *x, uint8_t bits){
    int bits_left_in_byte;
    uint32_t t = 0;
    
    while (bits > 0){
        bits_left_in_byte = 8 - (this->bit % 8);
        if (bits_left_in_byte == 8)
            this->read_file(this->bitbuf, 1, 1);
        
        if (bits > bits_left_in_byte){
            t <<= bits_left_in_byte;
            t |= ((((uint32_t)this->bitbuf[0]) & this->get_mask(bits_left_in_byte)) >> (8-bits_left_in_byte));
            bits -= bits_left_in_byte;
            this->bit += bits_left_in_byte;
        } else {
            t <<= bits;
            t |= ((((uint32_t)this->bitbuf[0]) & this->get_mask(bits)) >> (8 - bits));
            bitbuf[0] <<= bits;
            this->bit += bits;
            bits = 0;
        }
    }
    
    *x = t;
    return 1;
}


int FileReader::read_bits_uint16(uint16_t *x, uint8_t bits){
    int bits_left_in_byte;
    uint16_t t = 0;
    
    while (bits > 0){
        bits_left_in_byte = 8 - (this->bit % 8);
        if (bits_left_in_byte == 8)
            this->read_file(this->bitbuf, 1, 1);
        
        if (bits > bits_left_in_byte){
            t <<= bits_left_in_byte;
            t |= (((uint16_t)this->bitbuf[0] & this->get_mask(bits_left_in_byte)) >> (8-bits_left_in_byte));
            bits -= bits_left_in_byte;
            this->bit += bits_left_in_byte;
        } else {
            t <<= bits;
            t |= ((((uint16_t)this->bitbuf[0]) & this->get_mask(bits)) >> (8 - bits));
            bitbuf[0] <<= bits;
            this->bit += bits;
            bits = 0;
        }
    }
    *x = t;
    return 1;
}


int FileReader::read_bits_uint8(uint8_t *x, uint8_t bits){
    int bits_left_in_byte;
    uint8_t t = 0;
    
    while (bits > 0){
        bits_left_in_byte = 8 - (this->bit % 8);
        if (bits_left_in_byte == 8){
            this->read_file(this->bitbuf, sizeof(uint8_t), 1); 
        }
        
        if (bits > bits_left_in_byte){
            t <<= bits_left_in_byte;
            t |= ((this->bitbuf[0] & this->get_mask(bits_left_in_byte)) >> (8-bits_left_in_byte));
            bits -= bits_left_in_byte;
            this->bit += bits_left_in_byte;
        } else {
            t <<= bits;
            //fprintf(stderr, "Buffer contents: 0x%x\n", this->bitbuf[0]);
            
            t |= ((this->bitbuf[0] & this->get_mask(bits)) >> (8 - bits));
            this->bitbuf[0] <<= bits;
            this->bit += bits;
            bits = 0;
        }
    }
    
    *x = t;
    return 1;
}

int FileReader::read_bits_unary(uint16_t *x){
    int c = 0;
    uint8_t b = 0;
    while (!b){
        this->read_bits_uint8(&b, 1);
        c++;
    }
    
    *x = c - 1;
    return 1;
}



/* This code borrowed from libFLAC */
/* on return, if *val == 0xffffffff then the utf-8 sequence was invalid, but the return value will be true */
int FileReader::read_utf8_uint32(uint32_t *val){
    uint32_t v = 0;
    uint32_t x;
    unsigned i;

    if (!this->read_bits_uint32(&x, 8))
        return 0;
    if(!(x & 0x80)) { /* 0xxxxxxx */
        v = x;
        i = 0;
    }
    else if(x & 0xC0 && !(x & 0x20)) { /* 110xxxxx */
        v = x & 0x1F;
        i = 1;
    }
    else if(x & 0xE0 && !(x & 0x10)) { /* 1110xxxx */
        v = x & 0x0F;
        i = 2;
    }
    else if(x & 0xF0 && !(x & 0x08)) { /* 11110xxx */
        v = x & 0x07;
        i = 3;
    }
    else if(x & 0xF8 && !(x & 0x04)) { /* 111110xx */
        v = x & 0x03;
        i = 4;
    }
    else if(x & 0xFC && !(x & 0x02)) { /* 1111110x */
        v = x & 0x01;
        i = 5;
    }
    else {
        *val = 0xffffffff;
        return 1;
    }
    for( ; i; i--) {
        if (!this->read_bits_uint32(&x, 8))
            return 0;
        if (!(x & 0x80) || (x & 0x40)) { /* 10xxxxxx */
            *val = 0xffffffff;
            return 1;
        }
        v <<= 6;
        v |= (x & 0x3F);
    }
    *val = v;
    return 1;
}

/* on return, if *val == 0xffffffffffffffff then the utf-8 sequence was invalid, but the return value will be true */
int FileReader::read_utf8_uint64(uint64_t *val){
    uint64_t v = 0;
    uint32_t x;
    unsigned i;

    if (!this->read_bits_uint32(&x, 8))
        return 0;
    if(!(x & 0x80)) { /* 0xxxxxxx */
        v = x;
        i = 0;
    }
    else if(x & 0xC0 && !(x & 0x20)) { /* 110xxxxx */
        v = x & 0x1F;
        i = 1;
    }
    else if(x & 0xE0 && !(x & 0x10)) { /* 1110xxxx */
        v = x & 0x0F;
        i = 2;
    }
    else if(x & 0xF0 && !(x & 0x08)) { /* 11110xxx */
        v = x & 0x07;
        i = 3;
    }
    else if(x & 0xF8 && !(x & 0x04)) { /* 111110xx */
        v = x & 0x03;
        i = 4;
    }
    else if(x & 0xFC && !(x & 0x02)) { /* 1111110x */
        v = x & 0x01;
        i = 5;
    }
    else if(x & 0xFE && !(x & 0x01)) { /* 11111110 */
        v = 0;
        i = 6;
    }
    else {
        *val = (uint64_t) 0xffffffffffffffff;
        return 1;
    }
    for( ; i; i--) {
        if (!this->read_bits_uint32(&x, 8))
            return 0;
        if(!(x & 0x80) || (x & 0x40)) { /* 10xxxxxx */
            *val = (uint64_t)0xffffffffffffffff;
            return 1;
        }
        v <<= 6;
        v |= (x & 0x3F);
    }
    *val = v;
    return 1;
}


