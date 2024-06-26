#include <iostream>

#pragma pack(1)
struct AdtsHeader
{
    uint16_t sync : 12;
    uint16_t id : 1;
    uint16_t layer : 2;
    uint16_t protection : 1;
    uint8_t profile : 2;
    uint8_t sample : 4;
    uint8_t privateBit : 1;
    uint8_t channel : 3;
    uint8_t origin : 1;
    uint8_t home : 1;
};
#pragma pack()

static void GenAdtsHeader(AdtsHeader *dst)
{
    dst->sync = 0xFFF;
    dst->id = 0;
    dst->layer = 0;
    dst->protection = 1;
    dst->profile = 1;
    dst->sample = 4;
    dst->privateBit = 1;
    dst->channel = 2;
    dst->origin = 0;
    dst->home = 0;
}

void TestAdtsHeader()
{
    AdtsHeader header = { 0 };
    std::cout << sizeof(header) << std::endl;
    GenAdtsHeader(&header);
}