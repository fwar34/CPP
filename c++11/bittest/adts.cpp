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

int adts_header(char * const p_adts_header, const int data_length,
                const int profile, const int samplerate,
                const int channels, const int sample)
{
    int sampling_frequency_index = 3; // 默认使用48000hz
    int adtsLen = data_length + 7;


    p_adts_header[0] = 0xff;         //syncword:0xfff                          高8bits
    p_adts_header[1] = 0xf0;         //syncword:0xfff                          低4bits
    p_adts_header[1] |= (0 << 3);    //MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    p_adts_header[1] |= (0 << 1);    //Layer:0                                 2bits
    p_adts_header[1] |= 1;           //protection absent:1                     1bit

    p_adts_header[2] = (profile)<<6;            //profile:profile               2bits
    p_adts_header[2] |= (sample & 0x0f)<<2; //sampling frequency index:sampling_frequency_index  4bits
    p_adts_header[2] |= (0 << 1);             //private bit:0                   1bit
    p_adts_header[2] |= (channels & 0x04)>>2; //channel configuration:channels  高1bit

    p_adts_header[3] = (channels & 0x03)<<6; //channel configuration:channels 低2bits
    p_adts_header[3] |= (0 << 5);               //original：0                1bit
    p_adts_header[3] |= (0 << 4);               //home：0                    1bit
    p_adts_header[3] |= (0 << 3);               //copyright id bit：0        1bit
    p_adts_header[3] |= (0 << 2);               //copyright id start：0      1bit
    p_adts_header[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits

    p_adts_header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    p_adts_header[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
    p_adts_header[5] |= 0x1f;                                 //buffer fullness:0x7ff 高5bits
    p_adts_header[6] = 0xfc;      //‭11111100‬       //buffer fullness:0x7ff 低6bits
    // number_of_raw_data_blocks_in_frame：
    //    表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧。

    return 0;
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

    std::fstream outfile("./adts.bin", std::fstream::out | std::fstream::binary);
    outfile.write(reinterpret_cast<char*>(&header), sizeof(header));

    char array[7] = { 0 };
    GenAdtsHeaderUseArray(array, 4, 1, 2);
    std::fstream ofile("./adts2.bin", std::fstream::out | std::fstream::binary);
    // ofile.write(reinterpret_cast<char*>(array), sizeof(array) / sizeof(array[0]));
    ofile.write((char*)array, 7);

    char harray[7] = { 0 };
    adts_header(harray, 32, 3, 4, 2, 2);
    std::fstream outfile2("./adts_header.bin", std::fstream::out | std::fstream::binary);
    outfile2.write(harray, 7);
}