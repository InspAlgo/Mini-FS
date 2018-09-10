//
//	mfs_cs_help.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file implementation Help Module of Mini File System.
//

#include "mini_file_system.h"

#define HELP_OUTPUT(cmd_name, cmd_content, cmd_layout) \
	std::cout.setf(std::ios::left);                    \
	std::cout.fill('.');                               \
	std::cout.width(12);                               \
	std::cout << cmd_name;                             \
	std::cout.fill(' ');                               \
	std::cout.width(18);                               \
	std::cout << cmd_content;                          \
	std::cout << "指令格式: ";                         \
	std::cout.fill(' ');                               \
	std::cout << cmd_layout << std::endl;              \
	std::cout.unsetf(std::ios::left);

#define HELP_DETAIL(cmd_name, cmd_layout, cmd_content, cmd_example) \
	std::cout << cmd_name << std::endl;                             \
	std::cout << "指令格式: " << cmd_layout << std::endl;           \
	std::cout << "指令含义: " << cmd_content << std::endl;          \
	std::cout << "指令示例: " << cmd_example << std::endl;


void MiniFS::showHelp(int mode)
{
	switch (mode)
	{
	case 0:
		HELP_OUTPUT("create ", " 创建空间", "create <name> [size(可选)]");
		HELP_OUTPUT("mount ", " 安装空间", "mount <name>");
		HELP_OUTPUT("fmt ", " 格式化空间", "fmt [size(可选)]");
		HELP_OUTPUT("close ", " 退出空间", "close");
		HELP_OUTPUT("dr ", " 显示目录", "dr");
		HELP_OUTPUT("cp ", " 拷贝文件", "cp <name_old> <name_new>");
		HELP_OUTPUT("dl ", " 删除文件", "dl <name>");
		HELP_OUTPUT("tp ", " 显示文本文件", "tp <name>");
		HELP_OUTPUT("more ", " 分页显示文本文件", "more <name>");
		HELP_OUTPUT("att ", " 显示文件属性", "att <name>");
		HELP_OUTPUT("help ", " 帮助信息", "help [命令名(可选)]");
		break;

	case 1:
		HELP_DETAIL("create", "create <name> [size(可选)]",
					"用 create 命令建立一个新的 Mini-FS 空间",
					"create SName 建立一个名为 SName 大小为 1G 的 Windows 文件作为存储空间 \n\t  create SName 2048 建立一个名为 SName 大小为 2G 的 Windows 文件作为存储空间");
		break;

	case 2:
		HELP_DETAIL("mount", "mount <name>", "用 mount 命令安装要操作的空间",
					"mount SName 在 Mini-FS 中安装(打开)空间 SName，为后续操作进行准备");
		break;

	case 3:
		HELP_DETAIL("fmt", "fmt [size(可选)]", "用 fmt 命令格式化当前的空间",
					"fmt 初始化当前打开的 SName 空间，簇大小设置为 4KB \n\t  fmt 8 初始化当前打开的 SName 空间，簇大小设置为 8KB");
		break;

	case 4:
		HELP_DETAIL("close", "close", "用 close 命令退出 Mini-FS", "close 退出并关闭 Mini-FS 系统");
		break;

	case 5:
		HELP_DETAIL("dr", "dr", "dr 显示当前目录下的文件目录", "");
		break;

	case 6:
		HELP_DETAIL("cp", "cp <name1> <name2>", "用 cp 命令拷贝文件",
					"cp wFName sFName 将文件从 Windows 空间中拷贝至 Mini-FS 空间中 \n\t  cp sFName wFName 将文件从 Mini-FS 空间中拷贝至 Windows 空间中 \n\t  cp sFName1 sFName2 将文件从 Mini-FS 空间中拷贝至 Mini-FS 空间中");
		break;

	case 7:
		HELP_DETAIL("dl", "dl <name>", "用 dl 命令删除空间中的文件",
					"dl sFName 将文件 sFName 从空间中删除");
		break;

	case 8:
		HELP_DETAIL("tp", "tp <name>", "用 tp 命令显示单个文本文件",
					"tp SFName 显示名为 sFName 的文本文件的内容");
		break;

	case 9:
		HELP_DETAIL("more", "more <name>", "用 more 命令分页显示文本文件",
					"more sFName 分页显示名为 sFName 的文本文件的内容");
		break;

	case 10:
		HELP_DETAIL("att", "att <name>", "用 att 命令显示文本属性",
					"att sFName 显示名为 sFName 的文件的文件属性(文件名、拓展名、创建时间、文件大小)");
		break;

	case 11:
		HELP_DETAIL("help", "help [命令名(可选)]", "用 help 命令获取帮助信息",
					"help 显示系统名称及含义 \n\t  help create 显示 create 的指令格式等详细信息");
		break;
	}
}