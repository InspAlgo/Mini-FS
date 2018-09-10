//
//	mfs_coresystem.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core data structures of Mini File System.
//

#ifndef __MFS_CORESYSTEM_H__
#define __MFS_CORESYSTEM_H__


///	微型文件系统 (Mini File System)
class MiniFS
{
private:
	MBR mbr;													// 主引导记录
	FILE *space_fp;										// 当前空间(文件)指针
	uint_8 *CAB;											// 簇分配位图
	uint_32 CAB_occupu_byte;					// CAB占用字节数
	uint_32 *FAT;											// 文件分配表
	std::vector<Directory> directory; // 文件目录
	void *buffer;											// 空间输入输出缓冲区
    bool if_open;   // 判断空间是否打开
public:
	MiniFS();
	~MiniFS();

	//	命令解析模块			<mfs_cs_cmd.cpp>
public:
	int cmd(void);

private:
	inline void printFilePath(void);

	//	帮助系统模块			<mfs_cs_help.cpp>
private:
	void showHelp(int mode = 0);

	//	硬盘读写模块			<mfs_cs_diskio.cpp>
private:
	inline void readMBR(void);
	inline void writeMBR(void) const;
	inline void readCAB(void);
	inline void writeCAB(void) const;
	inline void readFAT(void);
	inline void writeFAT(void) const;
	inline void readCluster(const uint_32 cluster);
	inline void writeCluster(const uint_32 cluster) const;
	Directory readDirectory(const uint_32 dir_entrance) const;
	void rewriteDirectory(const Directory dir) const;
	void newWriteDirectory(const Directory dir) const;

	//	空间操作模块			<mfs_cs_spaceop.cpp>
private:
	int createSpace(char name[], uint_32 space_size = 1024, uint_32 cluster_size = 4);
	int mountSpace(char name[]);
	int formatSpace(uint_32 cluster_size = 4);
	int closeSpace(void);

	//	文件操作模块			<mfs_cs_fileop.cpp>
private:
	int showDirectory(void);
	int copyFile(char filename_1[], char filename_2[], int mode);
	int deleteFile(char filename[]);
	int displayFile(char filename[]);
	int moreDisplayFile(char filename[]);
	int showAttribute(char filename[]);
	int makeDir(char filename[]);
	int createFile(char filename[]);
};


/// <summary> 将硬盘中的MBR信息读入内存mbr中 </summary>
inline void MiniFS::readMBR(void)
{
    fseek(space_fp, 0L, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> 将内存中的mbr信息写回硬盘 </summary>
inline void MiniFS::writeMBR(void) const
{
    fseek(space_fp, 0L, SEEK_SET);
    fwrite(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> 将硬盘中的CAB信息读入内存CAB中 </summary>
inline void MiniFS::readCAB(void)
{
    fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
    fread(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> 将内存中的CAB信息写回硬盘 </summary>
inline void MiniFS::writeCAB(void) const
{
    fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
    fwrite(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> 将硬盘中的FAT信息读入内存FAT中 </summary>
inline void MiniFS::readFAT(void)
{
    fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
    fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> 将内存中的FAT信息写回硬盘 </summary>
inline void MiniFS::writeFAT(void) const
{
    fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
    fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> 将硬盘中指定簇号的信息读入buffer </summary>
/// <param name="cluster"> 指定簇号 </param>
inline void MiniFS::readCluster(const uint_32 cluster)
{
    fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
    fread(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

/// <summary> 将buffer中的内容写回硬盘指定簇号 </summary>
inline void MiniFS::writeCluster(const uint_32 cluster) const
{
    fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
    fwrite(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

#endif
