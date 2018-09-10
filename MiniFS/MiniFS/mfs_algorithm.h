//
//	mfs_algorithm.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the algorithm library that Mini File System need.
//

#ifndef __MFS_ALGORITHM_H__
#define __MFS_ALGORITHM_H__

/// 微型文件系统算法库
namespace MfsAlg
{

/// 位操作
// 位操作: 置位1
bool BitSet(uint_8 table[], uint_32 size, uint_32 bit_No);
// 位操作: 置零
bool BitReset(uint_8 table[], uint_32 size, uint_32 bit_No);
// 位操作: 查找0值位号
uint_32 BitFindRoom(uint_8 table[], uint_32 size);

/// 字符串匹配

} // namespace MfsAlg

#endif
