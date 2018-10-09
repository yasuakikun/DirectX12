#pragma once
#include "D3Dx12.h"
class Dx12Buffer
{
private:
	ID3D12Resource * _Buffer;
	
public:
	Dx12Buffer();
	virtual virtual ~Dx12Buffer();
};


