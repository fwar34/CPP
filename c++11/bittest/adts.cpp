#include <iostream>
#include <fstream>

#pragma pack(1)
#if 0
// 这里默认是小端平台，所以字段都是反着定义
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
#else
struct AdtsHeader
{
    uint16_t syncH : 8;
    uint16_t protection : 1;
    uint16_t layer : 2;
    uint16_t id : 1;
    uint16_t syncL : 4;
    uint8_t profile : 2;
    uint8_t sample : 4;
    uint8_t privateBit : 1;
    uint8_t channel : 3;
    uint8_t origin : 1;
    uint8_t home : 1;
};
#endif
#pragma pack()

static void GenAdtsHeader(AdtsHeader *dst)
{
    dst->syncH = 0x12;
    dst->syncL = 0x3;
    // dst->sync = 0xFFF;
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

void GenAdtsHeaderUseArray(char *array, uint32_t sample, uint32_t profile, uint32_t channel)
{
    array[0] = 0xFF; // 高8位
    array[1] = 0xF0; // 低4位
    array[1] |= (0 << 3); // id 
    array[1] |= (0 << 1); // layer
    array[1] |= 0x1; // protection
    array[2] |= ((profile & 0x2) << 6); // profile
    array[2] |= ((sample & 0xF) << 2); // sample
    array[2] |= (1 << 1); // privateBit
    array[2] |= ((channel & 0x4) >> 2); // channel高1位
    array[3] |= ((channel & 0x3) << 6); // channel低2位
    array[3] |= (0 << 5); // origin
    array[3] |= (0 << 4); // home
}


void TestAdtsHeader()
{
    AdtsHeader header = { 0 };
    std::cout << "TestAdtsHeader: " << sizeof(header) << std::endl;
    GenAdtsHeader(&header);

    // std::fstream outfile("./adts.bin", std::fstream::out | std::fstream::binary);
    // outfile.write(reinterpret_cast<char*>(&header), sizeof(header));

    char array[7] = { 0 };
    GenAdtsHeaderUseArray(array, 4, 1, 2);
    std::fstream ofile("./adts2.bin", std::fstream::out | std::fstream::binary);
    ofile.write(reinterpret_cast<char*>(array), sizeof(array) / sizeof(array[0]));
    // ofile.write((char*)array, 7);
}