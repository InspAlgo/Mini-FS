//
//	mfs_cs_cmd.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file realizes the external input and output command parsing.
//

#include "mini_file_system.h"

std::string command_array[] = {"create","mount","fmt","dr","cp","dl","att","help","close","end","mkdir"};
int command_distance[11];
typedef std::vector<int> TX;
typedef std::vector<TX> Tmatrix;

// 编辑距离算法 计算输入命令与已有命令参数相似度 从而在输入命令错误时给出推荐输入命令
int ldistance(const std::string source, const std::string target)
{
    //step 1
    int n = source.length();
    int m = target.length();
    if (m == 0)
        return n;
    if (n == 0)
        return m;

    Tmatrix matrix(n + 1);
    for (int i = 0; i <= n; i++)
        matrix[i].resize(m + 1);
    //step 2 Initialize
    for (int i = 1; i <= n; i++)
        matrix[i][0] = i;
    for (int i = 1; i <= m; i++)
        matrix[0][i] = i;
    //step 3
    for (int i = 1; i <= n; i++)
    {
        const char si = source[i - 1];
        //step 4
        for (int j = 1; j <= m; j++)
        {
            const char dj = target[j - 1];
            //step 5
            int cost;
            if (si == dj)
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }
            //step 6
            const int above = matrix[i - 1][j] + 1;
            const int left = matrix[i][j - 1] + 1;
            const int diag = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = std::min(above, std::min(left, diag));
        }
    } //step7
    return matrix[n][m];
}

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
    this->printFilePath();
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
            if (this->createSpace((char *)command_vector[1].data(), 1024, 4) == 1)
                std::cout << "空间创建成功!" << std::endl;
            else
                std::cout << "文件名已存在!" << std::endl;
		}
		else if (command_num == 3)
		{
			std::regex reg_pattern_space_size("^\\+?[1-9][0-9]*$"); // 空间大小必须为非0整数
			if (!std::regex_match(command_vector[2], reg_pattern_space_size))
			{
				std::cout << command_vector[2] << " 不是非零整数!" << std::endl;
				return 1;
			}

			int space_size = atoi((char *)command_vector[2].data());
			if (space_size < 128 || space_size > 2048)
			{
				std::cout << command_vector[2] << " 不能小于 128 或大于 2048!" << std::endl;
                return 1;
			}

            if (this->createSpace((char *)command_vector[1].data(), (uint_32)space_size, 4) == 1)
                std::cout << "空间创建成功!" << std::endl;
            else
                std::cout << "文件名已存在!" << std::endl;
		}
        else if (command_num == 4)
        {
            std::regex reg_pattern_space_size("^\\+?[1-9][0-9]*$");
            if (!std::regex_match(command_vector[2], reg_pattern_space_size))
            {
                std::cout << command_vector[2] << " 不是非零整数!" << std::endl;
                return 1;
            }

            int space_size = atoi((char *)command_vector[2].data());
            if (space_size < 128 || space_size > 2048)
            {
                std::cout << command_vector[2] << " 不能小于 128 或大于 2048!" << std::endl;
                return 1;
            }

            std::regex reg_pattern_cluster_size("^\\+?[1-9][0-9]*$");
            if (!std::regex_match(command_vector[3], reg_pattern_cluster_size))
            {
                std::cout << command_vector[3] << " 不是非零整数!" << std::endl;
                return 1;
            }

            int cluster_size = atoi((char *)command_vector[3].data());
            if ((cluster_size & (cluster_size - 1)) || cluster_size > 64)
            {
                std::cout << command_vector[3] << " 不是建议参数!(应{x|1<=x<=64 且 x为2的幂次方})" << std::endl;
                return 1;
            }

            if (this->createSpace((char *)command_vector[1].data(), (uint_32)space_size, (uint_32)cluster_size) == 1)
                std::cout << "空间创建成功!" << std::endl;
            else
                std::cout << "文件名已存在!" << std::endl;
        }
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: mount space_name; space_name 可以包含路径 ok
	else if (command == "mount")
	{
		if (command_num == 2)
		{
			std::ifstream ifs(command_vector[1].c_str());
			if (ifs)
			{
                if (this->mountSpace((char *)command_vector[1].data()) != 1)
                    std::cout << command_vector[1] << " 打开失败!" << std::endl;
                else
                    this->if_open = true;
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
            this->showDirectory();
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

	// example: help [命令名(可选)];   ok
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

	// example: close;  关闭当前空间  ok
	else if (command == "close")
	{
		if (command_num == 1)
		{
            if (!this->if_open)
            {
                std::cout << "当前没有打开空间!" << std::endl;
                return 1;
            }

			this->closeSpace(); // 需要做一些回写操作
            this->if_open = false;
		}
		else
			std::cout << command << " 参数错误!" << std::endl;
	}

	// example: fmt [size(可选)]; 格式化当前空间  ok
	else if (command == "fmt")
	{
        if (command_num == 1)
        {
            if (!this->if_open)
            {
                std::cout << "当前没有打开空间!" << std::endl;
                return 1;
            }
            this->formatSpace(); // 默认为 4
            std::cout << "格式化成功!" << std::endl;
        }
        else if (command_num == 2)
        {
            std::regex reg_pattern_size("^\\+?[1-9][0-9]*$");  // 非0整数
            if (!std::regex_match(command_vector[1], reg_pattern_size))
            {
                std::cout << command_vector[1] << " 不是非零整数!" << std::endl;
                return 1;
            }

            int cluster_size = atoi((char *)command_vector[1].data());
            if ((cluster_size & (cluster_size - 1)) || cluster_size > 64)
            {
                std::cout << command_vector[1] << " 不是建议参数!(应{x|1<=x<=64 且 x为2的幂次方})" << std::endl;
                return 1;
            }

            this->formatSpace((uint_32)cluster_size);
            std::cout << "格式化成功!" << std::endl;
        }
        else
            std::cout << command << " 参数错误!" << std::endl;
	}

    // example: end; 退出程序  ok
    else if (command == "end")
    {
        if (command_num == 1)
            return 0;
        else
            std::cout << command << " 参数错误!" << std::endl;
    }

    // example: mkdir <name>; 创建名为 name 文件夹
    else if (command == "mkdir")
    {
        if (command_num == 2)
        {
            if (command_vector[1].length() > 8)
            {
                std::cout << "创建" << command_vector[1] << " 失败，文件夹名称过长!" << std::endl;
                return 1;
            }

            if (this->makeDir((char *)command_vector[1].data()) == -1)
                std::cout << command_vector[1] << " 文件夹重名!" << std::endl;
        }
        else
            std::cout << command << " 参数错误!" << std::endl;
    }

    // example: cd <dirname>; 移动到 dirname 文件夹
    else if (command == "cd")
    {
        if (command_num == 2)
        {
           // if (this->changeDirectory((char *)command_vector[1].data()) != 1)
           //     std::cout << command_vector[1] << " 不存在!" << std::endl;
        }
        else
            std::cout << command << " 参数错误!" << std::endl;
    }

    // example: touch filename; 创建 filename 文件
    else if (command == "touch")
    {
        if (command_num == 2)
        {
            if (command_vector[1].length() > 12)
            {
                std::cout << "创建" << command_vector[1] << " 失败，文件名称过长!" << std::endl;
                return 1;
            }

            if (this->createFile((char *)command_vector[1].data()) == -1)
                std::cout << command_vector[1] << " 文件重名!" << std::endl;
        }
        else
            std::cout << command << " 参数错误!" << std::endl;
    }

    else
    {
        std::cout << command << " 不是一个正确的命令!" << std::endl;
        bool if_dis = false;
        for (int i = 0; i < 11; i++)
        {
            command_distance[i]= ldistance(command, command_array[i]);
            if (command_distance[i] < 3)
                if_dis = true;
        }
        if (!if_dis)
            return 1;
        std::cout << "您是想输入 ";
        bool dis_flag = false;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 11; j++)
            {
                if (command_distance[j] == i)
                {
                    std::cout << command_array[j] << " ";
                    if (i <= 0)
                        dis_flag = true;
                }
            }
            if (dis_flag)
                break;
        }
        std::cout << "命令吗?" << std::endl;
    }

	return 1;
}
