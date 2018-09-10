﻿//
//	mfs_define.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the definition of basic data structures for Mini File System.
//

#ifndef __MFS_DEFINE_H__
#define __MFS_DEFINE_H__

#include "mini_file_system.h"

typedef signed char int_8;
typedef signed short int_16;
typedef signed int int_32;
typedef signed long long int_64;

typedef signed char int_8;
typedef signed short int_16;
typedef signed int int_32;
typedef signed long long int_64;

typedef unsigned char uint_8;
typedef unsigned short uint_16;
typedef unsigned int uint_32;
typedef unsigned long long uint_64;

/// FAT表文件终止标记 (End Cluster Of File)
#define ECOF (0xffffffff)

///	主引导记录 (Main Boot Record)							[64B]
struct MBR
{
	char space_name[16];	  // 空间名字					[16B]
	uint_32 space_size;		  // 空间容量 (单位: MB)			[4B]
	uint_32 cluster_size;	 // 单簇大小 (单位: KB)			[4B]
	uint_32 cluster_num;	  // 空间簇数目 					[4B]
	uint_32 CAB_entrance;	 // 簇分配位图 入口簇号 			[4B]
	uint_32 FAT_entrance;	 // 文件分配图 入口簇号 			[4B]
	uint_32 RDF_entrance;	 // 根目录文件 入口簇号 			[4B]
	time_t create_time;		  // 空间创建时间 				[8B]
	uint_32 free_cluster_num; // 空闲簇数目 					[4B]
	uint_8 MBR_reserved[12];  // 保留12字节					[12B]
};

// 静态断言:
static_assert(sizeof(MBR) == 64, "MBR占用内存大小出错: 注意边界对齐;");

///	文件控制块 (File Control Block)								[64B]
struct FCB
{
	char name[12];				// 文件名						[12B]
	char extension[4];			// 文件拓展名					[4B]
	uint_8 mark;				// 标志域: 0:文件夹 1:文件		[1B]
	uint_8 attribute;			// 文件属性						[1B]
	uint_8 delete_flag;			// 删除标记: 0:存在 1:删除		[1B]
	uint_8 FCB_reserved_1[5];   // 保留5字节						[5B]
	uint_32 data_entrance;		// 文件数据 入口簇号				[4B]
	uint_32 occupy_cluster_num; // 文件占用簇数目					[4B]
	uint_64 file_size;			// 文件大小 单位B					[8B]
	time_t create_time;			// 文件创建时间					[8B]
	time_t modify_time;			// 文件最后一次修改时间			[8B]
	uint_8 FCB_reserved_2[8];   // 保留8字节						[8B]
};

// 静态断言:
static_assert(sizeof(FCB) == 64, "FCB占用内存大小出错: 注意边界对齐;");

///	目录文件头 (Directory File Header)							[64B]
struct DFH
{
	char name[12];				  // 当前目录文件名				[12B]
	uint_8 DFH_reserved_1[4];	 // 保留4字节					[4B]
	uint_32 occupy_cluster_num;   // 文件夹占用簇数目				[4B]
	uint_32 current_dir_entrance; // 当前目录文件 入口簇号		[4B]
	uint_32 parent_dir_entrance;  // 上一级目录文件 入口簇号		[4B]
	uint_32 file_num;			  // 该目录下文件数目				[4B]
	time_t create_time;			  // 文件夹创建时间				[8B]
	time_t modify_time;			  // 文件夹最后一次修改时间		[8B]
	uint_64 folder_size;		  // 文件夹大小 单位B				[8B]
	uint_8 DFH_reserved[8];		  // 保留8字节					[8B]
};

// 静态断言:
static_assert(sizeof(DFH) == 64, "DFH占用内存大小出错: 注意边界对齐;");

///	目录文件簇 (Directory File Cluster)							[64KB]
union DFC {
	struct
	{
		DFH header;
		FCB fcb[1023];
	} firstclu; // 目录文件第一簇					[64KB]

	struct
	{
		FCB fcb[1024];
	} otherclu; // 目录文件其它簇					[64KB]
};

// 静态断言:
static_assert(sizeof(DFC::firstclu) == 64 * 1024, "DFC的firstclu占用内存大小出错: 注意边界对齐;");
static_assert(sizeof(DFC::otherclu) == 64 * 1024, "DFC的otherclu占用内存大小出错: 注意边界对齐;");
static_assert(sizeof(DFC) == 64 * 1024, "DFC占用内存大小出错: 注意边界对齐;");

///	文件目录 (Directory)
struct Directory
{
	DFH header; // 目录文件头
	FCB *fcb;   // 目录文件信息
};

#endif
