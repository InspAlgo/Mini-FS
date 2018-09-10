//
//	mfs_cs_fileop.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements operations on files in space.
//

#include "mini_file_system.h"


int MiniFS::showDirectory(void)
{
	Directory current_directory = directory[directory.size() - 1];
	for (uint_32 i = 0; i < current_directory.header.file_num; i++)
		printf("%s\n", current_directory.fcb[i].name);
	return 1;
}

int MiniFS::copyFile(char filename_1[], char filename_2[], int mode)
{
	return 1;
}

int MiniFS::deleteFile(char filename[])
{
	return 1;
}

int MiniFS::displayFile(char filename[])
{
	return 1;
}

int MiniFS::moreDisplayFile(char filename[])
{
	return 1;
}

int MiniFS::showAttribute(char filename[])
{
	return 1;
}

int MiniFS::makeDir(char filename[])
{
	Directory current_directory = directory[directory.size() - 1];
	directory.erase(directory.end() - 1);
	current_directory = readDirectory(current_directory.header.current_dir_entrance);

	current_directory.header.modify_time = time(NULL);
	current_directory.header.file_num++;
	current_directory.header.folder_size += sizeof(FCB) + sizeof(DFH);

	uint_32 file_num = current_directory.header.file_num;
	current_directory.fcb = (FCB *)realloc(current_directory.fcb, file_num * sizeof(FCB));
	strcpy(current_directory.fcb[file_num - 1].name, filename);
	current_directory.fcb[file_num - 1].mark = 0;
	current_directory.fcb[file_num - 1].occupy_cluster_num = 1;
	current_directory.fcb[file_num - 1].file_size = sizeof(DFH);
	current_directory.fcb[file_num - 1].create_time = time(NULL);
	current_directory.fcb[file_num - 1].modify_time = time(NULL);
	uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
	FAT[empty_room] = ECOF;
	current_directory.fcb[file_num - 1].data_entrance = empty_room;

	rewriteDirectory(current_directory);
	free(current_directory.fcb);
	directory.push_back(current_directory);

	Directory dir_directory;
	strcpy(dir_directory.header.name, filename);
	dir_directory.header.occupy_cluster_num = 1;
	dir_directory.header.current_dir_entrance = empty_room;
	dir_directory.header.parent_dir_entrance = empty_room;
	dir_directory.header.file_num = 0;
	dir_directory.header.create_time = time(NULL);
	dir_directory.header.modify_time = time(NULL);
	dir_directory.header.folder_size = sizeof(DFH);
	dir_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
	newWriteDirectory(dir_directory);

	return 1;
}

/// <summary> 创建文件 </summary>
/// <return> -1:创建失败; 1:创建成功 </return>
int MiniFS::createFile(char filename[])
{
	Directory current_directory = directory[directory.size() - 1];
	directory.erase(directory.end() - 1);
	current_directory = readDirectory(current_directory.header.current_dir_entrance);

	current_directory.header.modify_time = time(NULL);
	current_directory.header.file_num++;
	current_directory.header.folder_size += sizeof(FCB);

	uint_32 file_num = current_directory.header.file_num;
	current_directory.fcb = (FCB *)realloc(current_directory.fcb, file_num * sizeof(FCB));
	strcpy(current_directory.fcb[file_num - 1].name, filename);
	current_directory.fcb[file_num - 1].mark = 1;
	current_directory.fcb[file_num - 1].occupy_cluster_num = 1;
	current_directory.fcb[file_num - 1].file_size = 0;
	current_directory.fcb[file_num - 1].create_time = time(NULL);
	current_directory.fcb[file_num - 1].modify_time = time(NULL);
	uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
	FAT[empty_room] = ECOF;
	current_directory.fcb[file_num - 1].data_entrance = empty_room;

	rewriteDirectory(current_directory);
	free(current_directory.fcb);
	directory.push_back(current_directory);

	return 1;
}
