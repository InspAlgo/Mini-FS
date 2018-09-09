//
//	mfs_coresystem.cpp
//
//		Copyright (c) AlphaBeta Team. All rights reserved.
//
//	This is the core implementation of Mini File System.
//

#include "mini_file_system.h"

MiniFS::MiniFS()
{
}

MiniFS::~MiniFS()
{
}

#define HELP_OUTPUT(cmd_name, cmd_content, cmd_layout) \
	std::cout.setf(std::ios::left);                    \
	std::cout.fill('.');                               \
	std::cout.width(12);                               \
	std::cout << cmd_name;                             \
	std::cout.fill(' ');                               \
	std::cout.width(18);                               \
	std::cout << cmd_content;                          \
	std::cout << "ָ���ʽ: ";                         \
	std::cout.fill(' ');                               \
	std::cout << cmd_layout << std::endl;              \
	std::cout.unsetf(std::ios::left);

#define HELP_DETAIL(cmd_name, cmd_layout, cmd_content, cmd_example) \
	std::cout << cmd_name << std::endl;                             \
	std::cout << "ָ���ʽ: " << cmd_layout << std::endl;           \
	std::cout << "ָ���: " << cmd_content << std::endl;          \
	std::cout << "ָ��ʾ��: " << cmd_example << std::endl;

MiniFS::MiniFS()
{
}

MiniFS::~MiniFS()
{
}

int MiniFS::cmd(void)
{
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

	int handle_re; // ���ո�������崦�����ķ���ֵ

	// example: create space_name; space_name ���԰���·��
	if (command == "create")
	{
		if (command_num == 2)
		{
			std::regex reg_pattern("^\\w+$"); // ���ļ���ΪӢ�ġ����ֺ��»���ʱ�Ϸ�
			if (std::regex_match(command_vector[1], reg_pattern))
			{
				this->createSpace((char *)command_vector[1].data(), 1024, 4);
				std::cout << "YES!" << std::endl;
			}
			else
				std::cout << command_vector[1] << " �Ƿ��ļ���!" << std::endl;
		}
		else if (command_num == 3)
		{
			bool flag = true;

			std::regex reg_pattern_name("^\\w+$"); // ���ļ���ΪӢ�ġ����ֺ��»���ʱ�Ϸ�
			if (!std::regex_match(command_vector[1], reg_pattern_name))
			{
				std::cout << command_vector[1] << " �Ƿ��ļ���!" << std::endl;
				flag = false;
			}

			std::regex reg_pattern_size("^\\+?[1-9][0-9]*$"); // �ռ��С����Ϊ��0����
			if (!std::regex_match(command_vector[2], reg_pattern_size))
			{
				std::cout << command_vector[2] << " ���Ƿ�������!" << std::endl;
				return 1;
			}

			int space_size = atoi((char *)command_vector[2].data());
			if (space_size < 128)
			{
				std::cout << command_vector[2] << " ����С�� 128!" << std::endl;
				flag = false;
			}

			if (!flag)
				return 1;

			this->createSpace((char *)command_vector[1].data(), space_size, 4);
			std::cout << "YES!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: mount space_name; space_name ���԰���·��
	else if (command == "mount")
	{
		if (command_num == 2)
		{
			std::ifstream ifs(command_vector[1].c_str());
			if (ifs)
			{
				std::cout << command_vector[1] << " ����" << std::endl;
				this->mountSpace((char *)command_vector[1].data());
			}
			else
				std::cout << command_vector[1] << " �����ڴ�·��!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: dr; ��ʾ��ǰ�ļ����µ��ļ�Ŀ¼
	else if (command == "dr")
	{
		if (command_num == 1)
		{
			this->showDirectory();
			std::cout << command << " YES!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: cp filename_old filename_new;
	// �д��ⲿϵͳ->Mini-FS��Mini-FS->�ⲿϵͳ��Mini-FS->Mini-FS�������
	else if (command == "cp")
	{
		if (command_num == 3)
		{
			// �ж� filename_old filename_new �ĺϷ���
			std::cout << command_vector[1] << command_vector[2] << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: dl filename; ɾ���ռ��е��ļ�
	else if (command == "dl")
	{
		if (command_num == 2)
		{
			// �ж� filename �ĺϷ���
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: tp filename; ��ʾ�����ı��ļ�
	else if (command == "tp")
	{
		if (command_num == 2)
		{
			// �ж� filename �ĺϷ���
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: more filename; ��ҳ��ʾ�ı��ļ�
	else if (command == "more")
	{
		if (command_num == 2)
		{
			// �ж� filename �ĺϷ���
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: att filename; ��ʾ�ļ�����
	else if (command == "att")
	{
		if (command_num == 2)
		{
			// �ж� filename �ĺϷ���
			std::cout << command_vector[1] << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: help [������(��ѡ)];
	else if (command == "help")
	{
		if (command_num == 1)
		{
			HELP_OUTPUT("create ", " �����ռ�", "create <name> [size(��ѡ)]");
			HELP_OUTPUT("mount ", " ��װ�ռ�", "mount <name>");
			HELP_OUTPUT("fmt ", " ��ʽ���ռ�", "fmt [size(��ѡ)]");
			HELP_OUTPUT("close ", " �˳��ռ�", "close");
			HELP_OUTPUT("dr ", " ��ʾĿ¼", "dr");
			HELP_OUTPUT("cp ", " �����ļ�", "cp <name_old> <name_new>");
			HELP_OUTPUT("dl ", " ɾ���ļ�", "dl <name>");
			HELP_OUTPUT("tp ", " ��ʾ�ı��ļ�", "tp <name>");
			HELP_OUTPUT("more ", " ��ҳ��ʾ�ı��ļ�", "more <name>");
			HELP_OUTPUT("att ", " ��ʾ�ļ�����", "att <name>");
			HELP_OUTPUT("help ", " ������Ϣ", "help [������(��ѡ)]");
		}
		else if (command_num == 2)
		{
			if (command_vector[1] == "create")
			{
				HELP_DETAIL("create", "create <name> [size(��ѡ)]",
							"�� create �����һ���µ� Mini-FS �ռ�",
							"create SName ����һ����Ϊ SName ��СΪ 1G �� Windows �ļ���Ϊ�洢�ռ� \n\t  create SName 2048 ����һ����Ϊ SName ��СΪ 2G �� Windows �ļ���Ϊ�洢�ռ�");
			}
			else if (command_vector[1] == "mount")
			{
				HELP_DETAIL("mount", "mount <name>", "�� mount ���װҪ�����Ŀռ�",
							"mount SName �� Mini-FS �а�װ(��)�ռ� SName��Ϊ������������׼��");
			}
			else if (command_vector[1] == "fmt")
			{
				HELP_DETAIL("fmt", "fmt [size(��ѡ)]", "�� fmt �����ʽ����ǰ�Ŀռ�",
							"fmt ��ʼ����ǰ�򿪵� SName �ռ䣬�ش�С����Ϊ 4KB \n\t  fmt 8 ��ʼ����ǰ�򿪵� SName �ռ䣬�ش�С����Ϊ 8KB");
			}
			else if (command_vector[1] == "close")
			{
				HELP_DETAIL("close", "close", "�� close �����˳� Mini-FS", "close �˳����ر� Mini-FS ϵͳ");
			}
			else if (command_vector[1] == "dr")
			{
				HELP_DETAIL("dr", "dr", "dr ��ʾ��ǰĿ¼�µ��ļ�Ŀ¼", "");
			}
			else if (command_vector[1] == "cp")
			{
				HELP_DETAIL("cp", "cp <name1> <name2>", "�� cp ������ļ�",
							"cp wFName sFName ���ļ��� Windows �ռ��п����� Mini-FS �ռ��� \n\t  cp sFName wFName ���ļ��� Mini-FS �ռ��п����� Windows �ռ��� \n\t  cp sFName1 sFName2 ���ļ��� Mini-FS �ռ��п����� Mini-FS �ռ���");
			}
			else if (command_vector[1] == "dl")
			{
				HELP_DETAIL("dl", "dl <name>", "�� dl ����ɾ���ռ��е��ļ�",
							"dl sFName ���ļ� sFName �ӿռ���ɾ��");
			}
			else if (command_vector[1] == "tp")
			{
				HELP_DETAIL("tp", "tp <name>", "�� tp ������ʾ�����ı��ļ�",
							"tp SFName ��ʾ��Ϊ sFName ���ı��ļ�������");
			}
			else if (command_vector[1] == "more")
			{
				HELP_DETAIL("more", "more <name>", "�� more �����ҳ��ʾ�ı��ļ�",
							"more sFName ��ҳ��ʾ��Ϊ sFName ���ı��ļ�������");
			}
			else if (command_vector[1] == "att")
			{
				HELP_DETAIL("att", "att <name>", "�� att ������ʾ�ı�����",
							"att sFName ��ʾ��Ϊ sFName ���ļ����ļ�����(�ļ�������չ��������ʱ�䡢�ļ���С)");
			}
			else if (command_vector[1] == "help")
			{
				HELP_DETAIL("help", "help [������(��ѡ)]", "�� help �����ȡ������Ϣ",
							"help ��ʾϵͳ���Ƽ����� \n\t  help create ��ʾ create ��ָ���ʽ����ϸ��Ϣ");
			}
			else
				std::cout << command_vector[1] << " ����һ����ȷ����������!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: close;  �رյ�ǰ�ռ�
	else if (command == "close")
	{
		if (command_num == 1)
		{
			this->closeSpace(); // ��Ҫ��һЩ��д����
			return 0;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	// example: fmt [size(��ѡ)]; ��ʽ����ǰ�ռ�
	else if (command == "fmt")
	{
		if (command_num == 1)
		{
			this->formatSpace(); // Ĭ��Ϊ 4
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
				std::cout << command_vector[1] << " �����Ƽ��Ĳ���!" << std::endl;
		}
		else
			std::cout << command << " ��������!" << std::endl;
	}

	else
		std::cout << command << " ����һ����ȷ������!" << std::endl;

	return 1;
}

/// <summary> ��Ӳ���е�MBR��Ϣ�����ڴ�mbr�� </summary>
void MiniFS::readMBR(void)
{
	fseek(space_fp, 0L, SEEK_SET);
	fread(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> ���ڴ��е�mbr��Ϣд��Ӳ�� </summary>
void MiniFS::writeMBR(void) const
{
	fseek(space_fp, 0L, SEEK_SET);
	fwrite(&mbr, sizeof(MBR), 1, space_fp);
}

/// <summary> ��Ӳ���е�CAB��Ϣ�����ڴ�CAB�� </summary>
void MiniFS::readCAB(void)
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> ���ڴ��е�CAB��Ϣд��Ӳ�� </summary>
void MiniFS::writeCAB(void) const
{
	fseek(space_fp, mbr.CAB_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(CAB, sizeof(uint_8), CAB_occupu_byte, space_fp);
}

/// <summary> ��Ӳ���е�FAT��Ϣ�����ڴ�FAT�� </summary>
void MiniFS::readFAT(void)
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> ���ڴ��е�FAT��Ϣд��Ӳ�� </summary>
void MiniFS::writeFAT(void) const
{
	fseek(space_fp, mbr.FAT_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(FAT, sizeof(uint_32), mbr.cluster_num, space_fp);
}

/// <summary> ��Ӳ����ָ���غŵ���Ϣ����buffer </summary>
/// <param name="cluster"> ָ���غ� </param>
void MiniFS::readCluster(const uint_32 cluster)
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fread(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

/// <summary> ��buffer�е�����д��Ӳ��ָ���غ� </summary>
void MiniFS::writeCluster(const uint_32 cluster) const
{
	fseek(space_fp, cluster * mbr.cluster_size * 1024, SEEK_SET);
	fwrite(buffer, mbr.cluster_size * 1024, 1, space_fp);
}

/// <summary> ��buffer�е�����д��Ӳ��ָ���غ� </summary>
/// <param name="cluster"> ָ��Ŀ¼�ļ�Ӳ����ڴغ� </param>
/// <return> �ڴ�Ŀ¼�ṹ </return>
Directory MiniFS::readDirectory(uint_32 dir_entrance) const
{
	Directory cur_dir;						   // ��ǰ��ȡ���ļ�Ŀ¼
	DFC dir_buffer;							   // Ŀ¼�ļ�������
	uint_32 cur_cluster = dir_entrance;		   // ��ǰ����Ĵغ�
	uint_32 block_num = mbr.cluster_size * 16; // ���ؿɴ�fcb����
	uint_32 remain_file;					   // ��ǰ�ļ���δ��ȡFCB�ļ���
	uint_32 remain_block;					   // ��ǰ��δ������

	fseek(space_fp, dir_entrance * mbr.cluster_size * 1024, SEEK_SET);
	fread(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);

	cur_dir.header = dir_buffer.firstclu.header;
	cur_dir.fcb = (FCB *)malloc(cur_dir.header.file_num * sizeof(FCB));

	remain_file = cur_dir.header.file_num;
	remain_block = block_num - 1;

	uint_32 idx_m = 0;
	uint_32 idx_d = 0;
	while (remain_block > 0 && remain_file > 0)
	{
		cur_dir.fcb[idx_m] = dir_buffer.firstclu.fcb[idx_d];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}
	while (remain_file > 0)
	{
		if (remain_block == 0)
		{
			cur_cluster = FAT[cur_cluster];
			fseek(space_fp, cur_cluster * mbr.cluster_size * 1024, SEEK_SET);
			fread(&dir_buffer, mbr.cluster_size * 1024, 1, space_fp);
			remain_block = block_num;
			idx_d = 0;
		}
		cur_dir.fcb[idx_m] = dir_buffer.otherclu.fcb[idx_d];
		remain_block--;
		remain_file--;
		idx_m++;
		idx_d++;
	}

	return cur_dir;
}

void MiniFS::rewriteDirectory(const Directory dir) const
{
	uint_32 cur_cluster = dir.header.current_dir_entrance;
	uint_32 remain_file;  // ��ǰ�ļ���δ��ȡFCB�ļ���
	uint_32 remain_block; // ��ǰ��ʣ��տ���
}

int MiniFS::createSpace(char name[], uint_32 space_size, uint_32 cluster_size)
{
}

int MiniFS::mountSpace(char name[])
{
}

/// <summary> ��ʽ���ռ� </summary>
/// <param name="cluster_size"> �ļ�ϵͳ���ش�С </param>
/// <return> 1:��ʽ���ɹ�; -1:��ʽ��ʧ�� </return>
int MiniFS::formatSpace(uint_32 cluster_size)
{
	// ��дMBR

	/// <summary> ��ʽ���ռ� </summary>
	/// <param name="cluster_size"> �ļ�ϵͳ���ش�С </param>
	/// <return> 1:��ʽ���ɹ�; -1:��ʽ��ʧ�� </return>
	int MiniFS::formatSpace(uint_32 cluster_size)
	{
		// ��дMBR
		mbr.cluster_size = cluster_size;
		mbr.cluster_num = mbr.space_size * 1024 / mbr.cluster_size;
		mbr.CAB_entrance = 1;
		// CAB�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 8192 ��
		mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0 * mbr.cluster_size));
		// FAT�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 256 ��
		mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0 * mbr.cluster_size));
		mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;

		// MBRд��Ӳ��
		writeMBR();

		// ���¿���CAB�ڴ�ռ䲢д��
		CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));

		free(CAB);
		CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
		writeCAB();

		// ���¿���FAT�ڴ�ռ�
		free(FAT);
		FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
		writeFAT();

		// ����ļ�Ŀ¼������Ŀ¼�ļ�д��
		directory.clear();
		strcpy(directory[0].header.name, mbr.space_name);
		directory[0].header.occupy_cluster_num = 1;
		directory[0].header.current_dir_entrance = mbr.RDF_entrance;
		directory[0].header.parent_dir_entrance = mbr.RDF_entrance;
		directory[0].header.file_num = 0;
		directory[0].header.create_time = mbr.create_time;
		directory[0].header.modify_time = time(NULL);
		directory[0].header.folder_size = sizeof(DFH);
		directory[0].fcb = (FCB *)calloc(1, sizeof(FCB));

		rewriteDirectory(directory[0]);

		// ��д�ļ���������С
		buffer = realloc(buffer, mbr.cluster_size * 1024);
	}

	mbr.cluster_size = cluster_size;
	mbr.cluster_num = mbr.space_size * 1024 / mbr.cluster_size;
	mbr.CAB_entrance = 1;
	// CAB�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 8192 ��
	mbr.FAT_entrance = mbr.CAB_entrance + (uint_32)ceil(mbr.cluster_num / (8192.0 * mbr.cluster_size));
	// FAT�ļ�ռ�ô��� Ϊ �� ���� / ���ش�С(KB) / 256 ��
	mbr.RDF_entrance = mbr.FAT_entrance + (uint_32)ceil(mbr.cluster_num / (256.0 * mbr.cluster_size));
	mbr.free_cluster_num = mbr.cluster_num - mbr.RDF_entrance - 1;

	// MBRд��Ӳ��
	writeMBR();

	// ���¿���CAB�ڴ�ռ䲢д��
	CAB_occupu_byte = (uint_32)(ceil(mbr.cluster_num / 8.0));

	free(CAB);
	CAB = (uint_8 *)calloc(CAB_occupu_byte, sizeof(uint_8));
	writeCAB();

	// ���¿���FAT�ڴ�ռ�
	free(FAT);
	FAT = (uint_32 *)calloc(mbr.cluster_num, sizeof(uint_32));
	writeFAT();

	// ����ļ�Ŀ¼������Ŀ¼�ļ�д��
	directory.clear();
	strcpy(directory[0].header.name, mbr.space_name);
	directory[0].header.occupy_cluster_num = 1;
	directory[0].header.current_dir_entrance = mbr.RDF_entrance;
	directory[0].header.parent_dir_entrance = mbr.RDF_entrance;
	directory[0].header.file_num = 0;
	directory[0].header.create_time = mbr.create_time;
	directory[0].header.modify_time = time(NULL);
	directory[0].header.folder_size = sizeof(DFH);
	directory[0].fcb = (FCB *)calloc(1, sizeof(FCB));

	rewriteDirectory(directory[0]);

	// ��д�ļ���������С
	buffer = realloc(buffer, mbr.cluster_size * 1024);
}

int MiniFS::closeSpace(void)
{
}

int MiniFS::showDirectory(void)
{
}
int MiniFS::copyFile(char filename_1[], char filename_2[], int mode)
{
}
int MiniFS::deleteFile(char filename[])
{
}
int MiniFS::displayFile(char filename[])
{
}
int MiniFS::moreDisplayFile(char filename[])
{
}
int MiniFS::showAttribute(char filename[])
{
}

int MiniFS::showHelp(int mode)
{
}

int MiniFS::makeDir(char filename[])
{
}
