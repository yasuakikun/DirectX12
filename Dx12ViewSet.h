#pragma once
#include "Dx12Buffermanager.h"
#include "Dx12TextureBuffer.h"
#include "Dx12ConstantBuffer.h"
#include <vector>
#include <memory>
class Dx12ViewSet
{
	friend Dx12Buffermanager;
private:
	std::vector<std::shared_ptr<Dx12TextureBuffer>> _textres;
	std::vector < std::shared_ptr<Dx12ConstantBuffer>> _constant;
	D3D12_VERTEX_BUFFER_VIEW * _bufferview;

public:
	Dx12ViewSet();
	~Dx12ViewSet();
	void BuildHeapAndViews();
};

