//
//	mfs_coresystem.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core implementation of Mini File System.
//

#include "mini_file_system.h"

#define HELP_OUTPUT(cmd_name, cmd_content, cmd_layout)  \
    std::cout.setf(std::ios::left);\
    std::cout.fill('.');\
    std::cout.width(12);\
    std::cout << cmd_name;\
    std::cout.fill(' ');\
    std::cout.width(18);\
    std::cout << cmd_content;\
    std::cout << "指令格式: ";\
    std::cout.fill(' ');\
    std::cout << cmd_layout << std::endl;\
    std::cout.unsetf(std::ios::left);

#define HELP_DETAIL(cmd_name, cmd_layout, cmd_content, cmd_example) \
    std::cout << cmd_name << std::endl;\
    std::cout << "指令格式: " << cmd_layout << std::endl;\
    std::cout << "指令含义: " << cmd_content << std::endl;\
    std::cout << "指令示例: " << cmd_example << std::endl;

MiniFS::MiniFS() {

}

MiniFS::~MiniFS() {

}

int MiniFS::cmd(void) {
    char delim = ' ';
    std::string input_temp = "";
    std::string command_input = "";

    // 每次只读取一行命令，多次输入由外界循环控制
    std::getline(std::cin, command_input);  

    // 防止空输入
    if ((command_input[0] == ' ' && command_input.length() == 1) 
        || command_input.length() < 1)
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
    if ((command_input[0] == ' ' && command_input.length() == 1) 
        || command_input.length() < 1)
        return 1;

    // 按空格 ' ' 切分输入的一行字符串
    std::vector<std::string>command_vector;
    command_vector.clear();
    std::istringstream iss(command_input);
    std::string temp;

    while (std::getline(iss, temp, delim))
        command_vector.emplace_back(std::move(temp));

    int command_num = command_vector.size();
    std::string command = command_vector[0];

    int handle_re;  // 接收各命令具体处理函数的返回值

    // example: create space_name; space_name 可以包含路径
    if (command == "create")  
    {
        if (command_num == 2)
        {
            std::regex reg_pattern("^\\w+$");  // 当文件名为英文、数字和下划线时合法
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

            std::regex reg_pattern_name("^\\w+$");  // 当文件名为英文、数字和下划线时合法
            if (!std::regex_match(command_vector[1], reg_pattern_name))
            {
                std::cout << command_vector[1] << " 非法文件名!" << std::endl;
                flag = false;
            }

            std::regex reg_pattern_size("^\\+?[1-9][0-9]*$");  // 空间大小必须为非0整数
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
        }
        else if (command_num == 2)
        {
            if (command_vector[1] == "create")
            {
                HELP_DETAIL("create", "create <name> [size(可选)]", 
                    "用 create 命令建立一个新的 Mini-FS 空间", 
                    "create SName 建立一个名为 SName 大小为 1G 的 Windows 文件作为存储空间 \n\t  create SName 2048 建立一个名为 SName 大小为 2G 的 Windows 文件作为存储空间");
            }
            else if (command_vector[1] == "mount")
            {
                HELP_DETAIL("mount", "mount <name>", "用 mount 命令安装要操作的空间", 
                    "mount SName 在 Mini-FS 中安装(打开)空间 SName，为后续操作进行准备");
            }
            else if (command_vector[1] == "fmt")
            {
                HELP_DETAIL("fmt", "fmt [size(可选)]", "用 fmt 命令格式化当前的空间",
                    "fmt 初始化当前打开的 SName 空间，簇大小设置为 4KB \n\t  fmt 8 初始化当前打开的 SName 空间，簇大小设置为 8KB");
            }
            else if (command_vector[1] == "close")
            {
                HELP_DETAIL("close", "close", "用 close 命令退出 Mini-FS", "close 退出并关闭 Mini-FS 系统");
            }
            else if (command_vector[1] == "dr")
            {
                HELP_DETAIL("dr", "dr", "dr 显示当前目录下的文件目录", "");
            }
            else if (command_vector[1] == "cp")
            {
                HELP_DETAIL("cp", "cp <name1> <name2>", "用 cp 命令拷贝文件", 
                    "cp wFName sFName 将文件从 Windows 空间中拷贝至 Mini-FS 空间中 \n\t  cp sFName wFName 将文件从 Mini-FS 空间中拷贝至 Windows 空间中 \n\t  cp sFName1 sFName2 将文件从 Mini-FS 空间中拷贝至 Mini-FS 空间中");
            }
            else if (command_vector[1] == "dl")
            {
                HELP_DETAIL("dl", "dl <name>", "用 dl 命令删除空间中的文件", 
                    "dl sFName 将文件 sFName 从空间中删除");
            }
            else if (command_vector[1] == "tp")
            {
                HELP_DETAIL("tp", "tp <name>", "用 tp 命令显示单个文本文件", 
                    "tp SFName 显示名为 sFName 的文本文件的内容");
            }
            else if (command_vector[1] == "more")
            {
                HELP_DETAIL("more", "more <name>", "用 more 命令分页显示文本文件", 
                    "more sFName 分页显示名为 sFName 的文本文件的内容");
            }
            else if (command_vector[1] == "att")
            {
                HELP_DETAIL("att", "att <name>", "用 att 命令显示文本属性", 
                    "att sFName 显示名为 sFName 的文件的文件属性(文件名、拓展名、创建时间、文件大小)");
            }
            else if (command_vector[1] == "help")
            {
                HELP_DETAIL("help", "help [命令名(可选)]", "用 help 命令获取帮助信息", 
                    "help 显示系统名称及含义 \n\t  help create 显示 create 的指令格式等详细信息");
            }
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
            this->closeSpace();  // 需要做一些回写操作
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
            this->formatSpace();  // 默认为 4
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
                this->formatSpace(8);
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

int MiniFS::createSpace(char name[], int space_size, int cluster_size) {


    return 1;
}
int MiniFS::mountSpace(char name[]) {

    return 1;
}
int MiniFS::formatSpace(int cluster_size) {

    return 1;
}
int MiniFS::closeSpace(void) {

    return 1;
}

int MiniFS::showDirectory(void) {

    return 1;
}
int MiniFS::copyFile(char filename_1[], char filename_2[], int mode) {

    return 1;
}
int MiniFS::deleteFile(char filename[]) {

    return 1;
}
int MiniFS::displayFile(char filename[]) {

    return 1;
}
int MiniFS::moreDisplayFile(char filename[]) {

    return 1;
}
int MiniFS::showAttribute(char filename[]) {

    return 1;

}


int MiniFS::showHelp(int mode) {

    return 1;
}


int MiniFS::makeDir(char filename[]) {

    return 1;
}


