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
	char delim = ' ';
	std::string input_temp = "";
	std::string command_input = "";

	// 每次只读取一行命令，多次输入由外界循环控制
	std::getline(std::cin, command_input);

	// 防止空输入
	if ((command_input[0] == ' ' && command_input.length() == 1) || command_input.length() < 1)
		return 1;

	// 去除多余的空格
	bool space_flag = true;
	for (int i = 0; i < command_input.length(); i++)
	{
		if (command_input[i] == ' ')
		{
			if (space_flag)
				input_temp += command_input[i];
			space_flag = false;
		}
		else
		{
			space_flag = true;
			input_temp += command_input[i];
		}
	}
	if (input_temp[0] == ' ')
		command_input = input_temp.substr(1);
	else
		command_input = input_temp;

	// 防止空输入
	if ((command_input[0] == ' ' && command_input.length() == 1) || command_input.length() < 1)
		return 1;

	// 按空格 ' ' 切分输入的一行字符串
	std::vector<std::string> command_vector;
	command_vector.clear();
	std::istringstream iss(command_input);
	std::string temp;

	while (std::getline(iss, temp, delim))
		command_vector.emplace_back(std::move(temp));

	int command_num = command_vector.size();
	std::string command = command_vector[0];

	int handle_re; // 接收各命令具体处理函数的返回值

	// example: create space_name; space_name 可以包含路径
	if (command == "create")
	{
		if (command_num == 2)
		{
			std::regex reg_pattern("^\\w+$"); // 当文件名为英文、数字和下划线时合法
			if (std::regex_match(command_vector[1], reg_pattern))
			{
				this->createSpace((char *)command_vector[1].data(), 1024, 4);
				std::cout << "YES!" << std::endl;
			}
			else
				std::cout << command_vector[1] << " 非法文件名!" << std::endl;
		}
		else if (command_num == 3)
		{
			bool flag = true;

			std::regex reg_pattern_name("^\\w+$"); // 当文件名为英文、数字和下划线时合法
			if (!std::regex_match(command_vector[1], reg_pattern_name))
			{
				std::cout << command_vector[1] << " 非法文件名!" << std::endl;
				flag = false;
			}

			std::regex reg_pattern_size("^\\+?[1-9][0-9]*$"); // 空间大小必须为非0整数
			if (!std::regex_match(command_vector[2], reg_pattern_size))
			{
				std::cout << command_vector[2] << " 不是非零整数!" << std::endl;
				return 1;
			}

			int space_size = atoi((char *)command_vector[2].data());
			if (space_size < 128)
			{
				std::cout << command_vector[2] << " 不能小于 128!" << std::endl;
				flag = false;
			}

			if (!flag)
				return 1;

			this->createSpace((char *)command_vector[1].data(), space_size, 4);
			std::cout << "YES!" << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: mount space_name; space_name 可以包含路径
	else if (command == "mount")
	{
		if (command_num == 2)
		{
			std::ifstream ifs(command_vector[1].c_str());
			if (ifs)
			{
				std::cout << command_vector[1] << " 存在" << std::endl;
				this->mountSpace((char *)command_vector[1].data());
			}
			else
				std::cout << command_vector[1] << " 不存在此路径!" << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: dr; 显示当前文件夹下的文件目录
	else if (command == "dr")
	{
		if (command_num == 1)
		{
			this->showDirectory();
			std::cout << command << " YES!" << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: cp filename_old filename_new;
	// 有从外部系统->Mini-FS、Mini-FS->外部系统、Mini-FS->Mini-FS三种情况
	else if (command == "cp")
	{
		if (command_num == 3)
		{
			// 判断 filename_old filename_new 的合法性
			std::cout << command_vector[1] << command_vector[2] << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: dl filename; 删除空间中的文件
	else if (command == "dl")
	{
		if (command_num == 2)
		{
			// 判断 filename 的合法性
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: tp filename; 显示单个文本文件
	else if (command == "tp")
	{
		if (command_num == 2)
		{
			// 判断 filename 的合法性
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: more filename; 分页显示文本文件
	else if (command == "more")
	{
		if (command_num == 2)
		{
			// 判断 filename 的合法性
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: att filename; 显示文件属性
	else if (command == "att")
	{
		if (command_num == 2)
		{
			// 判断 filename 的合法性
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: help [命令名(可选)];
	else if (command == "help")
	{
		if (command_num == 1)
		{
			this->showHelp(0);
		}
		else if (command_num == 2)
		{
			if (command_vector[1] == "create")
				this->showHelp(1);
			else if (command_vector[1] == "mount")
				this->showHelp(2);
			else if (command_vector[1] == "fmt")
				this->showHelp(3);
			else if (command_vector[1] == "close")
				this->showHelp(4);
			else if (command_vector[1] == "dr")
				this->showHelp(5);
			else if (command_vector[1] == "cp")
				this->showHelp(6);
			else if (command_vector[1] == "dl")
				this->showHelp(7);
			else if (command_vector[1] == "tp")
				this->showHelp(8);
			else if (command_vector[1] == "more")
				this->showHelp(9);
			else if (command_vector[1] == "att")
				this->showHelp(10);
			else if (command_vector[1] == "help")
				this->showHelp(11);
			else
				std::cout << command_vector[1] << " 不是一个正确的命令名称!" << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: close;  关闭当前空间
	else if (command == "close")
	{
		if (command_num == 1)
		{
			this->closeSpace(); // 需要做一些回写操作
			return 0;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: fmt [size(可选)]; 格式化当前空间
	else if (command == "fmt")
	{
		if (command_num == 1)
		{
			this->formatSpace(); // 默认为 4
			std::cout << command << " YES!" << std::endl;
		}
		else if (command_num == 2)
		{
			if (command_vector[1] == "4")
			{
				this->formatSpace();
				std::cout << command_vector[1] << " YES!" << std::endl;
			}
			else if (command_vector[1] == "8")
			{
				this->formatSpace((uint_32)8);
				std::cout << command_vector[1] << " YES!" << std::endl;
			}
			else
				std::cout << command_vector[1] << " 不是推荐的参数!" << std::endl;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	else
		std::cout << command << " 不是一个正确的命令!" << std::endl;

	return 1;
}
