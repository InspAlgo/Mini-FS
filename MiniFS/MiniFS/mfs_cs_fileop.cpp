//
//	mfs_cs_fileop.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements operations on files in space.
//
#pragma warning (disable:4996)
#include "mini_file_system.h"



/// <summary> 显示当前文件目录 </summary>
int MiniFS::showDirectory(void)
{
	Directory current_directory = directory[directory.size() - 1];
	struct tm* time;
	uint_32 dir_num = 0, file_num = 0;

	putchar('\n');
	printf(" Mini-FS:");
	std::vector<Directory>::iterator iter;
	for (iter = directory.begin(); iter != directory.end(); iter++)
		printf("\\%s", (*iter).header.name);
	printf(" 的目录\n\n");

	if (directory.size() > 1) {
		Directory &parent_directory = directory[directory.size() - 2];
		dir_num += 2;
		time = gmtime(&current_directory.header.modify_time);
		printf("%04d/%02d/%02d  %02d:%02d", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
		printf("    <DIR>          .\n");
		time = gmtime(&parent_directory.header.modify_time);
		printf("%04d/%02d/%02d  %02d:%02d", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
		printf("    <DIR>          ..\n");
	}

	for (uint_32 i = 0; i < current_directory.header.file_num; i++){
		if (current_directory.fcb[i].delete_flag == 1)
			continue;
		time = gmtime(&current_directory.fcb[i].modify_time);
		printf("%04d/%02d/%02d  %02d:%02d", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
		if (current_directory.fcb[i].mark == 1) {
			printf("%18lld ", current_directory.fcb[i].file_size);
			file_num++;
		}
		else if (current_directory.fcb[i].mark == 0) {
			printf("    <DIR>          ");
			dir_num++;
		}
		printf("%s\n", current_directory.fcb[i].name);
	}
	printf("            %d 个文件      %d 个目录\n", file_num, dir_num);
	return 1;
}


/// <summary> 树形打印当前文件夹内文件目录 </summary>
void MiniFS::treeDirectory(void)
{
	uint_32 cur_dir_entrance = directory[directory.size() - 1].header.current_dir_entrance;
	std::vector<int> path;
	puts("");
	if (directory[directory.size() - 1].header.file_num == 0){
		puts(" 没有子文件夹");
		return;
	}
	treeRecur(cur_dir_entrance, path);
	path.clear();
}


/// <summary> 输出文件占用簇号 </summary>
/// <return> -1:查找文件失败; 1:输出成功 </return>
int MiniFS::occupyCluster(char filename[]){
	Directory current_directory = directory[directory.size() - 1];

	uint_32 file_num = current_directory.header.file_num;
	for (uint_32 i = 0; i < file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag == 0) {
			uint_32 cur_cluster = current_directory.fcb[i].data_entrance;
			int count_cluster = 0;
			printf("\n %s 占用的全部块号为:\n", filename);
			while (cur_cluster != ECOF){
				count_cluster++;
				if (count_cluster % 20 == 0)
					puts("");
				printf(" %d%c", cur_cluster, FAT[cur_cluster] == ECOF ? '\n' : ',');
			}
			return 1;
		}
	}

	return -1;
}


/// <summary> 改变当前所处文件夹位置 </summary>
/// <return> -1:改变失败; 1:改变成功 </return>
int MiniFS::changeDirectory(char filename[])
{
	Directory current_directory = directory[directory.size() - 1];

	if (strcmp(filename, "..") == 0) {
		if (directory.size() <= 1)
			return 1;
		free(current_directory.fcb);
		directory.erase(directory.end() - 1);
		return 1;
	}
	else if (strcmp(filename, ".") == 0) {
		return 1;
	}

	uint_32 file_num = current_directory.header.file_num;
	for (uint_32 i = 0; i < file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag==0) {
			if (current_directory.fcb[i].mark == 1)
				return -1;
			else {
				Directory next_directory = readDirectory(current_directory.fcb[i].data_entrance);
				directory.push_back(next_directory);
				return 1;
			}
		}
	}
	return -1;
}


/// <summary> 创建文件夹 </summary>
int MiniFS::makeDir(char filename[])
{
	Directory &current_directory = directory[directory.size() - 1];

	for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].mark == 0) {
			return -1;
		}
	}

	current_directory.header.modify_time = time(NULL);
	current_directory.header.file_num++;
	current_directory.header.occupy_cluster_num = current_directory.header.file_num / 1024 + 1;

	uint_32 file_num = current_directory.header.file_num;
	current_directory.fcb = (FCB *)realloc(current_directory.fcb, file_num * sizeof(FCB));
	strcpy(current_directory.fcb[file_num - 1].name, filename);
	current_directory.fcb[file_num - 1].mark = 0;
	current_directory.fcb[file_num - 1].attribute = ATT_ARCHIVE;
	current_directory.fcb[file_num - 1].delete_flag = 0;
	current_directory.fcb[file_num - 1].occupy_cluster_num = 1;
	current_directory.fcb[file_num - 1].file_size = sizeof(DFH);
	current_directory.fcb[file_num - 1].create_time = current_directory.header.modify_time;
	current_directory.fcb[file_num - 1].modify_time = current_directory.header.modify_time;
	uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
	FAT[empty_room] = ECOF;
	mbr.free_cluster_num -= 1;
	current_directory.fcb[file_num - 1].data_entrance = empty_room;

	std::vector<Directory>::reverse_iterator iter;
	uint_32 last_cluster = current_directory.header.current_dir_entrance;
	for (iter = directory.rbegin(); iter != directory.rend(); iter++) {
		Directory &cur_dir = (*iter);
		cur_dir.header.modify_time = current_directory.header.modify_time;
		uint_32 file_num = cur_dir.header.file_num;
		for (uint_32 i = 0; i < file_num; i++) {
			if (cur_dir.fcb[i].data_entrance == last_cluster) {
				cur_dir.fcb[i].modify_time = current_directory.header.modify_time;
				break;
			}
		}
		last_cluster = cur_dir.header.current_dir_entrance;
		rewriteDirectory(cur_dir);
	}

	Directory dir_directory;
	strcpy(dir_directory.header.name, filename);
	dir_directory.header.occupy_cluster_num = 1;
	dir_directory.header.current_dir_entrance = empty_room;
	dir_directory.header.parent_dir_entrance = current_directory.header.current_dir_entrance;
	dir_directory.header.file_num = 0;
	dir_directory.header.create_time = current_directory.header.modify_time;
	dir_directory.header.modify_time = current_directory.header.modify_time;
	dir_directory.header.folder_size = 0;
	dir_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
	newWriteDirectory(dir_directory);
	free(dir_directory.fcb);

	return 1;
}



/// <summary> 创建文件 </summary>
/// <return> -1:创建失败; 1:创建成功 </return>
int MiniFS::createFile(char filename[])
{
	Directory &current_directory = directory[directory.size() - 1];

	for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].mark == 1 && current_directory.fcb[i].delete_flag == 0) {
			return -1;
		}
	}

	current_directory.header.modify_time = time(NULL);
	current_directory.header.file_num++;
	current_directory.header.occupy_cluster_num = current_directory.header.file_num / 1024 + 1;

	uint_32 file_num = current_directory.header.file_num;
	current_directory.fcb = (FCB *)realloc(current_directory.fcb, file_num * sizeof(FCB));
	strcpy(current_directory.fcb[file_num - 1].name, filename);
	current_directory.fcb[file_num - 1].mark = 1;
	current_directory.fcb[file_num - 1].attribute = ATT_ARCHIVE;
	current_directory.fcb[file_num - 1].delete_flag = 0;
	current_directory.fcb[file_num - 1].occupy_cluster_num = 1;
	current_directory.fcb[file_num - 1].file_size = 0;
	current_directory.fcb[file_num - 1].create_time = current_directory.header.modify_time;
	current_directory.fcb[file_num - 1].modify_time = current_directory.header.modify_time;
	uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
	MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
	FAT[empty_room] = ECOF;
	mbr.free_cluster_num -= 1;
	current_directory.fcb[file_num - 1].data_entrance = empty_room;

	std::vector<Directory>::reverse_iterator iter;
	uint_32 last_cluster = current_directory.header.current_dir_entrance;
	for (iter = directory.rbegin(); iter != directory.rend(); iter++) {
		Directory &cur_dir = (*iter);
		cur_dir.header.modify_time = current_directory.header.modify_time;
		uint_32 file_num = cur_dir.header.file_num;
		for (uint_32 i = 0; i < file_num; i++) {
			if (cur_dir.fcb[i].data_entrance == last_cluster) {
				cur_dir.fcb[i].modify_time = current_directory.header.modify_time;
				break;
			}
		}
		last_cluster = cur_dir.header.current_dir_entrance;
		rewriteDirectory(cur_dir);
	}

	return 1;
}



int MiniFS::copyFile(char filename_1[], char filename_2[])
{
	int mode = 0;
	struct stat buf;
	int result = stat(filename_1, &buf);
	std::vector<std::string> path_1, path_2;
	Directory current_directory = readDirectory(mbr.RDF_entrance);

	if (filename_1[1] == ':')
		mode = 1;
	if (filename_2[1] == ':')
		mode = 2;

	if (mode == 1) {
		if (buf.st_mode == 0)
			return -1;
		MfsAlg::cutPath(filename_1, path_1);
		MfsAlg::cutPath(filename_2, path_2);
		for (int i = 1; i < (int)path_2.size(); i++) {
			uint_32 file_num = current_directory.header.file_num;
			bool find = 0;
			for (uint_32 j = 0; j < current_directory.header.file_num; j++) {
				if (current_directory.fcb[j].name == path_2[i] && current_directory.fcb[j].delete_flag == 0 && current_directory.fcb[j].mark == 0) {
					current_directory = readDirectory(current_directory.fcb[j].data_entrance);
					find = 1;
					break;
				}
			}
			if (find == 0)
				return -1;
		}
		if ((S_IFMT & buf.st_mode) != S_IFDIR) {
			current_directory.header.modify_time = time(NULL);
			current_directory.header.file_num++;
			current_directory.header.occupy_cluster_num = current_directory.header.file_num / 1024 + 1;
			current_directory.header.folder_size += sizeof(FCB) + buf.st_size;

			uint_32 file_num = current_directory.header.file_num;
			current_directory.fcb = (FCB *)realloc(current_directory.fcb, file_num * sizeof(FCB));
			const char* name = path_1[path_1.size() - 1].data();
			strcpy(current_directory.fcb[file_num - 1].name, name);
			current_directory.fcb[file_num - 1].mark = 1;
			if ((S_IREAD&buf.st_mode) == S_IREAD)
				current_directory.fcb[file_num - 1].attribute |= ATT_READ;
			if ((S_IWRITE&buf.st_mode) == S_IWRITE)
				current_directory.fcb[file_num - 1].attribute |= ATT_ARCHIVE;
			current_directory.fcb[file_num - 1].delete_flag = 0;
			current_directory.fcb[file_num - 1].occupy_cluster_num = (uint_32)ceil(buf.st_size / mbr.cluster_size);
			current_directory.fcb[file_num - 1].file_size = buf.st_size;
			current_directory.fcb[file_num - 1].create_time = buf.st_atime;
			current_directory.fcb[file_num - 1].modify_time = buf.st_mtime;
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			current_directory.fcb[file_num - 1].data_entrance = empty_room;
			rewriteDirectory(current_directory);
			free(current_directory.fcb);
			path_1.clear();
			path_2.clear();

			FILE* fp = fopen(filename_1, "r");
			uint_32 cur_cluster = empty_room;
			do {
				fread(buffer, mbr.cluster_size * 1024, 1, fp);
				writeCluster(cur_cluster);
				uint_32 next_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
				MfsAlg::BitSet(CAB, mbr.cluster_num, next_room);
				FAT[cur_cluster] = next_room;
				cur_cluster = next_room;
				buf.st_size -= mbr.cluster_size * 1024;
			} while (buf.st_size > 0);
			FAT[cur_cluster] = ECOF;
		}
		else if ((S_IFMT & buf.st_mode) == S_IFDIR) {

		}
	}


	return 1;
}


/// <summary> 显示文件属性 </summary>
/// <return> -1:查找文件失败; 1:显示成功 </return>
int MiniFS::showAttribute(const char filename[])
{
	Directory current_directory = directory[directory.size() - 1];
	struct tm* time;

	puts("");
	if (strcmp(filename, ".") == 0) {
		printf(" %s 文件夹已存放 %lld 字节数据\n", 
					current_directory.header.name,
					current_directory.header.folder_size
		      );

		for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
			if (current_directory.fcb[i].delete_flag == 1)
				continue;
			printf("  %-25s ",current_directory.fcb[i].name);
			if (current_directory.fcb[i].attribute == ATT_ARCHIVE)		printf("存档\n");
			else if (current_directory.fcb[i].attribute == ATT_READ)	printf("只读\n");
			else if (current_directory.fcb[i].attribute == ATT_HIDE)	printf("隐藏\n");
			else if (current_directory.fcb[i].attribute == ATT_SYSTEM)	printf("系统\n");
			else printf("\n");
		}
		return 1;
	}

	for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
		if (current_directory.fcb[i].delete_flag == 1)
			continue;
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag == 0) {
			if (current_directory.fcb[i].mark == 0)
				printf(" 文件夹名:          %s\n", current_directory.fcb[i].name);
			else
				printf(" 文件名:            %s\n", current_directory.fcb[i].name);
			
			printf(" 权限:              ");
			if (current_directory.fcb[i].attribute == ATT_ARCHIVE)		printf("存档\n");
			else if (current_directory.fcb[i].attribute == ATT_READ)	printf("只读\n");
			else if (current_directory.fcb[i].attribute == ATT_HIDE)	printf("隐藏\n");
			else if (current_directory.fcb[i].attribute == ATT_SYSTEM)	printf("系统\n");
			else printf("\n");
			
			time = gmtime(&current_directory.fcb[i].create_time);
			printf(" 创建时间:          %04d/%02d/%02d  %02d:%02d\n", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
			time = gmtime(&current_directory.fcb[i].modify_time);
			printf(" 修改时间:          %04d/%02d/%02d  %02d:%02d\n", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
			printf(" 文件大小:          %d\n", current_directory.fcb[i].file_size);
			return 1;
		}
	}
	return -1;
}


/// <summary> 删除文件 </summary>
/// <return> -1:删除失败; 1:删除成功 </return>
int MiniFS::deleteFile(char filename[])
{
	Directory &current_directory = directory[directory.size() - 1];

	current_directory.header.modify_time = time(NULL);

	uint_32 file_num = current_directory.header.file_num;
	for (uint_32 i = 0; i < file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0) {
			if (current_directory.fcb[i].delete_flag == 1)
				return -1;
			else {
				current_directory.fcb[i].delete_flag = 1;
				break;
			}
		}
	}

	std::vector<Directory>::reverse_iterator iter;
	uint_32 last_cluster = current_directory.header.current_dir_entrance;
	for (iter = directory.rbegin(); iter != directory.rend(); iter++) {
		Directory	&cur_dir = (*iter);
		cur_dir.header.modify_time = current_directory.header.modify_time;
		uint_32 file_num = cur_dir.header.file_num;
		for (uint_32 i = 0; i < file_num; i++) {
			if (cur_dir.fcb[i].data_entrance == last_cluster) {
				cur_dir.fcb[i].modify_time = current_directory.header.modify_time;
				break;
			}
		}
		last_cluster = cur_dir.header.current_dir_entrance;
		rewriteDirectory(cur_dir);
	}

	return 1;
}


///	<summary> 显示回收站内容 </summary>
void MiniFS::showRecycleBin(void)
{
	std::vector<std::string> path;
	path.clear();
	puts("");
	showRBRecur(mbr.RDF_entrance, path);
	path.clear();
}


///	<summary> 递归打印该目录下的所有文件 </summary>
/// <comment> 递归函数: 必须输入正确的目录文件入口 </comment>
void MiniFS::treeRecur(uint_32 dir_entrance, std::vector<int>& path)
{
	Directory dir;
	dir = readDirectory(dir_entrance);
	path.push_back(1);

	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		if (dir.fcb[i].delete_flag == 1)
			continue;

		// 输出本级文件名 
		for (uint_32 blank = 0; blank < path.size()-1; blank++) {
			if (path[blank]==1)
				printf("│  ");
			else
				printf("    ");
		}
		if (i == dir.header.file_num - 1){
			printf("└- %s\n", dir.fcb[i].name);
			path.erase(path.end() - 1);
			path.push_back(0);
		}
		else
			printf("├- %s\n", dir.fcb[i].name);
		// 文件夹递归查找 
		if (dir.fcb[i].mark == 0) {
			treeRecur(dir.fcb[i].data_entrance, path);
		}
	}
	path.erase(path.end() - 1);

	free(dir.fcb);
}


///	<summary> 递归彻底该目录以及该目录下的所有文件 </summary>
/// <comment> 递归函数: 必须输入正确的目录文件入口 </comment>
void MiniFS::delRecur(uint_32 dir_entrance)
{
	Directory dir;
	dir = readDirectory(dir_entrance);

	// 清空该目录下所有文件的CAB、FAT表 
	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		// 文件夹递归清空 
		if (dir.fcb[i].mark == 0) {
			delRecur(dir.fcb[i].data_entrance);
		}

		// 本级数据递归清空
		uint_32 cur_cluster = dir.fcb[i].data_entrance;
		while (cur_cluster != ECOF) {
			uint_32 last_cluster = cur_cluster;
			MfsAlg::BitReset(CAB, mbr.cluster_num, cur_cluster);
			cur_cluster = FAT[cur_cluster];
			FAT[last_cluster] = 0;
		}

	}

	// 清空该目录文件的CAB、FAT表 
	uint_32 cur_cluster = dir.header.current_dir_entrance;
	while (cur_cluster != ECOF) {
		uint_32 last_cluster = cur_cluster;
		MfsAlg::BitReset(CAB, mbr.cluster_num, cur_cluster);
		cur_cluster = FAT[cur_cluster];
		FAT[last_cluster] = 0;
	}

	free(dir.fcb);
}


void MiniFS::showRBRecur(uint_32 dir_entrance, std::vector<std::string>& path)
{
	Directory dir;
	dir = readDirectory(dir_entrance);
	path.push_back(dir.header.name);

	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		if (dir.fcb[i].delete_flag == 0) {	// 存在 
			if (dir.fcb[i].mark == 0) {				// 文件夹
				showRBRecur(dir.fcb[i].data_entrance, path);
			}
			else {									// 文件
				continue;
			}
		}
		else {								// 被删除
			printf(" %-24s Mini-FS:", dir.fcb[i].name);
			for (uint_32 pi = 0; pi < path.size(); pi++) {
				std::cout << "\\" << path[pi];
			}
			puts("\\");
		}
	}

	path.erase(path.end() - 1);
	free(dir.fcb);
}


