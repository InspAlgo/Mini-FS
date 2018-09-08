//
//	mfs_coresystem.h
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core data structures of Mini File System.
//

#ifndef __MFS_CORESYSTEM_H__
#define __MFS_CORESYSTEM_H__



// ΢���ļ�ϵͳ (Mini File System)
class MiniFS {
private:
	MBR				mbr;					// ��������¼ 
	HANDLE			h_space;				// �ռ��� 
	uint_8 *		CAB;					// �ط���λͼ 
	uint_32 *		FAT;					// �ļ������ 
	Directory		current_directory;		// ��ǰ�ļ���Ŀ¼ 
	uint_8 *		buffers;				// �ļ������� 

public:
	MiniFS();
	~MiniFS();

	int cmd(void);				// ���봦�� 
private:
	int createSpace(char name[], int space_size = 1024, int cluster_size = 4);			// �����ռ�
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

	// ��չ����
	int makeDir(char filename[]);
	// �����ļ� �༭�ļ�
};



#endif

