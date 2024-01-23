#include "TestObject.h"
#include "Graphics.h"

TestObject::TestObject()
{
	vertecies.push_back(ColorVertex{ {0.5, 0.5, 0},{1,0,0} });
	vertecies.push_back(ColorVertex{ {0.5, -0.5, 0}, {0,1,0} });
	vertecies.push_back(ColorVertex{ {-0.5,0.5, 0},{0,0,1} });
	vertecies.push_back(ColorVertex{ {-0.5,-0.5, 0},{1,1,1} });
	Object::init();
}

TestObject::~TestObject()
{
}
