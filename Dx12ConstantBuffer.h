#pragma once
#include "Dx12ConstantBufferBase.h"
#include "Dx12Buffermanager.h"
//�萔�o�b�t�@���N���X����h�������萔�o�b�t�@�N���X
//template <typename T>
class Dx12ConstantBuffer :public Dx12ConstantBufferBase
{
	friend Dx12Buffermanager;
private:
	//T* _mappedAddress;
	//���ڍ�点�Ȃ�

	Dx12ConstantBuffer() {};
	Dx12ConstantBuffer(const Dx12ConstantBuffer&);
	void operator=(const Dx12ConstantBuffer&);
public:
	//~Dx12ConstantBuffer();
	/*void updateValue(T& Value) 
	{
		*_mappedAddress = Value;
	}*/
};

