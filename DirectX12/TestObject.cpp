#include "TestObject.h"
#include "Graphics.h"

TestObject::TestObject()
{
	this->model = loadModel("../Objects/sponza.obj");
	Object::init();
}

TestObject::~TestObject()
{
}
