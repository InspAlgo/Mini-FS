//
//	mfs_algorithm.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the implementation of algorithm library.
//

#include "mini_file_system.h"



/// 仅限本文件使用
static const uint_8	set_mask[8]   = {128,  64,  32,  16,   8,   4,   2,   1};
static const uint_8	reset_mask[8] = {127, 191, 223, 239, 247, 251, 253, 254};


/// <summary> 位操作: 置位1 </summary>
/// <param name="table"> 待置位数组 </param>
/// <param name="size"> 数组有效位数目 </param>
/// <param name="bit_no"> 置1的位置 </param>
/// <return> true:置位成功; false:置位失败 </return>
bool	MfsAlg::BitSet(uint_8 table[], uint_32 size, uint_32 bit_No)
{
	if (bit_No >= size) {
		return false;
	}
	table[bit_No >> 3] |= set_mask[bit_No & 7];
	return true;
}


/// <summary> 位操作: 置零 </summary>
/// <return> true:置零成功; false:置零失败 </return>
bool	MfsAlg::BitReset(uint_8 table[], uint_32 size, uint_32 bit_No)
{
	if (bit_No >= size) {
		return false;
	}
	table[bit_No >> 3] &= reset_mask[bit_No & 7];
	return true;
}


/// <summary> 位操作: 查找0值位号 </summary>
/// <return> true:置零成功; 0xffffffff:无空位置 </return>
uint_32	MfsAlg::BitFindRoom(uint_8 table[], uint_32 size)
{
	bool	flag   = false;
	uint_32	bit_No = 0xffffffff;

	int byte_num = (int)ceil(size*1.0/8.0);
	for (int i = 0; i < byte_num; i++) {
		if (table[i] == 255) {
			continue;
		}
		for (int j = 0; j < 8; j++) {
			if (table[i] & set_mask[j] == 0) {
				bit_No = i * 8 + j;
				flag = true;
				break;
			}
		}
		if (flag) break;
	}

	if (bit_No > size) {
		bit_No = 0xffffffff;
	}
	return bit_No;
}
















