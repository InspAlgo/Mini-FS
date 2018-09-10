//
//	mfs_cs_spaceop.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements rewriting control information of space.
//

#include "mini_file_system.h"


/// <summary> 创建空间 </summary>
/// <return> -1:空间名重复; 1:创建成功 </return>
int MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size)
{
	// 判断此位置是否有同名文件
	FILE * fp = fopen(name, "r");
	if (fp != NULL) {
		fclose(fp);
		return -1;
	}

	// 创建空间
	space_fp = fopen(name, "wb+");
	fseek(space_fp, space_size * 1024 * 1024 - 1, SEEK_SET);
	fwrite("\0", 1, 1, space_fp);
	fclose(space_fp);

	// 初始化空间信息
	space_fp = fopen(name, "rb+");

	// 写入mbr
	strcpy(mbr.space_name, name);
	mbr.space_size = space_size;
	mbr.cluster_size = cluster_size;
	mbr.cluster_num = space_size * 1024 / cluster_size;
	mbr.CAB_entrance = 1;
	mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
	mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
	mbr.create_time = time(NULL);
	mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;
	// MBR写回硬盘
	writeMBR();

	// 重新开辟CAB内存空间并写回
	CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	for (uint_32 i = 0; i <= mbr.RDF_entrance; i++) {
		MfsAlg::BitSet(CAB, mbr.cluster_num, i);
	}
	writeCAB();
	free(CAB);

	// 重新开辟FAT内存空间并写回
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	FAT[0] = ECOF;
	for (uint_32 i = mbr.CAB_entrance; i < mbr.FAT_entrance - 1; i++) {
		FAT[i] = i + 1;
	}
	FAT[mbr.FAT_entrance - 1] = ECOF;
	for (uint_32 i = mbr.FAT_entrance; i < mbr.RDF_entrance - 1; i++) {
		FAT[i] = i + 1;
	}
	FAT[mbr.RDF_entrance - 1] = ECOF;
	FAT[mbr.RDF_entrance] = ECOF;
	writeFAT();
	free(FAT);

	// 新建根目录文件并写入硬盘
	Directory creat_directory;
	strcpy(creat_directory.header.name, mbr.space_name);
	creat_directory.header.occupy_cluster_num = 1;
	creat_directory.header.current_dir_entrance = mbr.RDF_entrance;
	creat_directory.header.parent_dir_entrance = mbr.RDF_entrance;
	creat_directory.header.file_num = 0;
	creat_directory.header.create_time = mbr.create_time;
	creat_directory.header.modify_time = mbr.create_time;
	creat_directory.header.folder_size = sizeof(DFH);
	creat_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
	rewriteDirectory(creat_directory);
	free(creat_directory.fcb);

	fclose(space_fp);
	space_fp = NULL;
	return 1;
}

/// <summary> 打开空间 </summary>
/// <return> -1:打开失败; 1:打开成功 </return>
int MiniFS::mountSpace(char name[])
{
	space_fp = fopen(name, "r");
	if (space_fp == NULL) return -1;
	fclose(space_fp);
	space_fp = fopen(name, "rb+");

	readMBR();
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	readCAB();
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	readFAT();
	Directory current_directory = readDirectory(mbr.RDF_entrance);
	directory.push_back(current_directory);

	return 1;
}

/// <summary> 格式化空间 </summary>
/// <param name="cluster_size"> 文件系统单簇大小 </param>
/// <return> 1:格式化成功; -1:格式化失败 </return>
int MiniFS::formatSpace(uint_32 cluster_size)
{
	// 改写MBR
	mbr.cluster_size = cluster_size;
	mbr.cluster_num = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance = 1;
	// CAB文件占用簇数 为 【 簇数 / 单簇大小(KB) / 8192 】
	mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
	// FAT文件占用簇数 为 【 簇数 / 单簇大小(KB) / 256 】
	mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
	mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;

	// MBR写回硬盘
	writeMBR();

	// 重新开辟CAB内存空间并写回
	CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));

	free(CAB);
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	writeCAB();

	// 重新开辟FAT内存空间
	free(FAT);
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	writeFAT();

	// 清空文件目录并将根目录文件写回
	directory.clear();
	Directory format_directory;
	strcpy(format_directory.header.name, mbr.space_name);
	format_directory.header.occupy_cluster_num = 1;
	format_directory.header.current_dir_entrance = mbr.RDF_entrance;
	format_directory.header.parent_dir_entrance = mbr.RDF_entrance;
	format_directory.header.file_num = 0;
	format_directory.header.create_time = mbr.create_time;
	format_directory.header.modify_time = time(NULL);
	format_directory.header.folder_size = sizeof(DFH);
	format_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
	rewriteDirectory(format_directory);
	directory.push_back(format_directory);
	free(format_directory.fcb);

	// 改写文件缓冲区大小
	buffer = realloc(buffer, mbr.cluster_size * 1024);

	return 1;
}

int MiniFS::closeSpace(void)
{
	writeMBR();

	writeCAB();
	free(CAB);

	writeFAT();
	free(FAT);

	free(buffer);
	directory.clear();
	fclose(space_fp);

	return 1;
}
