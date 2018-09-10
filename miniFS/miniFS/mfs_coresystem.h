//
//	mfs_coresystem.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core data structures of Mini File System.
//
#pragma once
#ifndef __MFS_CORESYSTEM_H__
#define __MFS_CORESYSTEM_H__


///	΢���ļ�ϵͳ (Mini File System)
class MiniFS {
private:
	MBR						mbr;				// ��������¼ 
	FILE *					space_fp;			// ��ǰ�ռ�(�ļ�)ָ�� 
	uint_8 *				CAB;				// �ط���λͼ 
	uint_32					CAB_occupu_byte;	// CABռ���ֽ��� 
	uint_32 *				FAT;				// �ļ������ 
	std::vector<Directory>	directory;			// �ļ�Ŀ¼ 
	void *					buffer;				// �ռ�������������� 

public:
	MiniFS();
	~MiniFS();

//	�������ģ��			<mfs_cs_cmd.cpp>
public:
	int			cmd(void);
private:
	inline void	printFilePath(void);


//	����ϵͳģ��			<mfs_cs_help.cpp>
private:
	void		showHelp(int mode = 0);


//	Ӳ�̶�дģ��			<mfs_cs_diskio.cpp>
private:
	inline void	readMBR(void);
	inline void	writeMBR(void) const;
	inline void	readCAB(void);
	inline void	writeCAB(void) const;
	inline void	readFAT(void);
	inline void	writeFAT(void) const;
	inline void	readCluster(const uint_32 cluster);
	inline void	writeCluster(const uint_32 cluster) const;
	Directory	readDirectory(const uint_32 dir_entrance) const;
	void		rewriteDirectory(const Directory dir) const;
	void		newWriteDirectory(const Directory dir) const;


//	�ռ����ģ��			<mfs_cs_spaceop.cpp>
private:
	int			createSpace(char name[], uint_32 space_size = 1024, uint_32 cluster_size = 4);
	int			mountSpace(char name[]);
	int			formatSpace(uint_32 cluster_size = 4);
	int			closeSpace(void);


//	�ļ�����ģ��			<mfs_cs_fileop.cpp>
private:
	int			showDirectory(void);
	int			copyFile(char filename_1[], char filename_2[], int mode);
	int			deleteFile(char filename[]);
	int			displayFile(char filename[]);
	int			moreDisplayFile(char filename[]);
	int			showAttribute(char filename[]);
	int			makeDir(char filename[]);
	int			createFile(char filename[]);
};



#endif