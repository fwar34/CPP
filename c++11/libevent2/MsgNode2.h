#pragma once

// 使用可扩展的 buffer，在读写指针相等（buffer 数据读完了）的时候充值读写指针为 0，
// 或者读写指针相减（buffer 内数据剩余长度）小于一个阈值的时候将剩余的数据使用 
// memmove 移动到 buffer 起始位置，并且设置相应的读写指针位置
class MsgNode
{

};