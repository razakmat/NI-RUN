#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <cstdint>

using namespace std;

uint8_t readInt8_t(unsigned char * code, uint32_t index);

uint16_t readInt16_t(unsigned char * code, uint32_t index);

uint32_t readInt32_t(unsigned char * code, uint32_t index);

void readString(unsigned char * code, uint32_t index, uint32_t size, string & res);

void writeInt8_t(unsigned char * code,uint8_t value);

void writeInt16_t(unsigned char * code,uint16_t value);

void writeInt32_t(unsigned char * code,uint32_t value);

void writeString(unsigned char * code,const string & name);



#endif // __UTILS_H__