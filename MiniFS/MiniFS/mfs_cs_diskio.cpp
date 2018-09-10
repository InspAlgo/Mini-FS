//
//	mfs_cs_diskio.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements reading and writing on hard disk.
//

#include "mini_file_system.h"


/// <summary> ��Ӳ���е�MBR��Ϣ�����ڴ�mbr�� </summary>
inline void MiniFS::readMBR(void)
{
	fseek(space_fp, 0L, SEEK_SET);
	fread(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> ���ڴ��е�mbr��Ϣд��Ӳ�� </summary>
inline void MiniFS::writeMBR(void) const
{
	fseek(space_fp, 0L, SEEK_SET);
	fwrite(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> ��Ӳ���е�CAB��Ϣ�����ڴ�CAB�� </summary>
inline void MiniFS::readCAB(void)
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> ���ڴ��е�CAB��Ϣд��Ӳ�� </summary>
inline void MiniFS::writeCAB(void) const
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> ��Ӳ���е�FAT��Ϣ�����ڴ�FAT�� </summary>
inline void MiniFS::readFAT(void)
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> ���ڴ��е�FAT��Ϣд��Ӳ�� </summary>
inline void MiniFS::writeFAT(void) const
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> ��Ӳ����ָ���غŵ���Ϣ����buffer </summary>
/// <param name="cluster"> ָ���غ� </param>
inline void MiniFS::readCluster(const uint_32 cluster)
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fread(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

/// <summary> ��buffer�е�����д��Ӳ��ָ���غ� </summary>
inline void MiniFS::writeCluster(const uint_32 cluster) const
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

/// <summary> ��ȡӲ��Ŀ¼�ļ������ص��ڴ��� </summary>
/// <param name="dir_entrance"> ָ��Ŀ¼�ļ�Ӳ����ڴغ� </param>
/// <return> �ڴ�Ŀ¼�ṹ </return>
Directory MiniFS::readDirectory(uint_32 dir_entrance) const
{
	Directory	cur_dir;							// ��ǰ��ȡ���ļ�Ŀ¼
	DFC			dir_buffer;							// Ŀ¼�ļ�������
	uint_32		cur_cluster = dir_entrance;			// ��ǰ����Ĵغ�
	uint_32		block_num = mbr.cluster_size * 16;	// ���ؿɴ�fcb����
	uint_32		remain_file;						// ��ǰ�ļ���δ��ȡFCB�ļ���
	uint_32		remain_block;						// ��ǰ��δ������

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

/// <summary> ��Ŀ¼�ļ���д��Ӳ�� </summary>
/// <param name="dir"> ָ��Ŀ¼�ļ� </param>
void MiniFS::rewriteDirectory(const Directory dir) const
{
	uint_32		cur_cluster;						// ��ǰ����غ�
	uint_32		remain_file;						// ��ǰ�ļ���δ��ȡFCB�ļ���
	uint_32		remain_block;						// ��ǰ��ʣ��տ���
	uint_32		block_num = mbr.cluster_size * 16;	// ���ؿɴ�fcb����
	DFC			dir_buffer;							// Ŀ¼�ļ�������

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

/// <summary> ���½���Ŀ¼�ļ�д��Ӳ�� </summary>
/// <param name="dir"> Ŀ¼��Ϣ </param>
void MiniFS::newWriteDirectory(const Directory dir) const
{
	uint_32		cur_cluster;						// ��ǰ����غ�
	DFC			dir_buffer;							// Ŀ¼�ļ�������

	cur_cluster = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, cur_cluster);
	FAT[cur_cluster] = ECOF;

	dir_buffer.firstclu.header = dir.header;

	fseek(space_fp, cur_cluster*mbr.cluster_size * 1024, SEEK_SET);
	fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
}

