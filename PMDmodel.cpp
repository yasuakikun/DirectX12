#include "PMDModel.h"
#include <iostream>

PMDModel::PMDModel(const char* filePath)
{
	//ヘッダー部分
	struct PMDHeader
	{
		char magic[3];	//4バイトアライメントで1バイトパディングが置かれる
		float version;
		char model_name[20];
		char comment[256];
	};	//合計283バイト

	auto fp = fopen(filePath, "rb");

	//ヘッダー部分の読み込み
	PMDHeader header = {};

	fread(&header, sizeof(header.magic), 1, fp);

	fread(&header.version, sizeof(header) - sizeof(header.magic) - 1, 1, fp);

	//データ部分の読み込み
	unsigned int vert_count = 0;

	fread(&vert_count, sizeof(vert_count), 1, fp);	//データ量読み込み

	vert_data_size = sizeof(PMDVerts) - 2;

	verticesData.resize(vert_count);

	for (unsigned int cnt = 0; cnt < vert_count; ++cnt)
	{
		fread(&verticesData[cnt], vert_data_size, 1, fp);
	}

	//インデックスの読み込み
	//９０３６インデクス数
	unsigned int indexCnt = 0;

	fread(&indexCnt, sizeof(indexCnt), 1, fp);

	indicesData.resize(indexCnt);

	fread(indicesData.data(), sizeof(unsigned short), indexCnt, fp);

	//マテリアルの読み込み
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
