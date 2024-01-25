#pragma once
#include "CommonHeader.h"

static std::wstring CreateStringandIndexToWString(std::string str, int index) {
	std::string t = str + std::to_string(index);
	std::wstring temp = std::wstring(t.begin(), t.end());
	return temp;
}

static std::wstring createStringToWString(std::string str)
{
	return std::wstring(str.begin(), str.end());
}

namespace HF {
	template <typename T>
	void release(T*& resource)
	{
		if (resource)
		{
			resource->Release();
			resource = nullptr;
		}
	}

	void xRotation(DirectX::XMMATRIX& matrix, const float& rotation);
	void yRotation(DirectX::XMMATRIX& matrix, const float& rotation);
	void zRotation(DirectX::XMMATRIX& matrix, const float& rotation);
	void rotationMatrix(DirectX::XMMATRIX& matrix, float xRotation, float yRotation, float zRotation);
}