#pragma once
#include <vector>

//���_�f�[�^
struct PMDVerts
{
	float pos[3];				// x, y, z // ���W
	float normal_vec[3];		// nx, ny, nz // �@���x�N�g��
	float uv[2];				// u, v // UV���W // MMD �͒��_ UV
	unsigned short boneIdx[2];	// �{�[���ԍ� 1�A�ԍ� 2 // ���f���ό`(���_�ړ�)���ɉe��
	unsigned char  bone_weight;	// �{�[�� 1 �ɗ^����e���x // min:0 max:100 // �{�[�� 2 �ւ̉e���x�́A(100 - bone_weight)
	unsigned char edge_flag;	// 0:�ʏ�A1:�G�b�W���� // �G�b�W(�֊s)���L���̏ꍇ
};	//38�o�C�g(�p�f�B���O�ɂ��A40�o�C�g�̍\���̂ɂȂ�)

//�}�e���A���e�[�^
struct PMDMaterial
{
	float diffuse_color[3];		// dr, dg, db
	float alpha;				// �����F�̕s�����x 
	float specularity;
	float specular_color[3];	// sr, sg, sb // ����F 
	float mirror_color[3];		// mr, mg, mb // ���F(ambient) 
	unsigned char toon_index;	// toon??.bmp // 0.bmp:0xFF, 1(01).bmp:0x00 �E�E�E 10.bmp:0x09 
	unsigned char edge_flag;	// �֊s�A�e 
	unsigned int face_vert_count; // �ʒ��_��
	char texture_file_name[20];	//�e�N�X�`���t�@�C����
};	//70�o�C�g(�p�f�B���O�ɂ��A72�o�C�g)

class PMDModel
{
private:
	unsigned int vert_data_size;	//1���_�̃T�C�Y

	std::vector<PMDVerts> verticesData;	//���_�f�[�^

	std::vector<unsigned short> indicesData;	//�C���f�b�N�X�f�[�^

	std::vector<PMDMaterial> materialData;	//�}�e���A���f�[�^

public:
	PMDModel(const char* filePath);
	~PMDModel();

	//���_�f�[�^�̎擾
	std::vector<PMDVerts>& GetModelData();

	//�C���f�b�N�X�̎擾
	std::vector<unsigned short> & GetIndexData();

	std::vector<PMDMaterial>& Materials();
};
