//
//	mfs_cs_fileop.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implements operations on files in space.
//
#pragma warning (disable:4996)
#include "mini_file_system.h"



/// <summary> ��ʾ��ǰ�ļ�Ŀ¼ </summary>
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
	printf(" ��Ŀ¼\n\n");

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
	printf("            %d ���ļ�      %d ��Ŀ¼\n", file_num, dir_num);
	return 1;
}


/// <summary> ���δ�ӡ��ǰ�ļ������ļ�Ŀ¼ </summary>
void MiniFS::treeDirectory(void)
{
	uint_32 cur_dir_entrance = directory[directory.size() - 1].header.current_dir_entrance;
	std::vector<int> path;
	puts("");
	if (directory[directory.size() - 1].header.file_num == 0){
		puts(" û�����ļ���");
		return;
	}
	treeRecur(cur_dir_entrance, path);
	path.clear();
}


/// <summary> ����ļ�ռ�ôغ� </summary>
/// <return> -1:�����ļ�ʧ��; 1:����ɹ� </return>
int MiniFS::occupyCluster(char filename[]){
	Directory current_directory = directory[directory.size() - 1];

	uint_32 file_num = current_directory.header.file_num;
	for (uint_32 i = 0; i < file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag == 0) {
			uint_32 cur_cluster = current_directory.fcb[i].data_entrance;
			int count_cluster = 0;
			printf("\n %s ռ�õ�ȫ�����Ϊ:\n", filename);
			while (cur_cluster != ECOF){
				count_cluster++;
				if (count_cluster % 20 == 0)
					puts("");
				printf(" %d%c", cur_cluster, FAT[cur_cluster] == ECOF ? '\n' : ',');
				cur_cluster = FAT[cur_cluster];
			}
			return 1;
		}
	}

	return -1;
}


/// <summary> �ı䵱ǰ�����ļ���λ�� </summary>
/// <return> -1:�ı�ʧ��; 1:�ı�ɹ� </return>
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
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag == 0) {
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


/// <summary> �����ļ��� </summary>
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



/// <summary> �����ļ� </summary>
/// <return> -1:����ʧ��; 1:�����ɹ� </return>
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


/// <summary> �����ļ� </summary>
/// <return> -1:·��������; 1:���Ƴɹ� </return>
int MiniFS::copyFile(char filename_1[], char filename_2[])
{
	int mode = 0;
	struct stat buf;
	std::vector<std::string> path_1, path_2;
	Directory* current_directory = &readDirectory(mbr.RDF_entrance);

	if (filename_1[1] == ':'&&filename_2[1] != ':')
		mode = 1;
	else if (filename_2[1] == ':'&&filename_1[1] != ':')
		mode = 2;
	else if (filename_1[1] != ':'&&filename_2[1] != ':')
		mode = 3;
	else
		return -1;
	MfsAlg::cutPath(filename_1, path_1);
	MfsAlg::cutPath(filename_2, path_2);

	if (mode == 1) {
		stat(filename_1, &buf);
		if (buf.st_mode == 0)
			return -1;
		if (path_2[0] == "")
			path_2.erase(path_2.begin());
		for (int i = 1; i < (int)path_2.size(); i++) {
			uint_32 file_num = current_directory->header.file_num;
			bool find = 0;
			for (uint_32 j = 0; j < current_directory->header.file_num; j++) {
				if (current_directory->fcb[j].name == path_2[i] && current_directory->fcb[j].delete_flag == 0 && current_directory->fcb[j].mark == 0) {
					current_directory = &readDirectory(current_directory->fcb[j].data_entrance);
					find = 1;
					break;
				}
			}
			if (find == 0)
				return -1;
		}
		bool in_path = false;
		for (uint_32 i = 0; i < directory.size(); i++){
			if (current_directory->header.current_dir_entrance == directory[i].header.current_dir_entrance){
				current_directory = &directory[i];
				in_path = true;
				break;
			}
		}
		if ((S_IFDIR & buf.st_mode) != S_IFDIR) {
			const char* name = path_1[path_1.size() - 1].data();
			for (uint_32 i = 0; i < current_directory->header.file_num; i++){
				if (strcmp(name, current_directory->fcb[i].name) == 0 && current_directory->fcb[i].delete_flag == 0 && current_directory->fcb[i].mark == 1)
					return 1;
			}
			current_directory->header.modify_time = time(NULL);
			current_directory->header.file_num++;
			current_directory->header.occupy_cluster_num = current_directory->header.file_num / 1024 + 1;
			current_directory->header.folder_size += buf.st_size;

			uint_32 file_num = current_directory->header.file_num;
			current_directory->fcb = (FCB *)realloc(current_directory->fcb, file_num * sizeof(FCB));
			strcpy(current_directory->fcb[file_num - 1].name, name);
			current_directory->fcb[file_num - 1].mark = 1;
			current_directory->fcb[file_num - 1].attribute = 0;
			if ((S_IREAD&buf.st_mode) == S_IREAD)
				current_directory->fcb[file_num - 1].attribute |= ATT_READ;
			if ((S_IWRITE&buf.st_mode) == S_IWRITE)
				current_directory->fcb[file_num - 1].attribute |= ATT_ARCHIVE;
			current_directory->fcb[file_num - 1].delete_flag = 0;
			current_directory->fcb[file_num - 1].occupy_cluster_num = (uint_32)ceil(buf.st_size / mbr.cluster_size);
			mbr.free_cluster_num -= current_directory->fcb[file_num - 1].occupy_cluster_num;
			current_directory->fcb[file_num - 1].file_size = buf.st_size;
			current_directory->fcb[file_num - 1].create_time = buf.st_atime;
			current_directory->fcb[file_num - 1].modify_time = buf.st_mtime;
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			current_directory->fcb[file_num - 1].data_entrance = empty_room;
			rewriteDirectory(*current_directory);
			if (in_path == false)
				free(current_directory->fcb);

			FILE* fp = fopen(filename_1, "rb");
			uint_32 cur_cluster = empty_room;
			uint_64 file_size = buf.st_size;
			while (file_size > 0){
				if (file_size <= mbr.cluster_size * 1024){
					fread(buffer, (long)file_size, 1, fp);
					writeCluster(cur_cluster);
					file_size = 0;
					break;
				}
				fread(buffer, mbr.cluster_size * 1024, 1, fp);
				writeCluster(cur_cluster);
				file_size -= mbr.cluster_size * 1024;
				uint_32 next_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
				MfsAlg::BitSet(CAB, mbr.cluster_num, next_room);
				FAT[cur_cluster] = next_room;
				cur_cluster = next_room;
			}
			FAT[cur_cluster] = ECOF;

			Directory* parent_directory = &readDirectory(current_directory->header.parent_dir_entrance);
			Directory son_directory = readDirectory(current_directory->header.current_dir_entrance);
			while (son_directory.header.current_dir_entrance != mbr.RDF_entrance){
				for (uint_32 i = 0; i < directory.size(); i++){
					if (parent_directory->header.current_dir_entrance == directory[i].header.current_dir_entrance){
						parent_directory = &directory[i];
						break;
					}
				}
				for (uint_32 i = 0; i < parent_directory->header.file_num; i++){
					if (parent_directory->fcb[i].data_entrance == son_directory.header.current_dir_entrance){
						parent_directory->header.folder_size -= parent_directory->fcb[i].file_size;
						parent_directory->fcb[i].file_size = son_directory.header.folder_size;
						parent_directory->header.folder_size += parent_directory->fcb[i].file_size;
						break;
					}
				}
				rewriteDirectory(*parent_directory);
				if (parent_directory->header.current_dir_entrance == mbr.RDF_entrance)
					break;
				son_directory = *parent_directory;
				parent_directory = &readDirectory(parent_directory->header.parent_dir_entrance);
			}
		}
		else if ((S_IFDIR & buf.st_mode) == S_IFDIR) {
			const char* name = path_1[path_1.size() - 1].data();
			for (uint_32 i = 0; i < current_directory->header.file_num; i++){
				if (strcmp(name, current_directory->fcb[i].name) == 0 && current_directory->fcb[i].delete_flag == 0 && current_directory->fcb[i].mark == 0)
					return 1;
			}
			current_directory->header.modify_time = time(NULL);
			current_directory->header.file_num++;
			current_directory->header.occupy_cluster_num = current_directory->header.file_num / 1024 + 1;

			uint_32 file_num = current_directory->header.file_num;
			current_directory->fcb = (FCB *)realloc(current_directory->fcb, file_num * sizeof(FCB));
			strcpy(current_directory->fcb[file_num - 1].name, name);
			current_directory->fcb[file_num - 1].mark = 0;
			current_directory->fcb[file_num - 1].attribute = ATT_ARCHIVE;
			current_directory->fcb[file_num - 1].delete_flag = 0;
			current_directory->fcb[file_num - 1].occupy_cluster_num = (uint_32)ceil(buf.st_size / mbr.cluster_size);
			current_directory->fcb[file_num - 1].file_size = 0;
			current_directory->fcb[file_num - 1].create_time = buf.st_atime;
			current_directory->fcb[file_num - 1].modify_time = buf.st_mtime;
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			current_directory->fcb[file_num - 1].data_entrance = empty_room;
			FAT[empty_room] = ECOF;
			rewriteDirectory(*current_directory);

			Directory dir_directory;
			strcpy(dir_directory.header.name, name);
			dir_directory.header.occupy_cluster_num = 1;
			dir_directory.header.current_dir_entrance = empty_room;
			dir_directory.header.parent_dir_entrance = current_directory->header.current_dir_entrance;
			dir_directory.header.file_num = 0;
			dir_directory.header.create_time = current_directory->header.modify_time;
			dir_directory.header.modify_time = current_directory->header.modify_time;
			dir_directory.header.folder_size = 0;
			dir_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
			newWriteDirectory(dir_directory);
			free(dir_directory.fcb);

			char next_path[200];
			char next_fspath[200];
			strcpy(next_path, filename_1);
			if (next_path[strlen(next_path) - 1] != '\\')
				strcat(next_path, "\\");
			strcat(next_path, "*.*");
			intptr_t handle;
			_finddata_t find_data;
			handle = _findfirst(next_path, &find_data);
			if (handle == -1)
				return 1;
			do{
				if ((find_data.attrib & _A_SUBDIR) == _A_SUBDIR&&strcmp(find_data.name, ".") == 0 || strcmp(find_data.name, "..") == 0)
					continue;
				else{
					strcpy(next_fspath, filename_2);
					strcat(next_fspath, "\\");
					strcat(next_fspath, name);
					strcpy(next_path, filename_1);
					if (next_path[strlen(next_path) - 1] != '\\')
						strcat(next_path, "\\");
					strcat(next_path, find_data.name);
					copyFile(next_path, next_fspath);
				}
			} while (_findnext(handle, &find_data) == 0);
		}
	}
	else if (mode == 2){
		stat(filename_2, &buf);
		if (buf.st_mode == 0 || ((S_IFDIR & buf.st_mode) != S_IFDIR))
			return -1;
		if (path_1[0] == "")
			path_1.erase(path_1.begin());
		for (int i = 1; i < (int)path_1.size() - 1; i++) {
			uint_32 file_num = current_directory->header.file_num;
			bool find = 0;
			for (uint_32 j = 0; j < current_directory->header.file_num; j++) {
				if (current_directory->fcb[j].name == path_1[i] && current_directory->fcb[j].delete_flag == 0 && current_directory->fcb[j].mark == 0) {
					current_directory = &readDirectory(current_directory->fcb[j].data_entrance);
					find = 1;
					break;
				}
			}
			if (find == 0)
				return -1;
		}

		uint_32 file = ECOF;
		for (uint_32 i = 0; i < current_directory->header.file_num; i++){
			if (current_directory->fcb[i].name == path_1[path_1.size() - 1] && current_directory->fcb[i].delete_flag == 0 && current_directory->fcb[i].mark == 1) {
				file = i;
				break;
			}
		}
		if (file != ECOF){
			uint_32 cur_cluster = current_directory->fcb[file].data_entrance;
			uint_64 file_size = current_directory->fcb[file].file_size;
			char file_name[200];
			strcpy(file_name, filename_2);
			if (file_name[strlen(file_name) - 1] != '\\')
				strcat(file_name, "\\");
			strcat(file_name, current_directory->fcb[file].name);

			FILE * fp = fopen(file_name, "r");
			if (fp != NULL) {
				fclose(fp);
				return -1;
			}

			fp = fopen(file_name, "wb+");
			while (file_size > 0){
				if (file_size <= mbr.cluster_size * 1024){
					readCluster(cur_cluster);
					fwrite(buffer, (long)file_size, 1, fp);
					file_size = 0;
					break;
				}
				readCluster(cur_cluster);
				fwrite(buffer, mbr.cluster_size * 1024, 1, fp);
				file_size -= mbr.cluster_size * 1024;
				cur_cluster = FAT[cur_cluster];
			}
			fclose(fp);
		}
		else{
			uint_32 dir = ECOF;
			for (uint_32 i = 0; i < current_directory->header.file_num; i++){
				if (current_directory->fcb[i].name == path_1[path_1.size() - 1] && current_directory->fcb[i].delete_flag == 0 && current_directory->fcb[i].mark == 0) {
					current_directory = &readDirectory(current_directory->fcb[i].data_entrance);
					dir = i;
					break;
				}
			}
			if (dir == ECOF)
				return -1;
			char dir_name[200];
			char file_name[200];
			strcpy(dir_name, filename_2);
			if (dir_name[strlen(dir_name) - 1] != '\\')
				strcat(dir_name, "\\");
			strcat(dir_name, current_directory->header.name);
			_mkdir(dir_name);
			strcpy(file_name, filename_1);
			if (file_name[strlen(file_name) - 1] != '\\')
				strcat(file_name, "\\");
			for (uint_32 i = 0; i < current_directory->header.file_num; i++){
				strcpy(file_name, filename_1);
				if (file_name[strlen(file_name) - 1] != '\\')
					strcat(file_name, "\\");
				strcat(file_name, current_directory->fcb[i].name);
				copyFile(file_name, dir_name);
			}
		}
	}
	else if (mode == 3){
		Directory* fir_directory = &readDirectory(mbr.RDF_entrance);
		Directory* sec_directory = &readDirectory(mbr.RDF_entrance);
		if (path_1[0] == "")
			path_1.erase(path_1.begin());
		if (path_2[0] == "")
			path_2.erase(path_2.begin());
		for (int i = 1; i < (int)path_1.size() - 1; i++) {
			uint_32 file_num = fir_directory->header.file_num;
			bool find = 0;
			for (uint_32 j = 0; j < fir_directory->header.file_num; j++) {
				if (fir_directory->fcb[j].name == path_1[i] && fir_directory->fcb[j].delete_flag == 0 && fir_directory->fcb[j].mark == 0) {
					fir_directory = &readDirectory(fir_directory->fcb[j].data_entrance);
					find = 1;
					break;
				}
			}
			if (find == 0)
				return -1;
		}
		for (int i = 1; i < (int)path_2.size(); i++) {
			uint_32 file_num = sec_directory->header.file_num;
			bool find = 0;
			for (uint_32 j = 0; j < sec_directory->header.file_num; j++) {
				if (sec_directory->fcb[j].name == path_2[i] && sec_directory->fcb[j].delete_flag == 0 && sec_directory->fcb[j].mark == 0) {
					sec_directory = &readDirectory(sec_directory->fcb[j].data_entrance);
					find = 1;
					break;
				}
			}
			if (find == 0)
				return -1;
		}

		bool in_path = false;
		for (uint_32 i = 0; i < directory.size(); i++){
			if (sec_directory->header.current_dir_entrance == directory[i].header.current_dir_entrance){
				sec_directory = &directory[i];
				in_path = true;
				break;
			}
		}

		uint_32 file = ECOF;
		for (uint_32 i = 0; i < fir_directory->header.file_num; i++){
			if (fir_directory->fcb[i].name == path_1[path_1.size() - 1] && fir_directory->fcb[i].delete_flag == 0 && fir_directory->fcb[i].mark == 1) {
				file = i;
				break;
			}
		}
		if (file != ECOF){
			uint_32 fir_cluster = fir_directory->fcb[file].data_entrance;
			uint_64 file_size = fir_directory->fcb[file].file_size;

			sec_directory->header.modify_time = time(NULL);
			sec_directory->header.file_num++;
			sec_directory->header.occupy_cluster_num = current_directory->header.file_num / 1024 + 1;
			sec_directory->header.folder_size += file_size;

			uint_32 file_num = sec_directory->header.file_num;
			sec_directory->fcb = (FCB *)realloc(sec_directory->fcb, file_num * sizeof(FCB));
			strcpy(sec_directory->fcb[file_num - 1].name, fir_directory->fcb[file].name);
			sec_directory->fcb[file_num - 1].mark = 1;
			sec_directory->fcb[file_num - 1].attribute = fir_directory->fcb[file].attribute;
			sec_directory->fcb[file_num - 1].delete_flag = 0;
			sec_directory->fcb[file_num - 1].occupy_cluster_num = (uint_32)ceil(file_size / mbr.cluster_size);
			mbr.free_cluster_num -= current_directory->fcb[file_num - 1].occupy_cluster_num;
			sec_directory->fcb[file_num - 1].file_size = file_size;
			sec_directory->fcb[file_num - 1].create_time = fir_directory->fcb[file].create_time;
			sec_directory->fcb[file_num - 1].modify_time = fir_directory->fcb[file].modify_time;
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			sec_directory->fcb[file_num - 1].data_entrance = empty_room;
			rewriteDirectory(*sec_directory);
			if (in_path == false)
				free(sec_directory->fcb);

			uint_32 cur_cluster = empty_room;
			while (file_size > 0){
				if (file_size <= mbr.cluster_size * 1024){
					readCluster(fir_cluster);
					writeCluster(cur_cluster);
					file_size = 0;
					break;
				}
				readCluster(fir_cluster);
				writeCluster(cur_cluster);
				file_size -= mbr.cluster_size * 1024;
				uint_32 next_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
				MfsAlg::BitSet(CAB, mbr.cluster_num, next_room);
				FAT[cur_cluster] = next_room;
				cur_cluster = next_room;
				fir_cluster = FAT[fir_cluster];
			}
			FAT[cur_cluster] = ECOF;

			Directory* parent_directory = &readDirectory(sec_directory->header.parent_dir_entrance);
			Directory son_directory = readDirectory(sec_directory->header.current_dir_entrance);
			while (son_directory.header.current_dir_entrance != mbr.RDF_entrance){
				for (uint_32 i = 0; i < directory.size(); i++){
					if (parent_directory->header.current_dir_entrance == directory[i].header.current_dir_entrance){
						parent_directory = &directory[i];
						break;
					}
				}
				for (uint_32 i = 0; i < parent_directory->header.file_num; i++){
					if (parent_directory->fcb[i].data_entrance == son_directory.header.current_dir_entrance){
						parent_directory->header.folder_size -= parent_directory->fcb[i].file_size;
						parent_directory->fcb[i].file_size = son_directory.header.folder_size;
						parent_directory->header.folder_size += parent_directory->fcb[i].file_size;
						break;
					}
				}
				rewriteDirectory(*parent_directory);
				if (parent_directory->header.current_dir_entrance == mbr.RDF_entrance)
					break;
				son_directory = *parent_directory;
				parent_directory = &readDirectory(parent_directory->header.parent_dir_entrance);
			}
		}
		else{
			uint_32 dir = ECOF;
			for (uint_32 i = 0; i < fir_directory->header.file_num; i++){
				if (fir_directory->fcb[i].name == path_1[path_1.size() - 1] && fir_directory->fcb[i].delete_flag == 0 && fir_directory->fcb[i].mark == 0) {
					dir = i;
					break;
				}
			}
			if (dir == ECOF)
				return -1;
			uint_64 file_size = fir_directory->fcb[dir].file_size;
			sec_directory->header.modify_time = time(NULL);
			sec_directory->header.file_num++;
			sec_directory->header.occupy_cluster_num = current_directory->header.file_num / 1024 + 1;

			uint_32 file_num = sec_directory->header.file_num;
			sec_directory->fcb = (FCB *)realloc(sec_directory->fcb, file_num * sizeof(FCB));
			strcpy(sec_directory->fcb[file_num - 1].name, fir_directory->fcb[dir].name);
			sec_directory->fcb[file_num - 1].mark = 0;
			sec_directory->fcb[file_num - 1].attribute = fir_directory->fcb[dir].attribute;
			sec_directory->fcb[file_num - 1].delete_flag = 0;
			sec_directory->fcb[file_num - 1].occupy_cluster_num = (uint_32)ceil(file_size / mbr.cluster_size);
			sec_directory->fcb[file_num - 1].file_size = 0;
			sec_directory->fcb[file_num - 1].create_time = fir_directory->fcb[dir].create_time;
			sec_directory->fcb[file_num - 1].modify_time = fir_directory->fcb[dir].modify_time;
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			sec_directory->fcb[file_num - 1].data_entrance = empty_room;
			FAT[empty_room] = ECOF;
			rewriteDirectory(*sec_directory);

			Directory dir_directory;
			strcpy(dir_directory.header.name, fir_directory->fcb[dir].name);
			dir_directory.header.occupy_cluster_num = 1;
			dir_directory.header.current_dir_entrance = empty_room;
			dir_directory.header.parent_dir_entrance = sec_directory->header.current_dir_entrance;
			dir_directory.header.file_num = 0;
			dir_directory.header.create_time = sec_directory->header.modify_time;
			dir_directory.header.modify_time = sec_directory->header.modify_time;
			dir_directory.header.folder_size = 0;
			dir_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
			newWriteDirectory(dir_directory);
			free(dir_directory.fcb);

			char fir_path[200];
			char next_path[200];
			fir_directory = &readDirectory(fir_directory->fcb[dir].data_entrance);
			for (uint_32 i = 0; i < fir_directory->header.file_num; i++){
				strcpy(fir_path, filename_1);
				if (fir_path[strlen(fir_path) - 1] != '\\')
					strcat(fir_path, "\\");
				strcat(fir_path, fir_directory->fcb[i].name);
				strcpy(next_path, filename_2);
				if (next_path[strlen(next_path) - 1] != '\\')
					strcat(next_path, "\\");
				strcat(next_path, fir_directory->header.name);
				copyFile(fir_path, next_path);
			}
		}
	}

	return 1;
}


/// <summary> �ƶ��ļ� </summary>
/// <return> -1:��ǰ�ڱ��ƶ��ļ���; -2:�ƶ�ʧ��; 1:�ƶ��ɹ� </return>
int MiniFS::moveFile(char filename_1[], char filename_2[]){
	Directory *current_directory = &readDirectory(mbr.RDF_entrance);
	std::vector<std::string> path;

	MfsAlg::cutPath(filename_1, path);
	if (path[0] == "")
		path.erase(path.begin());

	bool in_path = true;
	for (int i = 1; i < (int)path.size()-1; i++) {
		uint_32 file_num = current_directory->header.file_num;
		for (uint_32 j = 0; j < current_directory->header.file_num; j++) {
			if (current_directory->fcb[j].name == path[i] && current_directory->fcb[j].delete_flag == 0 && current_directory->fcb[j].mark == 0) {
				current_directory = &readDirectory(current_directory->fcb[j].data_entrance);
				break;
			}
		}
		if ((int)directory.size() <= i&&in_path == true)
			return -1;
		if (directory[i].header.name != path[i])
			in_path = false;
	}

	if (copyFile(filename_1, filename_2) == -1)
		return -2;

	for (uint_32 i = 0; i < directory.size(); i++){
		if (current_directory->header.current_dir_entrance == directory[i].header.current_dir_entrance)
			current_directory = &directory[i];
	}

	for (uint_32 i = 0; i < current_directory->header.file_num; i++){
		if (current_directory->fcb[i].name == path[path.size() - 1] && current_directory->fcb[i].delete_flag == 0) {
			current_directory->header.folder_size -= current_directory->fcb[i].file_size;
			delRecur(current_directory->fcb[i].data_entrance);
			current_directory->fcb[i] = current_directory->fcb[current_directory->header.file_num - 1];
			current_directory->header.file_num -= 1;
			break;
		}
	}
	return 1;
}


/// <summary> ��ʾ�ļ����� </summary>
/// <return> -1:�����ļ�ʧ��; 1:��ʾ�ɹ� </return>
int MiniFS::showAttribute(const char filename[])
{
	Directory current_directory = directory[directory.size() - 1];
	struct tm* time;

	puts("");
	if (strcmp(filename, ".") == 0) {
		printf(" %s �ļ����Ѵ�� %lld �ֽ�����\n",
			current_directory.header.name,
			current_directory.header.folder_size
			);

		for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
			if (current_directory.fcb[i].delete_flag == 1)
				continue;
			printf("  %-25s ", current_directory.fcb[i].name);
			if ((current_directory.fcb[i].attribute & ATT_ARCHIVE) == ATT_ARCHIVE)		printf("�浵\n");
			else if ((current_directory.fcb[i].attribute & ATT_READ) == ATT_READ)	printf("ֻ��\n");
			else if ((current_directory.fcb[i].attribute & ATT_HIDE) == ATT_HIDE)	printf("����\n");
			else if ((current_directory.fcb[i].attribute & ATT_SYSTEM) == ATT_SYSTEM)	printf("ϵͳ\n");
			else printf("\n");
		}
		return 1;
	}

	for (uint_32 i = 0; i < current_directory.header.file_num; i++) {
		if (current_directory.fcb[i].delete_flag == 1)
			continue;
		if (strcmp(filename, current_directory.fcb[i].name) == 0 && current_directory.fcb[i].delete_flag == 0) {
			if (current_directory.fcb[i].mark == 0)
				printf(" �ļ�����:          %s\n", current_directory.fcb[i].name);
			else
				printf(" �ļ���:            %s\n", current_directory.fcb[i].name);

			printf(" Ȩ��:              ");
			if ((current_directory.fcb[i].attribute & ATT_ARCHIVE) == ATT_ARCHIVE)		printf("�浵\n");
			else if ((current_directory.fcb[i].attribute & ATT_READ) == ATT_READ)	printf("ֻ��\n");
			else if ((current_directory.fcb[i].attribute & ATT_HIDE) == ATT_HIDE)	printf("����\n");
			else if ((current_directory.fcb[i].attribute & ATT_SYSTEM) == ATT_SYSTEM)	printf("ϵͳ\n");
			else printf("\n");

			time = gmtime(&current_directory.fcb[i].create_time);
			printf(" ����ʱ��:          %04d/%02d/%02d  %02d:%02d\n", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
			time = gmtime(&current_directory.fcb[i].modify_time);
			printf(" �޸�ʱ��:          %04d/%02d/%02d  %02d:%02d\n", 1900 + time->tm_year, 1 + time->tm_mon, time->tm_mday, 8 + time->tm_hour, time->tm_min);
			printf(" �ļ���С:          %d\n", current_directory.fcb[i].file_size);
			return 1;
		}
	}
	return -1;
}


/// <summary> ɾ���ļ� </summary>
/// <return> -1:ɾ��ʧ��; 1:ɾ���ɹ� </return>
int MiniFS::deleteFile(char filename[])
{
	Directory &current_directory = directory[directory.size() - 1];

	current_directory.header.modify_time = time(NULL);

	uint_32 file_num = current_directory.header.file_num;
	uint_64 file_size;
	for (uint_32 i = 0; i < file_num; i++) {
		if (strcmp(filename, current_directory.fcb[i].name) == 0) {
			if (current_directory.fcb[i].delete_flag == 1)
				return -1;
			else {
				file_size = current_directory.fcb[i].file_size;
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
		cur_dir.header.folder_size -= file_size;
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



///	<summary> ��ʾ����վ���� </summary>
void MiniFS::showRecycleBin(void)
{
	std::vector<std::string> path;
	bool is_empty = true;
	path.clear();
	puts("");
	showRBRecur(mbr.RDF_entrance, path, is_empty);
	if (is_empty == true)
		puts(" ����վ���ļ�");
	path.clear();
}


///	<summary> ��ջ���վ </summary>
void MiniFS::emptyRecycleBin(void)
{
	emptyRBRecur(mbr.RDF_entrance);
	for (int i = 0; i < (int)directory.size(); i++){
		Directory cur_dir = readDirectory(directory[0].header.current_dir_entrance);
		directory.erase(directory.begin());
		directory.push_back(cur_dir);
	}
}



///	<summary> �ݹ��ӡ��Ŀ¼�µ������ļ� </summary>
/// <comment> �ݹ麯��: ����������ȷ��Ŀ¼�ļ���� </comment>
void MiniFS::treeRecur(uint_32 dir_entrance, std::vector<int>& path)
{
	Directory dir;
	dir = readDirectory(dir_entrance);

	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		if (dir.fcb[i].delete_flag == 1)
			continue;

		// ��������ļ��� 
		for (int blank = 0; blank < (int)path.size(); blank++) {
			if (path[blank] == 1)
				printf("��  ");
			else
				printf("    ");
		}
		bool end_fcb = true;
		for (uint_32 j = i + 1; j < dir.header.file_num; j++){
			if (dir.fcb[j].delete_flag == 0)
				end_fcb = false;
		}
		if (end_fcb == true)
			printf("��- %s\n", dir.fcb[i].name);
		else
			printf("��- %s\n", dir.fcb[i].name);
		// �ļ��еݹ���� 
		if (dir.fcb[i].mark == 0) {
			path.push_back(!end_fcb);
			treeRecur(dir.fcb[i].data_entrance, path);
			path.erase(path.end() - 1);
		}
	}

	free(dir.fcb);
}


///	<summary> �ݹ鳹�׸�Ŀ¼�Լ���Ŀ¼�µ������ļ� </summary>
/// <comment> �ݹ麯��: ����������ȷ��Ŀ¼�ļ���� </comment>
void MiniFS::delRecur(uint_32 dir_entrance)
{
	Directory dir;
	dir = readDirectory(dir_entrance);

	// ��ո�Ŀ¼�������ļ���CAB��FAT�� 
	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		// �ļ��еݹ���� 
		if (dir.fcb[i].mark == 0) {
			delRecur(dir.fcb[i].data_entrance);
		}

		// �������ݵݹ����
		uint_32 cur_cluster = dir.fcb[i].data_entrance;
		while (cur_cluster != ECOF) {
			uint_32 last_cluster = cur_cluster;
			MfsAlg::BitReset(CAB, mbr.cluster_num, cur_cluster);
			cur_cluster = FAT[cur_cluster];
			FAT[last_cluster] = 0;
		}

	}

	free(dir.fcb);
}


void MiniFS::showRBRecur(uint_32 dir_entrance, std::vector<std::string>& path, bool &empty, int mode)
{
	Directory dir;
	dir = readDirectory(dir_entrance);
	path.push_back(dir.header.name);

	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		if (dir.fcb[i].delete_flag == 1 || mode) {					// ��ɾ��
			empty = false;
			printf(" %-24s Mini-FS:", dir.fcb[i].name);
			for (uint_32 pi = 0; pi < path.size(); pi++) {
				std::cout << "\\" << path[pi];
			}
			puts("\\");
			if (dir.fcb[i].mark == 0)
				showRBRecur(dir.fcb[i].data_entrance, path, empty, 1);
		}
	}

	path.erase(path.end() - 1);
	free(dir.fcb);
}




///	<summary> �ݹ���ջ���վ </summary>
/// <comment> �ݹ麯��: �ⲿ������Ŀ¼��ڴغ� </comment>
void MiniFS::emptyRBRecur(uint_32 dir_entrance)
{
	Directory dir;
	dir = readDirectory(dir_entrance);

	for (uint_32 i = 0; i < dir.header.file_num; i++) {
		if (dir.fcb[i].delete_flag == 1){
			if (dir.fcb[i].mark == 0) {		// �ļ���
				delRecur(dir.fcb[i].data_entrance);
				mbr.free_cluster_num += dir.fcb[i].occupy_cluster_num;
				dir.fcb[i] = dir.fcb[dir.header.file_num - 1];
				dir.header.file_num--;
				i--;
			}
			else {							// �ļ�
				uint_32 cur_cluster = dir.fcb[i].data_entrance;
				while (cur_cluster != ECOF) {
					uint_32 last_cluster = cur_cluster;
					MfsAlg::BitReset(CAB, mbr.cluster_num, cur_cluster);
					cur_cluster = FAT[cur_cluster];
					FAT[last_cluster] = 0;
				}
				mbr.free_cluster_num += dir.fcb[i].occupy_cluster_num;
				dir.header.folder_size -= dir.fcb[i].file_size;
				dir.fcb[i] = dir.fcb[dir.header.file_num - 1];
				dir.header.file_num--;
				i--;
			}
		}
		else if (dir.fcb[i].mark == 0)
			emptyRBRecur(dir.fcb[i].data_entrance);
	}
	rewriteDirectory(dir);

	free(dir.fcb);
}

