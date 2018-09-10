//
//	mfs_cs_spaceop.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements rewriting control information of space.
//

#include "mini_file_system.h"


/// <summary> �����ռ� </summary>
/// <return> -1:�ռ����ظ�; 1:�����ɹ� </return>
int MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size)
{
	// �жϴ�λ���Ƿ���ͬ���ļ�
	FILE * fp = fopen(name, "r");
	if (fp != NULL) {
		fclose(fp);
		return -1;
	}

	// �����ռ�
	space_fp = fopen(name, "wb+");
	fseek(space_fp, space_size * 1024 * 1024 - 1, SEEK_SET);
	fwrite("\0", 1, 1, space_fp);
	fclose(space_fp);

	// ��ʼ���ռ���Ϣ
	space_fp = fopen(name, "rb+");

	// д��mbr
	strcpy(mbr.space_name, name);
	mbr.space_size = space_size;
	mbr.cluster_size = cluster_size;
	mbr.cluster_num = space_size * 1024 / cluster_size;
	mbr.CAB_entrance = 1;
	mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
	mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
	mbr.create_time = time(NULL);
	mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;
	// MBRд��Ӳ��
	writeMBR();

	// ���¿���CAB�ڴ�ռ䲢д��
	CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	for (uint_32 i = 0; i <= mbr.RDF_entrance; i++) {
		MfsAlg::BitSet(CAB, mbr.cluster_num, i);
	}
	writeCAB();
	free(CAB);

	// ���¿���FAT�ڴ�ռ䲢д��
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

	// �½���Ŀ¼�ļ���д��Ӳ��
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

/// <summary> �򿪿ռ� </summary>
/// <return> -1:��ʧ��; 1:�򿪳ɹ� </return>
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

/// <summary> ��ʽ���ռ� </summary>
/// <param name="cluster_size"> �ļ�ϵͳ���ش�С </param>
/// <return> 1:��ʽ���ɹ�; -1:��ʽ��ʧ�� </return>
int MiniFS::formatSpace(uint_32 cluster_size)
{
	// ��дMBR
	mbr.cluster_size = cluster_size;
	mbr.cluster_num = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance = 1;
	// CAB�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 8192 ��
	mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
	// FAT�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 256 ��
	mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
	mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;

	// MBRд��Ӳ��
	writeMBR();

	// ���¿���CAB�ڴ�ռ䲢д��
	CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));

	free(CAB);
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	writeCAB();

	// ���¿���FAT�ڴ�ռ�
	free(FAT);
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	writeFAT();

	// ����ļ�Ŀ¼������Ŀ¼�ļ�д��
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

	// ��д�ļ���������С
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
