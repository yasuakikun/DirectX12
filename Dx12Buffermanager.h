#pragma once
#include "Dx12Buffer.h"
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class Dx12Buffermanager : public Dx12Buffer
{


public:
	Dx12Buffermanager();
	virtual~Dx12Buffermanager() = 0;
};

