//
//	mfs_cs_cmd.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file realizes the external input and output command parsing.
//

#include "mini_file_system.h"


inline void MiniFS::printFilePath(void)
{
	printf("Mini-FS:");
	std::vector<Directory>::iterator iter;
	for (iter = directory.begin(); iter != directory.end(); iter++)
		printf("\\%s", (*iter).header.name);
	putchar('>');
}


int MiniFS::cmd(void)
{
	return 1;
}
