#pragma once
#include "Mesh.h"
#include "Texture.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <qstring.h>
class Model {
public:
	Model(QString path);
	void Draw();
	glm::vec3 centre;
	int vert_cnt{ 0 };
	int face_cnt{ 0 };
	bool load_success{ true };

private:
	float minX{ FLT_MIN };
	float minY{ FLT_MIN };
	float minZ{ FLT_MIN };
	float maxX{ FLT_MAX };
	float maxY{ FLT_MAX };
	float maxZ{ FLT_MAX };
	std::vector<Mesh> mesh_list;
	std::vector<Texture> texture_list;
	QString directory;

	void loadModel(QString path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	int loadMaterialTextures(Mesh& mesh, aiMaterial* material, aiTextureType type);
};

