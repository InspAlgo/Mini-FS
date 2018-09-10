//
//	mfs_cs_diskio.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements reading and writing on hard disk.
//

#include "mini_file_system.h"




/// <summary> 读取硬盘目录文件并加载到内存中 </summary>
/// <param name="dir_entrance"> 指定目录文件硬盘入口簇号 </param>
/// <return> 内存目录结构 </return>
Directory MiniFS::readDirectory(uint_32 dir_entrance) const
{
	Directory	cur_dir;							// 当前读取的文件目录
	DFC			dir_buffer;							// 目录文件缓冲区
	uint_32		cur_cluster = dir_entrance;			// 当前处理的簇号
	uint_32		block_num = mbr.cluster_size * 16;	// 单簇可存fcb块数
	uint_32		remain_file;						// 当前文件夹未读取FCB文件数
	uint_32		remain_block;						// 当前簇未读块数

	fseek(space_fp, dir_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);

	cur_dir.header = dir_buffer.firstclu.header;
	cur_dir.fcb = (FCB *)malloc(cur_dir.header.file_num * sizeof(FCB));

	remain_file = cur_dir.header.file_num;
	remain_block = block_num - 1;

	uint_32 idx_m = 0;
	uint_32 idx_d = 0;
	while (remain_block > 0 && remain_file > 0)
	{
		cur_dir.fcb[idx_m] = dir_buffer.firstclu.fcb[idx_d];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}
	while (remain_file > 0) {
		if (remain_block == 0) {
			cur_cluster = FAT[cur_cluster];
			fseek(space_fp, cur_cluster * mbr.cluster_size * 1024, SEEK_SET);
			fread(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
			remain_block = block_num;
			idx_d = 0;
		}
		cur_dir.fcb[idx_m] = dir_buffer.otherclu.fcb[idx_d];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}

	return cur_dir;
}

/// <summary> 将目录文件重写回硬盘 </summary>
/// <param name="dir"> 指定目录文件 </param>
void MiniFS::rewriteDirectory(const Directory dir) const
{
	uint_32		cur_cluster;						// 当前处理簇号
	uint_32		remain_file;						// 当前文件夹未读取FCB文件数
	uint_32		remain_block;						// 当前簇剩余空块数
	uint_32		block_num = mbr.cluster_size * 16;	// 单簇可存fcb块数
	DFC			dir_buffer;							// 目录文件缓冲区

	remain_file = dir.header.file_num;
	remain_block = block_num - 1;

	cur_cluster = dir.header.current_dir_entrance;
	while (cur_cluster != ECOF) {
		cur_cluster = FAT[cur_cluster];
		FAT[cur_cluster] = 0;
		MfsAlg::BitReset(CAB, mbr.cluster_num, cur_cluster);
	}

	uint_32 idx_m = 0;
	uint_32 idx_d = 0;
	cur_cluster = dir.header.current_dir_entrance;
	dir_buffer.firstclu.header = dir.header;
	while (remain_block > 0 && remain_file > 0) {
		dir_buffer.otherclu.fcb[idx_d] = dir.fcb[idx_m];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}
	fseek(space_fp, cur_cluster*mbr.cluster_size * 1024, SEEK_SET);
	fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
	if (remain_block == 0) {
		remain_block = block_num;
		idx_d = 0;
	}

	while (remain_file > 0) {
		dir_buffer.otherclu.fcb[idx_d] = dir.fcb[idx_m];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
		if (remain_block == 0) {
			cur_cluster = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, cur_cluster);
			FAT[cur_cluster] = cur_cluster;
			cur_cluster = cur_cluster;
			fseek(space_fp, cur_cluster * mbr.cluster_size * 1024, SEEK_SET);
			fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
			remain_block = block_num;
			idx_d = 0;
		}
	}
	if (remain_block != block_num) {
		cur_cluster = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
		MfsAlg::BitSet(CAB, mbr.cluster_num, cur_cluster);
		FAT[cur_cluster] = cur_cluster;
		cur_cluster = cur_cluster;
		fseek(space_fp, cur_cluster * mbr.cluster_size * 1024, SEEK_SET);
		fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
	}

	FAT[cur_cluster] = ECOF;
}

/// <summary> 将新建的目录文件写进硬盘 </summary>
/// <param name="dir"> 目录信息 </param>
void MiniFS::newWriteDirectory(const Directory dir) const
{
	uint_32		cur_cluster;						// 当前处理簇号
	DFC			dir_buffer;							// 目录文件缓冲区

	cur_cluster = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, cur_cluster);
	FAT[cur_cluster] = ECOF;

	dir_buffer.firstclu.header = dir.header;

	fseek(space_fp, cur_cluster*mbr.cluster_size * 1024, SEEK_SET);
	fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
}

