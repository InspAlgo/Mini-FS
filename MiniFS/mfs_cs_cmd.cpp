//
//	mfs_cs_cmd.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	Class MiniFS: This file realizes the external input and output command parsing.
//

#include "mini_file_system.h"

std::string command_array[] = { "create", "mount", "fmt", "dr", "cp", "dl", "att", "help", "close", "end", "mkdir" ,"tree", "move", "lrb", "crb" };
int command_distance[15];

void MiniFS::printFilePath(void)
{
	printf("\nMini-FS:");
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

	// ÿ��ֻ��ȡһ�����������������ѭ������
	std::getline(std::cin, command_input);

	// ��ֹ������
	if ((command_input[0] == ' ' && command_input.length() == 1) || command_input.length() < 1)
		return 1;

	// ȥ������Ŀո�
	bool space_flag = true;
	for (int i = 0; i < (int)command_input.length(); i++)
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

	// ��ֹ������
	if ((command_input[0] == ' ' && command_input.length() == 1) || command_input.length() < 1)
		return 1;

	// ���ո� ' ' �з������һ���ַ���
	std::vector<std::string> command_vector;
	command_vector.clear();
	std::istringstream iss(command_input);
	std::string temp;

	while (std::getline(iss, temp, delim))
		command_vector.emplace_back(std::move(temp));

	int command_num = command_vector.size();
	std::string command = command_vector[0];

	// example: create space_name; space_name ���԰���·��   ok
	if (command == "create")
	{
		if (command_num == 2)
		{
			if (this->createSpace((char *)command_vector[1].data(), 1024, 4) == 1)
				std::cout << "�ռ䴴���ɹ�!" << std::endl;
			else
				std::cout << "�ռ��Ѵ���!" << std::endl;
		}
		else if (command_num == 3)
		{
			std::regex reg_pattern_space_size("^\\+?[1-9][0-9]*$"); // �ռ��С����Ϊ��0����
			if (!std::regex_match(command_vector[2], reg_pattern_space_size))
			{
				std::cout << command_vector[2] << " ���Ƿ�������!" << std::endl;
				return 1;
			}

			int space_size = atoi((char *)command_vector[2].data());
			if (space_size < 128 || space_size > 2048)
			{
				std::cout << command_vector[2] << " ����С�� 128 ����� 2048!" << std::endl;
				return 1;
			}

			if (this->createSpace((char *)command_vector[1].data(), (uint_32)space_size, 4) == 1)
				std::cout << "�ռ䴴���ɹ�!" << std::endl;
			else
				std::cout << "�ռ��Ѵ���!" << std::endl;
		}
		else if (command_num == 4)
		{
			std::regex reg_pattern_space_size("^\\+?[1-9][0-9]*$");
			if (!std::regex_match(command_vector[2], reg_pattern_space_size))
			{
				std::cout << command_vector[2] << " ���Ƿ�������!" << std::endl;
				return 1;
			}

			int space_size = atoi((char *)command_vector[2].data());
			if (space_size < 128 || space_size > 2048)
			{
				std::cout << command_vector[2] << " ����С�� 128 ����� 2048!" << std::endl;
				return 1;
			}

			std::regex reg_pattern_cluster_size("^\\+?[1-9][0-9]*$");
			if (!std::regex_match(command_vector[3], reg_pattern_cluster_size))
			{
				std::cout << command_vector[3] << " ���Ƿ�������!" << std::endl;
				return 1;
			}

			int cluster_size = atoi((char *)command_vector[3].data());
			if ((cluster_size & (cluster_size - 1)) || cluster_size > 64)
			{
				std::cout << command_vector[3] << " ���ǽ������!(Ӧ{x|1<=x<=64 �� xΪ2���ݴη�})" << std::endl;
				return 1;
			}

			if (this->createSpace((char *)command_vector[1].data(), (uint_32)space_size, (uint_32)cluster_size) == 1)
				std::cout << "�ռ䴴���ɹ�!" << std::endl;
			else
				std::cout << "�ռ��Ѵ���!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: mount space_name; space_name ���԰���·�� ok
	else if (command == "mount")
	{
		if (command_num == 2)
		{
			std::ifstream ifs(command_vector[1].c_str());
			if (ifs)
			{
				if (this->mountSpace((char *)command_vector[1].data()) != 1)
					std::cout << command_vector[1] << " ��ʧ��!" << std::endl;
				else
					this->mount_flag = true;
			}
			else
				std::cout << command_vector[1] << " �����ڴ�·��!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: dr; ��ʾ��ǰ�ļ����µ��ļ�Ŀ¼  ok
	else if (command == "dr")
	{
		if (command_num == 1) {
			if (!this->mount_flag)
			{
				std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
				return 1;
			}
			this->showDirectory();
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: cp filename_old filename_new;  ok
	// �д��ⲿϵͳ->Mini-FS��Mini-FS->�ⲿϵͳ��Mini-FS->Mini-FS�������
	else if (command == "cp")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 3)
		{
			if (this->copyFile((char *)command_vector[1].data(), (char *)command_vector[2].data()) != 1)
				std::cout << "�����ļ�ʧ��!" << std::endl;
			else
				std::cout << "�ɹ������ļ�!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: dl filename; ɾ���ռ��е��ļ�  ok
	else if (command == "dl")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 2)
		{
			if (this->deleteFile((char *)command_vector[1].data()) != 1)
				std::cout << "ɾ���ļ�ʧ��!" << std::endl;
			else
				std::cout << "�ɹ�ɾ���ļ�!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: att filename; ��ʾ�ļ�����  ok
	else if (command == "att")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 2)
		{
			if (this->showAttribute((char *)command_vector[1].data()) != 1)
				std::cout << "�����ļ�ʧ��!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: help [������(��ѡ)];   ok
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
            else if (command_vector[1] == "tree")
                this->showHelp(8);
            else if (command_vector[1] == "move")
                this->showHelp(9);
            else if (command_vector[1] == "att")
                this->showHelp(10);
            else if (command_vector[1] == "help")
                this->showHelp(11);
            else if (command_vector[1] == "end")
                this->showHelp(12);
            else if (command_vector[1] == "mkdir")
                this->showHelp(13);
            else if (command_vector[1] == "touch")
                this->showHelp(14);
            else if (command_vector[1] == "lrb")
                this->showHelp(15);
            else if (command_vector[1] == "crb")
                this->showHelp(16);
            else if (command_vector[1] == "map")
                this->showHelp(17);
			else
				std::cout << command_vector[1] << " ����һ����ȷ����������!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: close;  �رյ�ǰ�ռ�  ok
	else if (command == "close")
	{
		if (command_num == 1)
		{
			if (!this->mount_flag)
			{
				std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
				return 1;
			}

			this->closeSpace(); // ��Ҫ��һЩ��д����
			this->mount_flag = false;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: fmt [size(��ѡ)]; ��ʽ����ǰ�ռ�  ok
	else if (command == "fmt")
	{
		if (command_num == 1)
		{
			if (!this->mount_flag)
			{
				std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
				return 1;
			}
			this->formatSpace(); // Ĭ��Ϊ 4
			std::cout << "��ʽ���ɹ�!" << std::endl;
		}
		else if (command_num == 2)
		{
			std::regex reg_pattern_size("^\\+?[1-9][0-9]*$");  // ��0����
			if (!std::regex_match(command_vector[1], reg_pattern_size))
			{
				std::cout << command_vector[1] << " ���Ƿ�������!" << std::endl;
				return 1;
			}

			int cluster_size = atoi((char *)command_vector[1].data());
			if ((cluster_size & (cluster_size - 1)) || cluster_size > 64)
			{
				std::cout << command_vector[1] << " ���ǽ������!(Ӧ{x|1<=x<=64 �� xΪ2���ݴη�})" << std::endl;
				return 1;
			}

			this->formatSpace((uint_32)cluster_size);
			std::cout << "��ʽ���ɹ�!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: end; �˳�����  ok
	else if (command == "end")
	{
		if (command_num == 1)
			return 0;
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: mkdir <name>; ������Ϊ name �ļ���  ok
	else if (command == "mkdir")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 2)
		{
			if (command_vector[1].length() > 8)
			{
				std::cout << "����" << command_vector[1] << " ʧ�ܣ��ļ������ƹ���!" << std::endl;
				return 1;
			}

			if (this->makeDir((char *)command_vector[1].data()) == -1)
				std::cout << command_vector[1] << " �ļ�������!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: cd <dirname>; �ƶ��� dirname �ļ���  ok
	else if (command == "cd")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 2)
		{
			if (this->changeDirectory((char *)command_vector[1].data()) != 1)
				std::cout << command_vector[1] << " ������!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: touch filename; ���� filename �ļ�  ok
	else if (command == "touch")
	{
		if (!this->mount_flag)
		{
			std::cout << "��ǰû�д򿪿ռ�!" << std::endl;
			return 1;
		}
		if (command_num == 2)
		{
			if (command_vector[1].length() > 12)
			{
				std::cout << "����" << command_vector[1] << " ʧ�ܣ��ļ����ƹ���!" << std::endl;
				return 1;
			}

			if (this->createFile((char *)command_vector[1].data()) == -1)
				std::cout << command_vector[1] << " �ļ�����!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

    // example: tree; ��ӡĿ¼��   ok
    else if (command == "tree")
    {
        if (command_num == 1)
            this->treeDirectory();
        else
            std::cout << command << " ��������!" << std::endl;
    }

    // example: lrb; ��ʾ����վ
    else if (command == "lrb")
    {
        if (command_num == 1)
            this->showRecycleBin();
        else
            std::cout << command << " ��������!" << std::endl;
    }
    
    // example: crb; ��ջ���վ
	/*
    else if (command == "crb")
    {
        if (command_num == 1)
            this->emptyRecycleBin();
        else
            std::cout << command << " ��������!" << std::endl;
    }*/

    // example: map; �鿴�ļ�ʹ�ÿ��
    else if (command == "map")
    {
        if (command_num == 2)
        {
            if (this->occupyCluster((char *)command_vector[1].data()) != 1)
                std::cout << "�����ļ�ʧ��!" << std::endl;
        }
        else
            std::cout << command << " ��������!" << std::endl;
    }
	else
	{
		std::cout << command << " ����һ����ȷ������!" << std::endl;
		bool if_dis = false;
		for (int i = 0; i < 15; i++)
		{
			command_distance[i] = MfsAlg::LevenDistance(command, command_array[i]);
			if (command_distance[i] < 3)
				if_dis = true;
		}
		if (!if_dis)
			return 1;
		std::cout << "���������� ";
		bool dis_flag = false;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				if (command_distance[j] == i)
				{
					std::cout << command_array[j] << " ";
					if (i <= 1)
						dis_flag = true;
				}
			}
			if (dis_flag)
				break;
		}
		std::cout << "������?" << std::endl;
	}

	return 1;
}