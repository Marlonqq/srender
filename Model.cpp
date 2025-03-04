#include "Model.h"

Model::Model(QString path)
{
	loadModel(path);
	centre = glm::vec3((minX + maxX) / 2.0f,
					   (minY + maxY) / 2.0f,
		               (minZ + maxZ) / 2.0f);
}

void Model::Draw()
{
	for (int i = 0; i < mesh_list.size(); ++i)
		mesh_list[i].Draw();
}

void Model::loadModel(QString path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.toStdString(), aiProcess_Triangulate | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		load_success = false;
		return;
	}
	
	directory = path.mid(0, path.lastIndexOf('/'));
	processNode(scene->mRootNode, scene);

	// SRender在Render循环前处理三角形
	//------------------------------------------------
	SRender::getInstance().texture_list = texture_list;
	SRender::getInstance().calculate_triangle_list();
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		// scene中的meshes是真正的mesh数据,  node中的meshes存储的是索引
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		mesh_list.emplace_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::cout << "Mesh++" << std::endl;
	vert_cnt += mesh->mNumVertices;
	face_cnt += mesh->mNumFaces;
	
	Mesh res;
	Vertex vert;
	// 遍历顶点 get顶点数据 在mesh.vertices队尾添加
	//----------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		minX = std::min(minX, mesh->mVertices[i].x);
		minY = std::min(minY, mesh->mVertices[i].y);
		minZ = std::min(minZ, mesh->mVertices[i].z);
		maxX = std::max(maxX, mesh->mVertices[i].x);
		maxY = std::max(maxY, mesh->mVertices[i].y);
		maxZ = std::max(maxZ, mesh->mVertices[i].z);

		vert.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vert.norm = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		// only process one texture
		if (mesh->mTextureCoords[0])
			vert.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vert.texCoord = glm::vec2(0.0f, 0.0f);


		if (vert.texCoord.x < 0) vert.texCoord.x += 1;
		if (vert.texCoord.y < 0) vert.texCoord.y += 1;

		res.vertices.emplace_back(vert);
	}

	// 遍历面 get顶点下标
	//----------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			res.indices.emplace_back(face.mIndices[j]);
	}

	// 获取材质贴图的纹理id
	//----------------------------------------------------------------------------------------
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		res.diffuseTextureIndex = loadMaterialTextures(res, material, aiTextureType_DIFFUSE);
		res.specularTextureIndex = loadMaterialTextures(res, material, aiTextureType_SPECULAR);
	}

	return res;
}

int Model::loadMaterialTextures(Mesh& mesh, aiMaterial* material, aiTextureType type)
{
	if (material->GetTextureCount(type) > 0)
	{
		aiString str;
		material->GetTexture(type, 0, &str);
		QString path = directory + "/" + str.C_Str();
		for (int i = 0; i < texture_list.size(); ++i)
		{
			if (texture_list[i].path == path.toStdString())
				return i;
		}

		Texture texture;
		if (texture.loadTexture(path.toStdString().c_str()))
		{
			//qDebug() << path;
			texture_list.emplace_back(texture);
			return texture_list.size() - 1;
		}
		else
			return -1;
	}
	
	return -1;
}
