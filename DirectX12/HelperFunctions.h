#pragma once

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
}