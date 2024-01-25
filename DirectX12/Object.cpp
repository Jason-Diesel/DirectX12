#include "Object.h"

Object::Object():
	position(0,0,0),
	rotation(0,0,0),
	scale(1,1,1)
{

}

Object::~Object()
{
}

void Object::init()
{
}

void Object::render()
{
	DirectX::XMMATRIX transformMatrix = CreateTransformMatrix();
	getGFX.getCommandList()->SetGraphicsRoot32BitConstants(1, sizeof(Transform) / 4, &transformMatrix, 0);

	model->render();
}

DirectX::XMMATRIX Object::CreateTransformMatrix()
{
	return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
