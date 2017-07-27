#pragma once
#include "stdafx.h"

struct BasePosition{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

struct ModelViewProjection
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};