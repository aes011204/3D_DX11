
#include "Converter_Define.h"
#include "Converter_function.h"

bool Convert_Binary(string fbxPath, string exportPath);

vector<Cvt_Bone> g_vecBones;
vector<Cvt_VTXMESH> g_vecVertices;


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

				cout << "Checking: " << entry.path().string() << endl;

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

	if (tmpScene && tmpScene->mNumAnimations > 0)
		return 1;

	if (tmpScene->HasAnimations() == true)
		return 1;

	for (uint32_t i = 0; i < tmpScene->mNumMeshes; i++)
	{
		if (tmpScene->mMeshes[i]->HasBones())
			return 1;
	}
	if (tmpScene == nullptr) return -1;

	// 핵심: 애니메이션 바구니(Stack)가 있고, 그 바구니 안에 실제 채널(Keyframe)이 있는지 확인
	//if (tmpScene->mNumAnimations > 0)
	//{
	//	for (unsigned int i = 0; i < tmpScene->mNumAnimations; ++i)
	//	{
	//		// 실제 애니메이션 채널(뼈대의 움직임 기록)이 하나라도 들어있어야 진짜 애니메이션 모델임
	//		if (tmpScene->mAnimations[i]->mNumChannels > 0)
	//			return 1;
	//	}
	//}


	return 0;
}

static const aiScene* LoadScene_Assimp(Assimp::Importer& importer, const string fbxPath, bool IsAnim, uint32_t iFlag = 0)
{
	uint32_t flags =
		aiProcess_ConvertToLeftHanded |	/* 왼손 좌표계 변경 */
		aiProcessPreset_TargetRealtime_Fast |   /* 실시간 렌더링 최적화를 위한 플래그 */
		aiProcess_Triangulate |	/* 폴리곤 삼각형 */
		aiProcess_JoinIdenticalVertices |	/* 중복 제거 */
		aiProcess_GenNormals;                   /* NORMAL 없으면 생성하기 */

	if (IsAnim == false)
	{
		flags |= aiProcess_PreTransformVertices;
	}
	return importer.ReadFile(fbxPath, flags);

}

bool Write_Model(const aiScene* scene, ofstream& OutFile, bool bIsAnim)
{



	// 메쉬 //
	Cvt_MeshInfo meshInfo = {};
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		g_vecVertices.clear();

		aiMesh* pAIMesh = scene->mMeshes[i];

		strncpy_s(meshInfo.szName, pAIMesh->mName.C_Str(), MAX_PATH);//strncpy_s 파일 이름이 63 개보다 많으면 짤리지만 들어감
		meshInfo.iNumVertices = pAIMesh->mNumVertices;
		meshInfo.iMaterialIndex = pAIMesh->mMaterialIndex;

		meshInfo.iNumIndices = pAIMesh->mNumFaces * 3; // 삼각형 가정

		meshInfo.iNumBones = pAIMesh->mNumBones;

		// 저장
		OutFile.write((char*)&meshInfo, sizeof(Cvt_MeshInfo));



		Cvt_VTXMESH vtx = {};
		// 버텍스 //

		g_vecVertices.reserve(meshInfo.iNumVertices);

		for (size_t j = 0; j < meshInfo.iNumVertices; j++)
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


			// 초기화
			for (int k = 0; k < 4; ++k)
			{
				vtx.vIndices[k] = -1;
				vtx.vWeights[k] = 0.f;
			}

			g_vecVertices.push_back(vtx);
		}

		// 가중치, 인덱스 //

		vector<Cvt_BoneAdd> m_VecBoneAdd;

		for (size_t i = 0; i < pAIMesh->mNumBones; i++)
		{

			aiBone* pAIBone = pAIMesh->mBones[i];
			int iBoneIndex = -1;

			for (int b = 0; b < g_vecBones.size(); ++b)
			{
				if (strcmp(g_vecBones[b].szName, pAIBone->mName.C_Str()) == 0)
				{
					iBoneIndex = b;

					aiMatrix4x4 tmpOffsetMat = pAIBone->mOffsetMatrix;
					tmpOffsetMat = tmpOffsetMat.Transpose();
					memcpy(&g_vecBones[b].OffsetMatrix, &tmpOffsetMat, sizeof(float) * 16);


					Cvt_BoneAdd boneAdd = {};
					boneAdd.iBoneIndex = iBoneIndex;
					memcpy(&boneAdd.OffsetMatrix, &g_vecBones[b].OffsetMatrix, sizeof(float) * 16);
					m_VecBoneAdd.push_back(boneAdd);

					break;
				}
			}
			if (-1 == iBoneIndex)
				return E_FAIL;

			//pAIBone->mNumWeights 이 뼈가 몇개의 정점에 영향을 주는가
			for (uint32_t w = 0; w < pAIBone->mNumWeights; ++w)
			{
				uint32_t iVertexID = pAIBone->mWeights[w].mVertexId;
				float fWeight = pAIBone->mWeights[w].mWeight;

				// 해당 정점의 빈 슬롯(4칸 중 하나) 찾기
				for (int k = 0; k < 4; ++k)
				{
					if (g_vecVertices[iVertexID].vIndices[k] == -1)
					{
						g_vecVertices[iVertexID].vIndices[k] = i;
						g_vecVertices[iVertexID].vWeights[k] = fWeight;
						break; // 한 칸 채웠으면 다음 정점으로
					}
				}
			}


		}

		if (!m_VecBoneAdd.empty())
			OutFile.write((char*)m_VecBoneAdd.data(), sizeof(Cvt_BoneAdd) * m_VecBoneAdd.size());

		OutFile.write((char*)g_vecVertices.data(), sizeof(Cvt_VTXMESH) * meshInfo.iNumVertices);


	



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


};
bool Ready_Bones(const aiNode* pAINode, unsigned int iParentBoneIndex, ofstream& OutFile)
{
	/*shared_ptr<CAssimp_Bone> pBone = CAssimp_Bone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;*/

	Cvt_Bone Bone = {};
	strcpy_s(Bone.szName, pAINode->mName.data);

	Bone.iParentBoneIndex = iParentBoneIndex;

	aiMatrix4x4 tmpsetMat = pAINode->mTransformation;
	tmpsetMat = tmpsetMat.Transpose();
	memcpy(&Bone.TransformationMatrix, &tmpsetMat, sizeof(float) * 16);


	aiMatrix4x4 identity = aiMatrix4x4(); // 이건 일단 항등 행렬 메쉬에서 처리해야함
	memcpy(&Bone.OffsetMatrix, &identity, sizeof(float) * 16);

	g_vecBones.push_back(Bone);

	int iPIndex = g_vecBones.size() - 1; // 부모인덱스 = 백터 사이즈 -1 


	//OutFile.write((char*)&Bone, sizeof(Cvt_Bone));	g_vecBones.

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iPIndex, OutFile);
	}

	return true;
}



bool Write_Channel(const aiNodeAnim* pAIChannel, ofstream& OutFile)
{

	Cvt_Channel channelDesc = {};

	unsigned int	m_iNumKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumScalingKeys);
	channelDesc.iNumKeyframes = m_iNumKeyFrames;



	for (int i = 0; i < g_vecBones.size(); i++)
	{
		if (strcmp(g_vecBones[i].szName, pAIChannel->mNodeName.data) == 0)
		{
			channelDesc.iBoneIndex = i;
			strncpy_s(channelDesc.szBoneName, pAIChannel->mNodeName.C_Str(), _TRUNCATE);
		}

	};
	OutFile.write((char*)&channelDesc, sizeof(Cvt_Channel));

	{

		float vScale[3] = {};
		float qRotation[4] = {};
		float vTranslation[3] = {};

		for (size_t i = 0;i < channelDesc.iNumKeyframes;i++)
		{
			Cvt_Keyframe KeyFrame = {};
			if (pAIChannel->mNumScalingKeys > i)
			{
				memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(float) * 3);
				KeyFrame.dTrackPosition = pAIChannel->mScalingKeys[i].mTime; // 이거 실제 시간이 아님
			}
			if (pAIChannel->mNumRotationKeys > i)
			{
				qRotation[0] = pAIChannel->mRotationKeys[i].mValue.x;
				qRotation[1] = pAIChannel->mRotationKeys[i].mValue.y;
				qRotation[2] = pAIChannel->mRotationKeys[i].mValue.z;
				qRotation[3] = pAIChannel->mRotationKeys[i].mValue.w;

				KeyFrame.dTrackPosition = pAIChannel->mRotationKeys[i].mTime;
			}
			if (pAIChannel->mNumPositionKeys > i)
			{
				memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(float) * 3);
				KeyFrame.dTrackPosition = pAIChannel->mPositionKeys[i].mTime; // 이거 실제 시간이 아님
			}

			memcpy(KeyFrame.vScale, vScale, sizeof(float) * 3);
			memcpy(KeyFrame.qRotation, qRotation, sizeof(float) * 4);
			memcpy(KeyFrame.vPos, vTranslation, sizeof(float) * 3);

			OutFile.write((char*)&KeyFrame, sizeof(Cvt_Keyframe));
		}

	}
	return true;
}

bool Write_Animation(const aiScene* AIScene, ofstream& OutFile)
{
	for (size_t i = 0; i < AIScene->mNumAnimations; i++)
	{
		aiAnimation* pAIAnimation = AIScene->mAnimations[i];

		Cvt_Animation animationDesc = {};
		strncpy_s(animationDesc.szName,pAIAnimation->mName.C_Str(),MAX_PATH);
		//memcpy(&animationDesc.szName, &pAIAnimation->mName, sizeof(MAX_PATH));
		animationDesc.dDuration = pAIAnimation->mDuration;
		animationDesc.dTickPerSecond = pAIAnimation->mTicksPerSecond;
		animationDesc.iNumChannels = pAIAnimation->mNumChannels;


		OutFile.write((char*)&animationDesc, sizeof(Cvt_Animation));


		for (size_t i = 0;i < animationDesc.iNumChannels;i++)
		{

			Write_Channel(pAIAnimation->mChannels[i], OutFile);
		}
	}
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



bool Write_Texture_vector(const aiScene* scene, ofstream& OutFile, const string& strInFilePath)
{
	// 재질의 총 개수 기록
	uint32_t iNumMaterials = scene->mNumMaterials;

	cout << "NumMaterials : " << iNumMaterials << endl;
	// 경로 조립용 정보
	char szDrive[MAX_PATH] = {}, szDir[MAX_PATH] = {};
	_splitpath_s(strInFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	for (size_t i = 0; i < iNumMaterials; i++)
	{
		aiMaterial* pAIMat = scene->mMaterials[i];

		// 재질 이름 저장 (고정 크기)
		aiString matName;
		pAIMat->Get(AI_MATKEY_NAME, matName);
		char szMatName[MAX_PATH] = {};
		strncpy_s(szMatName, matName.C_Str(), _TRUNCATE);
		OutFile.write(szMatName, MAX_PATH);

		// 
		for (size_t j = 0; j < MAX_TEXTURE_SLOT; j++)
		{
			// [확인용 로그] 만약 여기서 iNumTextures가 계속 0이면 FBX 문제임
			if (iNumMaterials == 0 && j == aiTextureType_DIFFUSE) {
				cout << "이 모델은 Diffuse 텍스처 정보가 아예 없음!" << endl;
			}
			uint32_t iNumTextures = pAIMat->GetTextureCount(static_cast<aiTextureType>(j));

			// 이 타입(j)의 텍스처 개수를 먼저 기록
			OutFile.write((char*)&iNumTextures, sizeof(uint32_t));

			if (iNumTextures != 0)
				cout << iNumTextures << " / " << MAX_TEXTURE_SLOT << "  this type's tex Num" << iNumTextures << endl;

			for (size_t k = 0; k < iNumTextures; k++)
			{
				aiString aiPath;
				pAIMat->GetTexture(static_cast<aiTextureType>(j), k, &aiPath);

				// 경로 조립 (파일명만 추출해서 모델 폴더 경로와 합침)
				string fullPath = aiPath.C_Str();
				size_t lastSlash = fullPath.find_last_of("\\/");
				string fileName = (lastSlash == string::npos) ? fullPath : fullPath.substr(lastSlash + 1);

				//std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

				char szFinalPath[MAX_PATH] = {};
				strcpy_s(szFinalPath, MAX_PATH, szDrive);
				strcat_s(szFinalPath, MAX_PATH, szDir);
				strcat_s(szFinalPath, MAX_PATH, fileName.c_str());


				// 완성된 경로 기록
				OutFile.write(szFinalPath, MAX_PATH);
				cout << szFinalPath << endl;
			}

		}
	}
	cout << "SUCCESS CONVERT TEX" << endl;
	return true;
}
bool Convert_Binary(string fbxPath, string exportPath)
{

	int bIsAnim = IsAnim(fbxPath);
	if (bIsAnim == -1)
	{
		return false; // 1이몀 애니메이션 2면 넌애님
	}

	Assimp::Importer Importer = {};
	const aiScene* AIScene = LoadScene_Assimp(Importer, fbxPath, bIsAnim);
	if (nullptr == AIScene)
		return false;

	ofstream OutFile(exportPath, ios::binary);
	if (!OutFile.is_open()) return false;

	g_vecBones.clear();
	if (false == Ready_Bones(AIScene->mRootNode, -1, OutFile))
	{
		return false;
	}


	Cvt_Header header = {};
	header.iMagic = 0x4D534842;//MSHB
	header.bIsAnim = bIsAnim ? 1 : 0;
	header.iNumMeshes = AIScene->mNumMeshes;
	header.iNumMaterial = AIScene->mNumMaterials;
	header.iNumAnimation = AIScene->mNumAnimations;
	header.iTotalNumBone = g_vecBones.size();
	// 저장
	OutFile.write((char*)&header, sizeof(Cvt_Header));

	// 본을 여기서 저장
	OutFile.write((char*)g_vecBones.data(), sizeof(Cvt_Bone) * g_vecBones.size());



	if (false == Write_Model(AIScene, OutFile, bIsAnim))
	{
		return false;
	}
	if (bIsAnim&&false == Write_Animation(AIScene, OutFile))
	{
		return false;
	}

	if (false == Write_Texture_vector(AIScene, OutFile, fbxPath))
	{
		return false;
	}

	cout << "done" << endl;

	return true;
}