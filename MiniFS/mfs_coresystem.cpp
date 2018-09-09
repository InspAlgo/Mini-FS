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


/// <summary> ��buffer�е�����д��Ӳ��ָ���غ� </summary>
/// <param name="cluster"> ָ��Ŀ¼�ļ�Ӳ����ڴغ� </param>
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


void MiniFS::rewriteDirectory(const Directory dir) const
{
	uint_32 cur_cluster = dir.header.current_dir_entrance;
	uint_32 remain_file;			// ��ǰ�ļ���δ��ȡFCB�ļ���
	uint_32 remain_block;			// ��ǰ��ʣ��տ���
	





}



int	MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size) {
	


}



int MiniFS::mountSpace(char name[]) {


}



/// <summary> ��ʽ���ռ� </summary>
/// <param name="cluster_size"> �ļ�ϵͳ���ش�С </param>
/// <return> 1:��ʽ���ɹ�; -1:��ʽ��ʧ�� </return>
int MiniFS::formatSpace(uint_32 cluster_size) {
	// ��дMBR
	


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


