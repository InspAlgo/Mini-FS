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

#endif
