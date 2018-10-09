#include "PMDModel.h"
#include <iostream>

PMDModel::PMDModel(const char* filePath)
{
	//�w�b�_�[����
	struct PMDHeader
	{
		char magic[3];	//4�o�C�g�A���C�����g��1�o�C�g�p�f�B���O���u�����
		float version;
		char model_name[20];
		char comment[256];
	};	//���v283�o�C�g

	auto fp = fopen(filePath, "rb");

	//�w�b�_�[�����̓ǂݍ���
	PMDHeader header = {};

	fread(&header, sizeof(header.magic), 1, fp);

	fread(&header.version, sizeof(header) - sizeof(header.magic) - 1, 1, fp);

	//�f�[�^�����̓ǂݍ���
	unsigned int vert_count = 0;

	fread(&vert_count, sizeof(vert_count), 1, fp);	//�f�[�^�ʓǂݍ���

	vert_data_size = sizeof(PMDVerts) - 2;

	verticesData.resize(vert_count);

	for (unsigned int cnt = 0; cnt < vert_count; ++cnt)
	{
		fread(&verticesData[cnt], vert_data_size, 1, fp);
	}

	//�C���f�b�N�X�̓ǂݍ���
	//�X�O�R�U�C���f�N�X��
	unsigned int indexCnt = 0;

	fread(&indexCnt, sizeof(indexCnt), 1, fp);

	indicesData.resize(indexCnt);

	fread(indicesData.data(), sizeof(unsigned short), indexCnt, fp);

	//�}�e���A���̓ǂݍ���
	unsigned int materialCnt = 0;

	fread(&materialCnt, sizeof(materialCnt), 1, fp);

	materialData.resize(materialCnt);

	for (auto& mat : materialData)
	{
		fread(&mat, 46, 1, fp);
		fread(&mat.face_vert_count, 24, 1, fp);
	}

	fclose(fp);
}

PMDModel::~PMDModel()
{
}

std::vector<PMDVerts>& PMDModel::GetModelData()
{
	return verticesData;
}

std::vector<unsigned short> & PMDModel::GetIndexData()
{
	return indicesData;
}

std::vector<PMDMaterial>& PMDModel::Materials()
{
	return materialData;
}
