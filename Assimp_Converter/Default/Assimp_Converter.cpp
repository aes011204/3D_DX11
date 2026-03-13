
#include "Converter_Define.h"
#include "Converter_function.h"

bool Convert_Binary(string fbxPath, string exportPath);

int main()
{
	const char* pInputFilePath = "../../Client/Bin/Resources/Models/";
	const char* pOutputFilePath = "../../Client/Bin/Resources/BinaryModels/";


	if (!filesystem::exists(pOutputFilePath))
	{
		filesystem::create_directories(pOutputFilePath);
	}


	uint32_t iFlag = { /*aiProcess_GlobalScale | aiProcess_PreTransformVertices |*/ aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
	bool result = { false };
	for (const auto& entry : filesystem::recursive_directory_iterator(pInputFilePath))
	{
		if (filesystem::is_regular_file(entry.path()))
		{
			if (entry.path().extension() == ".fbx" || entry.path().extension() == ".FBX")
			{
				filesystem::path relativePath = filesystem::relative(entry.path(), pInputFilePath);

				filesystem::path exportPath = pOutputFilePath / relativePath;
				exportPath.replace_extension(".dat");

				filesystem::create_directories(exportPath.parent_path());

				result = Convert_Binary(entry.path().string(), exportPath.string());

				if (result == false)
				{
					cout << "----------------FAIL-------------------" << endl;
				}
			} 
		}
	}
	if (result == true)
	{
		cout << "----------------FIN-------------------" << endl;
	}

}
int IsAnim(string fbxPath)
{
	Assimp::Importer tmpImporter = {};
	const aiScene* tmpScene = tmpImporter.ReadFile(fbxPath, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	if (tmpScene == nullptr)
	{
		cout << "FAEILED TO LOAD m_pAIScene" << endl;
		return -1;
	}

	if (tmpScene->HasAnimations() == true)
		return 1;

	for(uint32_t i =0; i< tmpScene->mNumMeshes; i++)
	{
		if (tmpScene->mMeshes[i]->HasBones())
			return 1;
	}



	return 0;
}

static const aiScene* LoadScene_Assimp(Assimp::Importer& importer, const string fbxPath, bool IsAnim, uint32_t iFlag = 0)
{
	uint32_t flags =
		aiProcess_ConvertToLeftHanded		|	/* 왼손 좌표계 변경 */
		aiProcessPreset_TargetRealtime_Fast |   /* 실시간 렌더링 최적화를 위한 플래그 */
		aiProcess_Triangulate				|	/* 폴리곤 삼각형 */
		aiProcess_JoinIdenticalVertices		|	/* 중복 제거 */
		aiProcess_GenNormals;                   /* NORMAL 없으면 생성하기 */

	if(IsAnim == false)
	{
		flags |= aiProcess_PreTransformVertices;
	}
	return importer.ReadFile(fbxPath, flags);

}

bool Write_Model(const aiScene* scene, ofstream& OutFile, bool bIsAnim)
{
	

	Cvt_Header header = {};
	header.iMagic = 0x4D534842;//MSHB
	header.bIsAnim = bIsAnim ? 1 : 0;
	header.iNumMeshes = scene->mNumMeshes;
	header.iNumMaterial = scene->mNumMaterials;

	// 저장
	OutFile.write((char*)&header, sizeof(Cvt_Header));

	// 메쉬 //
	Cvt_MeshInfo meshInfo = {};
	for(size_t i = 0; i < header.iNumMeshes; i++)
	{
		aiMesh* pAIMesh = scene->mMeshes[i];

		strncpy_s(meshInfo.szName, pAIMesh->mName.C_Str(), 63);//strncpy_s 파일 이름이 63 개보다 많으면 짤리지만 들어감
		meshInfo.iNumVertices = pAIMesh->mNumVertices;
		meshInfo.iMaterialIndex = pAIMesh->mMaterialIndex;

		meshInfo.iNumIndices = pAIMesh->mNumFaces*3; // 삼각형 가정

		// 저장
		OutFile.write((char*)&meshInfo, sizeof(Cvt_MeshInfo));


		Cvt_VTXMESH vtx = {};

		// 버텍스 //
		vector<Cvt_VTXMESH> vecVertices;
		vecVertices.reserve(meshInfo.iNumVertices);

		for(size_t j =0; j< meshInfo.iNumVertices; j++)
		{

			memcpy(&vtx.vPos, &pAIMesh->mVertices[j], sizeof(float) * 3);

			for (uint32_t i = 0; i < 8; ++i) {
				if (pAIMesh->HasTextureCoords(i)) {
					// i번 채널의 j번째 정점 데이터를 우리 구조체 vUV[i]에 복사
					vtx.vUV[i][0] = pAIMesh->mTextureCoords[i][j].x;
					vtx.vUV[i][1] = pAIMesh->mTextureCoords[i][j].y;
				}
			}
			if (pAIMesh->HasNormals())
			memcpy(&vtx.vNormal, &pAIMesh->mNormals[j], sizeof(float) * 3);
			if (pAIMesh->HasTangentsAndBitangents())
			memcpy(&vtx.vTangent, &pAIMesh->mTangents[j], sizeof(float) * 3);

			if (bIsAnim == true) 
			{
			
			}

			vecVertices.push_back(vtx);
		}
		OutFile.write((char*)vecVertices.data(), sizeof(Cvt_VTXMESH) * meshInfo.iNumVertices);

		// 인덱스 //
		vector<uint32_t> vecIndices;
		vecIndices.reserve(meshInfo.iNumIndices);
		for (uint32_t f = 0; f < pAIMesh->mNumFaces; ++f)
		{
			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[0]);
			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[1]);
			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[2]);

		}

		OutFile.write((char*)vecIndices.data(), sizeof(uint32_t) * meshInfo.iNumIndices);
	}


	cout << "SUCCESS CONVERT MESH" << endl;
	return true;


}

bool GetPath(aiMaterial* pAIMat, aiTextureType type, char* pOutPath, const char* szDrive, const char* szDir)
{
	aiString aiPath;
	if (pAIMat->GetTexture(type, 0, &aiPath) == AI_SUCCESS)
	{
		/*string fileName = std::filesystem::path(aiPath.C_Str()).filename().string();
		strncpy_s(pOutPath, _MAX_PATH, fileName.c_str(), _TRUNCATE);
		*/

		string fullPath = aiPath.C_Str();
		// 단순히 글자들을 뒤져서 '\'나 '/'가 나오는 위치를 찾음 (OS 규칙 안 따짐)
		size_t lastSlash = fullPath.find_last_of("\\/");
		string fileName = fullPath.substr(lastSlash + 1);


		char szFinalPath[MAX_PATH] = {};
		strcpy_s(szFinalPath, MAX_PATH, szDrive);
		strcat_s(szFinalPath, MAX_PATH, szDir);
		strcat_s(szFinalPath, MAX_PATH, fileName.c_str());



		strncpy_s(pOutPath, _MAX_PATH, fileName.c_str(), _TRUNCATE);
		return true;
	}


	return false;
	
}


bool Write_Texture(const aiScene* scene, ofstream& OutFile, const string& strInFilePath)
{

	uint32_t iNumMaterials = scene->mNumMaterials;


	char szDrive[MAX_PATH] = {}, szDir[MAX_PATH] = {};
	_splitpath_s(strInFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	vector<Cvt_Material> vecMat;
	vecMat.reserve(iNumMaterials);

	for (size_t i = 0; i < iNumMaterials; i++)
	{
		Cvt_Material mat = {};
		aiMaterial* pAIMat = scene->mMaterials[i];

		aiString matName;
		pAIMat->Get(AI_MATKEY_NAME, matName);
		strncpy_s(mat.szName, matName.C_Str(), _TRUNCATE);

		GetPath(pAIMat, aiTextureType_DIFFUSE, mat.szDiffusePath, szDrive, szDir);// 색상
		GetPath(pAIMat, aiTextureType_NORMALS, mat.szNormalPath, szDrive, szDir); // 노멀
		GetPath(pAIMat, aiTextureType_SPECULAR, mat.szSpecularPath, szDrive, szDir);// 광택
		GetPath(pAIMat, aiTextureType_OPACITY, mat.szOpacityPath, szDrive, szDir);// 투명도 (유리 등)


		vecMat.push_back(mat);
	}

	OutFile.write((char*)vecMat.data(), sizeof(Cvt_Material) * vecMat.size());

	cout << "SUCCESS CONVERT TEX" << endl;
	return true;


	
	
}





bool Convert_Binary(string fbxPath, string exportPath)
{
	
	int bIsAnim = IsAnim(fbxPath);
	if(bIsAnim == -1)
	{
		return false;
	}

	Assimp::Importer Importer = {};
	const aiScene* AIScene = LoadScene_Assimp(Importer, fbxPath, bIsAnim);
		if (nullptr == AIScene)
			return false;

		ofstream OutFile(exportPath, ios::binary);
		if (!OutFile.is_open()) return false;


		if(false == Write_Model(AIScene, OutFile, bIsAnim))
		{
			return false;
		}
		if (false == Write_Texture(AIScene, OutFile, fbxPath))
		{
			return false;
		}

		cout << "done" << endl;

		return true;
}