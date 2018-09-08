//
//	mfs_coresystem.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core data structures of Mini File System.
//

#ifndef __MFS_CORESYSTEM_H__
#define __MFS_CORESYSTEM_H__



// 微型文件系统 (Mini File System)
class MiniFS {
private:
	MBR				mbr;					// 主引导记录 
	HANDLE			h_space;				// 空间句柄 
	uint_8 *		CAB;					// 簇分配位图 
	uint_32 *		FAT;					// 文件分配表 
	Directory		current_directory;		// 当前文件夹目录 
	uint_8 *		buffers;				// 文件缓冲区 

public:
	MiniFS();
	~MiniFS();

	int cmd(void);				// 输入处理 
private:
	int createSpace(char name[], int space_size = 1024, int cluster_size = 4);			// 创建空间
	int mountSpace(char name[]);
	int formatSpace(int cluster_size = 4);
	int closeSpace(void);

	int showDirectory(void);
	int copyFile(char filename_1[], char filename_2[], int mode);
	int deleteFile(char filename[]);
	int displayFile(char filename[]);
	int moreDisplayFile(char filename[]);
	int showAttribute(char filename[]);
	int showHelp(int mode = 0);

	// 拓展功能
	int makeDir(char filename[]);
	// 创建文件 编辑文件
};



#endif

