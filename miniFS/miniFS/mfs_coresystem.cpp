//
//	mfs_coresystem.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core implementation of Mini File System.
//

#include "mini_file_system.h"


MiniFS::MiniFS()
{

}

MiniFS::~MiniFS()
{

}

int MiniFS::cmd(void)
{


}


/// <summary> ��Ӳ���е�MBR��Ϣ�����ڴ�mbr�� </summary>
void MiniFS::readMBR(void)
{
	fseek(space_fp, 0L, SEEK_SET);
	fread(&mbr, sizeof(MBR), 1, space_fp);
}


/// <summary> ���ڴ��е�mbr��Ϣд��Ӳ�� </summary>
void MiniFS::writeMBR(void) const
{
	fseek(space_fp, 0L, SEEK_SET);
	fwrite(&mbr, sizeof(MBR), 1, space_fp);
}


/// <summary> ��Ӳ���е�CAB��Ϣ�����ڴ�CAB�� </summary>
void MiniFS::readCAB(void)
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}


/// <summary> ���ڴ��е�CAB��Ϣд��Ӳ�� </summary>
void MiniFS::writeCAB(void) const
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}


/// <summary> ��Ӳ���е�FAT��Ϣ�����ڴ�FAT�� </summary>
void MiniFS::readFAT(void)
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}


/// <summary> ���ڴ��е�FAT��Ϣд��Ӳ�� </summary>
void MiniFS::writeFAT(void) const
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}


/// <summary> ��Ӳ����ָ���غŵ���Ϣ����buffer </summary>
/// <param name="cluster"> ָ���غ� </param>
void MiniFS::readCluster(const uint_32 cluster)
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fread(buffer, mbr.cluster_size * 1024, 1, space_fp);
}


/// <summary> ��buffer�е�����д��Ӳ��ָ���غ� </summary>
void MiniFS::writeCluster(const uint_32 cluster) const
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

	cur_cluster  = dir.header.current_dir_entrance;
	remain_file  = dir.header.file_num;
	remain_block = block_num - 1;

	uint_32 idx = cur_cluster;
	while (idx != ECOF) {
		idx = FAT[idx];
		FAT[idx] = 0;
		MfsAlg::BitReset(CAB, mbr.cluster_num, idx);
	}

	uint_32 idx_m = 0;
	uint_32 idx_d = 0;
	dir_buffer.firstclu.header = dir.header;
	while (remain_block > 0 && remain_file > 0) {
		dir_buffer.otherclu.fcb[idx_d] = dir.fcb[idx_m];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}
	fseek(space_fp, cur_cluster * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);

	/*if (remain_block == 0) {
		remain_block = block_num;
		idx_d = 0;
	}

	while (remain_file > 0) {
		if (remain_block == 0) {
			uint_32 empty_room = MfsAlg::BitFindRoom(CAB, mbr.cluster_num);
			MfsAlg::BitSet(CAB, mbr.cluster_num, empty_room);
			FAT[cur_cluster] = empty_room;
			cur_cluster = empty_room;
			fseek(space_fp, cur_cluster * mbr.cluster_size * 1024 - 1, SEEK_SET);
			fwrite(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
			remain_block = block_num;
			idx_d = 0;
		}
		dir_buffer.otherclu.fcb[idx_d] = dir.fcb[idx_m];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}*/
	FAT[cur_cluster] = -1;
}


/// <summary> ���½���Ŀ¼�ļ�д��Ӳ�� </summary>
/// <param name="dir"> Ŀ¼��Ϣ </param>
void MiniFS::newWriteDirectory(const Directory dir) const
{



}



/// <summary> �����ռ� </summary>
/// <return> -1:�ռ����ظ�; 1:�����ɹ� </return>
int MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size) {
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

	if (CAB != NULL) { free(CAB); }
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	for (uint_32 i = 0; i <= mbr.RDF_entrance; i++) {
		MfsAlg::BitSet(CAB, mbr.cluster_num, i);
	}
	writeCAB();
	free(CAB);


	// ���¿���FAT�ڴ�ռ�
	free(FAT);
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	writeFAT();


	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	FAT[mbr.RDF_entrance] = -1;
	fseek(space_fp, mbr.FAT_entrance*mbr.cluster_size * 1024, SEEK_SET);
	fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
	free(FAT);

	Directory creat_directory;
	strcpy(creat_directory.header.name, mbr.space_name);
	creat_directory.header.occupy_cluster_num = 1;
	creat_directory.header.current_dir_entrance = mbr.RDF_entrance;
	creat_directory.header.parent_dir_entrance = mbr.RDF_entrance;
	creat_directory.header.file_num = 0;
	creat_directory.header.create_time = creat_directory.header.modify_time = mbr.create_time;
	creat_directory.header.folder_size = sizeof(DFH);
	creat_directory.fcb = (FCB *)calloc(1, sizeof(FCB));
	fseek(space_fp, mbr.RDF_entrance*mbr.cluster_size * 1024, SEEK_SET);
	rewriteDirectory(creat_directory);
	free(creat_directory.fcb);

	


	fclose(space_fp);
	space_fp = NULL;
	return 1;
}


/// <summary> �򿪿ռ� </summary>
/// <return> -1:��ʧ��; 1:�򿪳ɹ� </return>
int MiniFS::mountSpace(char name[]) {
	space_fp = fopen(name, "r");
	if (space_fp == NULL) return -1;
	fclose(space_fp);
	space_fp = fopen(name, "r+");

	fread(&mbr, sizeof(mbr), 1, space_fp);
	buffer = calloc(mbr.cluster_size, 1024);

	fseek(space_fp, mbr.CAB_entrance*mbr.cluster_size * 1024, SEEK_SET);
	CAB = (uint_8 *)calloc((int)ceil(1.0*mbr.cluster_num / 8), sizeof(uint_8));
	fread(CAB, sizeof(uint_8), (int)ceil(1.0*mbr.cluster_num / 8), space_fp);

	fseek(space_fp, mbr.FAT_entrance*mbr.cluster_size * 1024, SEEK_SET);
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);

	Directory current_directory = readDirectory(mbr.RDF_entrance);
	directory.push_back(current_directory);

	return 1;
}




/// <summary> ��ʽ���ռ� </summary>
/// <param name="cluster_size"> �ļ�ϵͳ���ش�С </param>
/// <return> 1:��ʽ���ɹ�; -1:��ʽ��ʧ�� </return>
int MiniFS::formatSpace(uint_32 cluster_size) {
	// ��дMBR
	mbr.cluster_size     = cluster_size;
	mbr.cluster_num      = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance     = 1;
							// CAB�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 8192 ��
	mbr.FAT_entrance     = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
							// FAT�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 256 ��
	mbr.RDF_entrance     = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
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
	strcpy(directory[0].header.name, mbr.space_name);
	directory[0].header.occupy_cluster_num = 1;
	directory[0].header.current_dir_entrance = mbr.RDF_entrance;
	directory[0].header.parent_dir_entrance = mbr.RDF_entrance;
	directory[0].header.file_num = 0;
	directory[0].header.create_time = mbr.create_time;
	directory[0].header.modify_time = time(NULL);
	directory[0].header.folder_size = sizeof(DFH);
	directory[0].fcb = (FCB *)calloc(1, sizeof(FCB));

	rewriteDirectory(directory[0]);

	// ��д�ļ���������С
	buffer = realloc(buffer, mbr.cluster_size * 1024);
}




int MiniFS::closeSpace(void) {


}

int MiniFS::showDirectory(void) {


}
int MiniFS::copyFile(char filename_1[], char filename_2[], int mode) {


}
int MiniFS::deleteFile(char filename[]) {


}
int MiniFS::displayFile(char filename[]) {


}
int MiniFS::moreDisplayFile(char filename[]) {


}
int MiniFS::showAttribute(char filename[]) {



}


int MiniFS::showHelp(int mode = 0) {


}


int MiniFS::makeDir(char filename[]) {


}


