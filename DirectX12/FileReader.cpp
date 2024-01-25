#include "FileReader.h"

#include "Model.h"


void loadMaterials(std::vector<TextureViewClass>& textures, const aiScene* pScene)
{
	textures.resize(pScene->mNumMaterials);
	for (uint32_t i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				//load diffuse Texture
				std::string nPath = "../" + std::string(path.C_Str());
				textures[i] = getGFX.createTexture(nPath);
			}
		}
	}
}

Mesh* loadMesh(aiMesh* pMesh)
{
	Mesh* theReturnMesh = nullptr;
	std::vector<Vertex> vertecies;
	std::vector<uint32_t> indecies;

	vertecies.reserve(pMesh->mNumVertices);
	indecies.reserve(pMesh->mNumFaces * 3);

	for (uint32_t i = 0; i < pMesh->mNumVertices; i++)
	{
		Vertex vert;
		vert.position.x = pMesh->mVertices[i].x;
		vert.position.y = pMesh->mVertices[i].y;
		vert.position.z = pMesh->mVertices[i].z;

		vert.uv.x = pMesh->mTextureCoords[0][i].x;
		vert.uv.y = pMesh->mTextureCoords[0][i].y;
		vertecies.push_back(vert);
	}

	for (uint32_t i = 0; i < pMesh->mNumFaces; i++) {
		indecies.push_back(pMesh->mFaces[i].mIndices[0]);
		indecies.push_back(pMesh->mFaces[i].mIndices[1]);
		indecies.push_back(pMesh->mFaces[i].mIndices[2]);
	}
	theReturnMesh = new Mesh(vertecies, indecies);

	return theReturnMesh;
}

Model* loadModel(const std::string& fileName)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(fileName.c_str(),
		aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs
	);

	if (!pScene) {
		printf("Error");
		return nullptr;
	}
	Model* theReturnModel = new Model();

	theReturnModel->getMeshes().reserve(pScene->mNumMeshes);

	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		theReturnModel->getMeshes().push_back(loadMesh(pScene->mMeshes[i]));
	}

	std::vector<TextureViewClass> textures;
	loadMaterials(textures, pScene);

	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		if (textures[pScene->mMeshes[i]->mMaterialIndex].srvResource == nullptr)
		{
			bool done = false;
			for (int m = 0; m < textures.size() && !done; m++)
			{
				if (textures[m].srvResource != nullptr)
				{
					theReturnModel->getMeshes()[i]->setTextureViewClass(textures[m]);
					done = true;
				}
			}
			if (!done)
			{
				std::cout << "error" << std::endl;
				breakDebug;
			}
		}
		else {
			theReturnModel->getMeshes()[i]->setTextureViewClass(textures[pScene->mMeshes[i]->mMaterialIndex]);
		}
		
	}

	return theReturnModel;
}
