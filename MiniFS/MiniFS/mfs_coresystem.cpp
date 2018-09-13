//
//	mfs_coresystem.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: Basic definition.
//

#include "mini_file_system.h"
#include <Windows.h>

MiniFS::MiniFS()
{
	mount_flag = false;
    SetConsoleTitle(L"Mini-FS");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "Mini File System [�汾 3.2.12]" << std::endl;
	std::cout << "(c) 2018 AlphaBeta����������Ȩ����" << std::endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
}


MiniFS::~MiniFS()
{



}