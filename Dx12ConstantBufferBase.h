#pragma once
#include "Dx12Buffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
class Dx12Buffermanager;
//定数バッファ基底クラス
class Dx12ConstantBufferBase : public Dx12Buffer
{
	friend Dx12Buffermanager;
protected:
	size_t _buffSize;
	void* Map();
public:
	Dx12ConstantBufferBase();
	virtual ~Dx12ConstantBufferBase() = 0;
	size_t GetBufferSize() { return _buffSize; }

};

