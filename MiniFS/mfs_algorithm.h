//
//	mfs_algorithm.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the algorithm library that Mini File System need.
//
#pragma once
#ifndef __MFS_ALGORITHM_H__
#define __MFS_ALGORITHM_H__

/// ΢���ļ�ϵͳ�㷨��
namespace MfsAlg {

	/// λ����
			// λ����: ��λ1
	bool	BitSet(uint_8 table[], uint_32 size, uint_32 bit_No);
			// λ����: ����
	bool	BitReset(uint_8 table[], uint_32 size, uint_32 bit_No);
			// λ����: ����0ֵλ��
	uint_32	BitFindRoom(uint_8 table[], uint_32 size);



	/// �ַ���ƥ��
	

}


#endif
