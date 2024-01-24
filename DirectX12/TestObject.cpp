#include "TestObject.h"
#include "Graphics.h"

TestObject::TestObject()
{
	texture = getGFX.createTexture("../Textures/KungFuPanda.png");
	vertecies.push_back(Vertex{ {-0.5, -0.5, -0.5},{0,0} });
	vertecies.push_back(Vertex{ {-0.5, 0.5, -0.5}, {0,0} });
	vertecies.push_back(Vertex{ {0.5, 0.5, -0.5},  {0,1} });
	vertecies.push_back(Vertex{ {0.5, -0.5, -0.5}, {0,1} });
	vertecies.push_back(Vertex{ {-0.5, -0.5, 0.5}, {1,0} });
	vertecies.push_back(Vertex{ {-0.5, 0.5, 0.5},  {1,0} });
	vertecies.push_back(Vertex{ {0.5, 0.5, 0.5},   {1,1} });
	vertecies.push_back(Vertex{ {0.5, -0.5, 0.5},  {1,1} });

	indecies.resize(36);
	indecies = {
		0,1,2,0,2,3,
		4,6,5,4,7,6,
		4,5,1,4,1,0,
		3,2,6,3,6,7,
		1,5,6,1,6,2,
		4,0,3,4,3,7
	};

	Object::init();
}

TestObject::~TestObject()
{
}
