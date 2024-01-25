#include "HelperFunctions.h"

void HF::xRotation(DirectX::XMMATRIX& matrix, const float& rotation)
{
	//X rotation
	DirectX::XMMATRIX Mrot(
		std::cos(rotation), 0.0f, std::sin(rotation), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-std::sin(rotation), 0.0f, std::cos(rotation), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	matrix = matrix * Mrot;
}

void HF::yRotation(DirectX::XMMATRIX& matrix, const float& rotation)
{
	DirectX::XMMATRIX Mrot(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, std::cos(rotation), std::sin(rotation), 0.0f,
		0.0f, -std::sin(rotation), std::cos(rotation), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	matrix = matrix * Mrot;
}

void HF::zRotation(DirectX::XMMATRIX& matrix, const float& rotation)
{
	DirectX::XMMATRIX Mrot(
		std::cos(rotation), -std::sin(rotation), 0.0f, 0.0f,
		std::sin(rotation), std::cos(rotation), 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	matrix = matrix * Mrot;
}

void HF::rotationMatrix(DirectX::XMMATRIX& matrix, float xRotation, float yRotation, float zRotation)
{
	HF::xRotation(matrix, xRotation);
	HF::yRotation(matrix, yRotation);
	//HF::zRotation(matrix, zRotation);//Just gonna skip this for now
}
