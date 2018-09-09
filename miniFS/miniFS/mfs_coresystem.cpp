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


/// <summary> 将硬盘中的MBR信息读入内存mbr中 </summary>
void MiniFS::readMBR(void)
{
	fseek(space_fp, 0L, SEEK_SET);
	fread(&mbr, sizeof(MBR), 1, space_fp);
}


/// <summary> 将内存中的mbr信息写回硬盘 </summary>
void MiniFS::writeMBR(void) const
{
	fseek(space_fp, 0L, SEEK_SET);
	fwrite(&mbr, sizeof(MBR), 1, space_fp);
}


/// <summary> 将硬盘中的CAB信息读入内存CAB中 </summary>
void MiniFS::readCAB(void)
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}


/// <summary> 将内存中的CAB信息写回硬盘 </summary>
void MiniFS::writeCAB(void) const
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}


/// <summary> 将硬盘中的FAT信息读入内存FAT中 </summary>
void MiniFS::readFAT(void)
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}


/// <summary> 将内存中的FAT信息写回硬盘 </summary>
void MiniFS::writeFAT(void) const
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}


/// <summary> 将硬盘中指定簇号的信息读入buffer </summary>
/// <param name="cluster"> 指定簇号 </param>
void MiniFS::readCluster(const uint_32 cluster)
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fread(buffer, mbr.cluster_size * 1024, 1, space_fp);
}


/// <summary> 将buffer中的内容写回硬盘指定簇号 </summary>
void MiniFS::writeCluster(const uint_32 cluster) const
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(buffer, mbr.cluster_size * 1024, 1, space_fp);
}


/// <summary> 将buffer中的内容写回硬盘指定簇号 </summary>
/// <param name="cluster"> 指定目录文件硬盘入口簇号 </param>
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


void MiniFS::rewriteDirectory(const Directory dir) const
{
	uint_32 cur_cluster = dir.header.current_dir_entrance;
	uint_32 remain_file;			// 当前文件夹未读取FCB文件数
	uint_32 remain_block;			// 当前簇剩余空块数
	





}



int	MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size) {
	


}



int MiniFS::mountSpace(char name[]) {


}



/// <summary> 格式化空间 </summary>
/// <param name="cluster_size"> 文件系统单簇大小 </param>
/// <return> 1:格式化成功; -1:格式化失败 </return>
int MiniFS::formatSpace(uint_32 cluster_size) {
	// 改写MBR
	


/// <summary> 格式化空间 </summary>
/// <param name="cluster_size"> 文件系统单簇大小 </param>
/// <return> 1:格式化成功; -1:格式化失败 </return>
int MiniFS::formatSpace(uint_32 cluster_size) {
	// 改写MBR
	mbr.cluster_size     = cluster_size;
	mbr.cluster_num      = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance     = 1;
							// CAB文件占用簇数 为 【 簇数 / 单簇大小(KB) / 8192 】
	mbr.FAT_entrance     = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
							// FAT文件占用簇数 为 【 簇数 / 单簇大小(KB) / 256 】
	mbr.RDF_entrance     = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
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

	// 改写文件缓冲区大小
	buffer = realloc(buffer, mbr.cluster_size * 1024);
}

	mbr.cluster_size     = cluster_size;
	mbr.cluster_num      = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance     = 1;
							// CAB文件占用簇数 为 【 簇数 / 单簇大小(KB) / 8192 】
	mbr.FAT_entrance     = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0*mbr.cluster_size));
							// FAT文件占用簇数 为 【 簇数 / 单簇大小(KB) / 256 】
	mbr.RDF_entrance     = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0*mbr.cluster_size));
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

	// 改写文件缓冲区大小
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


