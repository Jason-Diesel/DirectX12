#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Graphics.h"

class Mesh;
class Model;

void loadMaterials(std::vector<TextureViewClass>& textures, const aiScene* pScene);

Mesh* loadMesh(aiMesh* pMesh);

Model* loadModel(const std::string& fileName);