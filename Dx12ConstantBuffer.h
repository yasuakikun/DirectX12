#pragma once
#include "Dx12ConstantBufferBase.h"
#include "Dx12Buffermanager.h"
//定数バッファ基底クラスから派生した定数バッファクラス
//template <typename T>
class Dx12ConstantBuffer :public Dx12ConstantBufferBase
{
	friend Dx12Buffermanager;
private:
	//T* _mappedAddress;
	//直接作らせない

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

