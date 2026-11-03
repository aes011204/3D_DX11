#include  "Converter_Define.h"
#include "Converter_function.h"

bool Convert_Binary(string fbxPath, string exportPath);

vector<Cvt_Bone> g_vecBones;
vector<Cvt_VTXMESH> g_vecVertices;


int main()
{
	const char* inputFilePath = "../../Client/Bin/Resources/Models/";
	const char* outputFilePath = "../../Client/Bin/Resources/BinaryModels/";


	if (!filesystem::exists(outputFilePath))
	{
		filesystem::create_directories(outputFilePath);
	}


	uint32_t flag = { /*aiProcess_GlobalScale | aiProcess_PreTransformVertices |*/ aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };
	bool result = { false };
	for (const auto& entry : filesystem::recursive_directory_iterator(inputFilePath))
	{

		if (filesystem::is_regular_file(entry.path()))
		{
			if (entry.path().extension() == ".fbx" || entry.path().extension() == ".FBX")
			{

				cout << "Checking: " << entry.path().string() << endl;

				filesystem::path relativePath = filesystem::relative(entry.path(), inputFilePath);

				filesystem::path exportPath = outputFilePath / relativePath;
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

static const aiScene* LoadScene_Assimp(Assimp::Importer& importer, const string fbxPath, bool IsAnim, uint32_t flag = 0)
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

//bool Write_Model(const aiScene* scene, ofstream& OutFile, bool bIsAnim)
//{
//
//
//
//	// 메쉬 //
//	Cvt_MeshInfo meshInfo = {};
//	for (size_t i = 0; i < scene->mNumMeshes; i++)
//	{
//		g_vecVertices.clear();
//
//		aiMesh* pAIMesh = scene->mMeshes[i];
//
//		strncpy_s(meshInfo.szName, pAIMesh->mName.C_Str(), MAX_PATH);//strncpy_s 파일 이름이 63 개보다 많으면 짤리지만 들어감
//		meshInfo.iNumVertices = pAIMesh->mNumVertices;
//		meshInfo.iMaterialIndex = pAIMesh->mMaterialIndex;
//
//		meshInfo.iNumIndices = pAIMesh->mNumFaces * 3; // 삼각형 가정
//
//		meshInfo.iNumBones = pAIMesh->mNumBones;
//
//		// 저장
//		OutFile.write((char*)&meshInfo, sizeof(Cvt_MeshInfo));
//
//
//
//		Cvt_VTXMESH vtx = {};
//		// 버텍스 //
//
//		g_vecVertices.reserve(meshInfo.iNumVertices);
//
//		for (size_t j = 0; j < meshInfo.iNumVertices; j++)
//		{
//
//			memcpy(&vtx.vPos, &pAIMesh->mVertices[j], sizeof(float) * 3);
//
//			for (uint32_t i = 0; i < 8; ++i) {
//				if (pAIMesh->HasTextureCoords(i)) {
//					// i번 채널의 j번째 정점 데이터를 우리 구조체 vUV[i]에 복사
//					vtx.vUV[i][0] = pAIMesh->mTextureCoords[i][j].x;
//					vtx.vUV[i][1] = pAIMesh->mTextureCoords[i][j].y;
//				}
//			}
//			if (pAIMesh->HasNormals())
//				memcpy(&vtx.vNormal, &pAIMesh->mNormals[j], sizeof(float) * 3);
//			if (pAIMesh->HasTangentsAndBitangents())
//				memcpy(&vtx.vTangent, &pAIMesh->mTangents[j], sizeof(float) * 3);
//
//
//			// 초기화
//			for (int k = 0; k < 4; ++k)
//			{
//				vtx.vIndices[k] = -1;
//				vtx.vWeights[k] = 0.f;
//			}
//
//			g_vecVertices.push_back(vtx);
//		}
//
//		// 가중치, 인덱스 //
//
//		vector<Cvt_BoneAdd> m_VecBoneAdd;
//
//		for (size_t i = 0; i < pAIMesh->mNumBones; i++)
//		{
//
//			aiBone* pAIBone = pAIMesh->mBones[i];
//			int iBoneIndex = -1;
//
//			for (int b = 0; b < g_vecBones.size(); ++b)
//			{
//				if (strcmp(g_vecBones[b].szName, pAIBone->mName.C_Str()) == 0)
//				{
//					iBoneIndex = b;
//
//					aiMatrix4x4 tmpOffsetMat = pAIBone->mOffsetMatrix;
//					tmpOffsetMat = tmpOffsetMat.Transpose();
//					memcpy(&g_vecBones[b].OffsetMatrix, &tmpOffsetMat, sizeof(float) * 16);
//
//
//					Cvt_BoneAdd boneAdd = {};
//					boneAdd.iBoneIndex = iBoneIndex;
//					memcpy(&boneAdd.OffsetMatrix, &g_vecBones[b].OffsetMatrix, sizeof(float) * 16);
//					m_VecBoneAdd.push_back(boneAdd);
//
//					break;
//				}
//			}
//			if (-1 == iBoneIndex)
//				return E_FAIL;
//
//			//pAIBone->mNumWeights 이 뼈가 몇개의 정점에 영향을 주는가
//			for (uint32_t w = 0; w < pAIBone->mNumWeights; ++w)
//			{
//				uint32_t iVertexID = pAIBone->mWeights[w].mVertexId;
//				float fWeight = pAIBone->mWeights[w].mWeight;
//
//				// 해당 정점의 빈 슬롯(4칸 중 하나) 찾기
//				for (int k = 0; k < 4; ++k)
//				{
//					if (g_vecVertices[iVertexID].vIndices[k] == -1)
//					{
//						g_vecVertices[iVertexID].vIndices[k] = i;
//						g_vecVertices[iVertexID].vWeights[k] = fWeight;
//						break; // 한 칸 채웠으면 다음 정점으로
//					}
//				}
//			}
//
//
//		}
//
//		if (!m_VecBoneAdd.empty())
//			OutFile.write((char*)m_VecBoneAdd.data(), sizeof(Cvt_BoneAdd) * m_VecBoneAdd.size());
//
//		OutFile.write((char*)g_vecVertices.data(), sizeof(Cvt_VTXMESH) * meshInfo.iNumVertices);
//
//
//
//
//
//
//		// 인덱스 //
//		vector<uint32_t> vecIndices;
//		vecIndices.reserve(meshInfo.iNumIndices);
//		for (uint32_t f = 0; f < pAIMesh->mNumFaces; ++f)
//		{
//			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[0]);
//			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[1]);
//			vecIndices.push_back(pAIMesh->mFaces[f].mIndices[2]);
//
//		}
//
//		OutFile.write((char*)vecIndices.data(), sizeof(uint32_t) * meshInfo.iNumIndices);
//
//
//
//	}
//
//	cout << "SUCCESS CONVERT MESH" << endl;
//	return true;
//
//
//};

bool Write_Model(const aiScene* scene, ofstream& OutFile, bool isAnim)
{
	Cvt_MeshInfo meshInfo = {};

	for (size_t meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++)
	{
		g_vecVertices.clear();

		aiMesh* srcAIMesh = scene->mMeshes[meshIdx];

		strncpy_s(meshInfo.name, srcAIMesh->mName.C_Str(), MAX_PATH);
		meshInfo.numVertices = srcAIMesh->mNumVertices;
		meshInfo.materialIndex = srcAIMesh->mMaterialIndex;
		meshInfo.numIndices = srcAIMesh->mNumFaces * 3;
		meshInfo.numBones = 0;

		// -----------------------------
		// 버텍스 생성
		// -----------------------------
		g_vecVertices.reserve(meshInfo.numVertices);

		for (size_t j = 0; j < meshInfo.numVertices; j++)
		{
			Cvt_VTXMESH vtx = {};

			memcpy(&vtx.pos, &srcAIMesh->mVertices[j], sizeof(float) * 3);

			for (uint32_t uv = 0; uv < 8; ++uv)
			{
				if (srcAIMesh->HasTextureCoords(uv))
				{
					vtx.uv[uv][0] = srcAIMesh->mTextureCoords[uv][j].x;
					vtx.uv[uv][1] = srcAIMesh->mTextureCoords[uv][j].y;
				}
			}

			if (srcAIMesh->HasNormals())
				memcpy(&vtx.normal, &srcAIMesh->mNormals[j], sizeof(float) * 3);

			if (srcAIMesh->HasTangentsAndBitangents())
				memcpy(&vtx.tangent, &srcAIMesh->mTangents[j], sizeof(float) * 3);

			for (int k = 0; k < 4; ++k)
			{
				vtx.indices[k] = -1;
				vtx.weights[k] = 0.f;
			}

			g_vecVertices.push_back(vtx);
		}

		// -----------------------------
		// Bone 처리 + Remap
		// -----------------------------
		vector<Cvt_BoneAdd> m_VecBoneAdd;
		vector<int> boneRemap(srcAIMesh->mNumBones, -1);

		for (size_t boneIdx = 0; boneIdx < srcAIMesh->mNumBones; boneIdx++)
		{
			aiBone* srcAIBone = srcAIMesh->mBones[boneIdx];

			int globalBoneIndex = -1;

			for (int b = 0; b < g_vecBones.size(); ++b)
			{
				if (strcmp(g_vecBones[b].name, srcAIBone->mName.C_Str()) == 0)
				{
					globalBoneIndex = b;

					aiMatrix4x4 offset = srcAIBone->mOffsetMatrix;
					offset = offset.Transpose();

					memcpy(&g_vecBones[b].offsetMatrix, &offset, sizeof(float) * 16);

					Cvt_BoneAdd boneAdd = {};
					boneAdd.boneIndex = globalBoneIndex;
					memcpy(&boneAdd.offsetMatrix, &g_vecBones[b].offsetMatrix, sizeof(float) * 16);

					int mappedIndex = (int)m_VecBoneAdd.size();
					m_VecBoneAdd.push_back(boneAdd);

					boneRemap[boneIdx] = mappedIndex; 

					break;
				}
			}

			if (globalBoneIndex == -1)
				return E_FAIL;
		}

		// -----------------------------
		// Weight 적용 
		// -----------------------------
		for (size_t boneIdx = 0; boneIdx < srcAIMesh->mNumBones; boneIdx++)
		{
			aiBone* srcAIBone = srcAIMesh->mBones[boneIdx];

			int mappedIndex = boneRemap[boneIdx];
			if (mappedIndex == -1)
				continue;

			for (uint32_t w = 0; w < srcAIBone->mNumWeights; ++w)
			{
				uint32_t vtxID = srcAIBone->mWeights[w].mVertexId;
				float weight = srcAIBone->mWeights[w].mWeight;

				if (vtxID >= g_vecVertices.size())
					continue;

				int minIndex = 0;
				float minWeight = g_vecVertices[vtxID].weights[0];

				for (int k = 1; k < 4; k++)
				{
					if (g_vecVertices[vtxID].weights[k] < minWeight)
					{
						minWeight = g_vecVertices[vtxID].weights[k];
						minIndex = k;
					}
				}

				if (g_vecVertices[vtxID].indices[minIndex] == -1 || weight > minWeight)
				{
					g_vecVertices[vtxID].indices[minIndex] = mappedIndex;
					g_vecVertices[vtxID].weights[minIndex] = weight;
				}
			}
		}

		// -----------------------------
		// weight 보정
		// -----------------------------
		for (auto& v : g_vecVertices)
		{
			float sum = 0.f;
			for (int k = 0; k < 4; k++)
				sum += v.weights[k];

			if (sum == 0.f)
			{
				v.indices[0] = 0;
				v.weights[0] = 1.f;
			}
			else
			{
				for (int k = 0; k < 4; k++)
					v.weights[k] /= sum;
			}

			for (int k = 0; k < 4; k++)
			{
				if (v.indices[k] < 0)
				{
					v.indices[k] = 0;
					v.weights[k] = 0.f;
				}
			}
		}

		// -----------------------------
		// 저장
		// -----------------------------
		meshInfo.numBones = (uint32_t)m_VecBoneAdd.size();

		OutFile.write((char*)&meshInfo, sizeof(Cvt_MeshInfo));

		if (!m_VecBoneAdd.empty())
			OutFile.write((char*)m_VecBoneAdd.data(), sizeof(Cvt_BoneAdd) * meshInfo.numBones);

		OutFile.write((char*)g_vecVertices.data(), sizeof(Cvt_VTXMESH) * meshInfo.numVertices);

		// -----------------------------
		// 인덱스
		// -----------------------------
		vector<uint32_t> vecIndices;
		vecIndices.reserve(meshInfo.numIndices);

		for (uint32_t f = 0; f < srcAIMesh->mNumFaces; ++f)
		{
			vecIndices.push_back(srcAIMesh->mFaces[f].mIndices[0]);
			vecIndices.push_back(srcAIMesh->mFaces[f].mIndices[1]);
			vecIndices.push_back(srcAIMesh->mFaces[f].mIndices[2]);
		}

		OutFile.write((char*)vecIndices.data(), sizeof(uint32_t) * meshInfo.numIndices);
	}

	cout << "SUCCESS CONVERT MESH" << endl;
	return true;
}

bool Ready_Bones(const aiNode* srcAINode, unsigned int parentBoneIndex, ofstream& OutFile)
{
	/*shared_ptr<CAssimp_Bone> pBone = CAssimp_Bone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;*/

	Cvt_Bone Bone = {};
	strcpy_s(Bone.name, srcAINode->mName.data);

	Bone.parentBoneIndex = parentBoneIndex;

	aiMatrix4x4 tmpsetMat = srcAINode->mTransformation;
	tmpsetMat = tmpsetMat.Transpose();
	memcpy(&Bone.transformationMatrix, &tmpsetMat, sizeof(float) * 16);


	aiMatrix4x4 identity = aiMatrix4x4(); // 이건 일단 항등 행렬 메쉬에서 처리해야함
	memcpy(&Bone.offsetMatrix, &identity, sizeof(float) * 16);

	g_vecBones.push_back(Bone);

	int curBoneIndex = g_vecBones.size() - 1; // 부모인덱스 = 백터 사이즈 -1 


	//OutFile.write((char*)&Bone, sizeof(Cvt_Bone));	g_vecBones.

	for (size_t i = 0; i < srcAINode->mNumChildren; i++)
	{
		Ready_Bones(srcAINode->mChildren[i], curBoneIndex, OutFile);
	}

	return true;
}



bool Write_Channel(const aiNodeAnim* srcAIChannel, ofstream& OutFile)
{

	Cvt_Channel channelDesc = {};

	unsigned int	m_numKeyFrames = max(srcAIChannel->mNumPositionKeys, srcAIChannel->mNumRotationKeys);
	m_numKeyFrames = max(m_numKeyFrames, srcAIChannel->mNumScalingKeys);
	channelDesc.numKeyframes = m_numKeyFrames;



	for (int i = 0; i < g_vecBones.size(); i++)
	{
		if (strcmp(g_vecBones[i].name, srcAIChannel->mNodeName.data) == 0)
		{
			channelDesc.boneIndex = i;
			strncpy_s(channelDesc.boneName, srcAIChannel->mNodeName.C_Str(), _TRUNCATE);
		}

	};
	OutFile.write((char*)&channelDesc, sizeof(Cvt_Channel));

	{

		float scale[3] = {};
		float rotation[4] = {};
		float translation[3] = {};

		for (size_t i = 0; i < channelDesc.numKeyframes; i++)
		{
			Cvt_Keyframe keyFrame = {};
			if (srcAIChannel->mNumScalingKeys > i)
			{
				memcpy(&scale, &srcAIChannel->mScalingKeys[i].mValue, sizeof(float) * 3);
				keyFrame.trackPosition = srcAIChannel->mScalingKeys[i].mTime; // 이거 실제 시간이 아님
			}
			if (srcAIChannel->mNumRotationKeys > i)
			{
				rotation[0] = srcAIChannel->mRotationKeys[i].mValue.x;
				rotation[1] = srcAIChannel->mRotationKeys[i].mValue.y;
				rotation[2] = srcAIChannel->mRotationKeys[i].mValue.z;
				rotation[3] = srcAIChannel->mRotationKeys[i].mValue.w;

				keyFrame.trackPosition = srcAIChannel->mRotationKeys[i].mTime;
			}
			if (srcAIChannel->mNumPositionKeys > i)
			{
				memcpy(&translation, &srcAIChannel->mPositionKeys[i].mValue, sizeof(float) * 3);
				keyFrame.trackPosition = srcAIChannel->mPositionKeys[i].mTime; // 이거 실제 시간이 아님
			}

			memcpy(keyFrame.scale, scale, sizeof(float) * 3);
			memcpy(keyFrame.rotation, rotation, sizeof(float) * 4);
			memcpy(keyFrame.pos, translation, sizeof(float) * 3);

			OutFile.write((char*)&keyFrame, sizeof(Cvt_Keyframe));
		}

	}
	return true;
}

bool Write_Animation(const aiScene* srcAIScene, ofstream& OutFile)
{
	for (size_t i = 0; i < srcAIScene->mNumAnimations; i++)
	{
		aiAnimation* srcAIAnimation = srcAIScene->mAnimations[i];

		Cvt_Animation animationDesc = {};
		strncpy_s(animationDesc.name, srcAIAnimation->mName.C_Str(), MAX_PATH);
		//memcpy(&animationDesc.szName, &pAIAnimation->mName, sizeof(MAX_PATH));
		animationDesc.duration = srcAIAnimation->mDuration;
		animationDesc.tickPerSecond = srcAIAnimation->mTicksPerSecond;
		animationDesc.numChannels = srcAIAnimation->mNumChannels;


		OutFile.write((char*)&animationDesc, sizeof(Cvt_Animation));


		for (size_t i = 0; i < animationDesc.numChannels; i++)
		{

			Write_Channel(srcAIAnimation->mChannels[i], OutFile);
		}
	}
	return true;
}

bool GetPath(aiMaterial* srcAIMat, aiTextureType type, char* outPath, const char* szDrive, const char* szDir)
{
	aiString aiPath;

	if (srcAIMat->GetTexture(type, 0, &aiPath) == AI_SUCCESS)
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



		strncpy_s(outPath, _MAX_PATH, fileName.c_str(), _TRUNCATE);
		return true;
	}


	return false;

}


bool Write_Texture(const aiScene* scene, ofstream& OutFile, const string& inFilePath)
{

	uint32_t numMaterials = scene->mNumMaterials;


	char szDrive[MAX_PATH] = {}, szDir[MAX_PATH] = {};
	_splitpath_s(inFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	vector<Cvt_Material> vecMat;
	vecMat.reserve(numMaterials);

	for (size_t i = 0; i < numMaterials; i++)
	{
		Cvt_Material mat = {};
		aiMaterial* srcAIMat = scene->mMaterials[i];

		aiString matName;
		srcAIMat->Get(AI_MATKEY_NAME, matName);
		strncpy_s(mat.name, matName.C_Str(), _TRUNCATE);

		GetPath(srcAIMat, aiTextureType_DIFFUSE, mat.diffusePath, szDrive, szDir);// 색상
		GetPath(srcAIMat, aiTextureType_NORMALS, mat.normalPath, szDrive, szDir); // 노멀
		GetPath(srcAIMat, aiTextureType_SPECULAR, mat.specularPath, szDrive, szDir);// 광택
		GetPath(srcAIMat, aiTextureType_OPACITY, mat.opacityPath, szDrive, szDir);// 투명도 (유리 등)


		vecMat.push_back(mat);
	}

	OutFile.write((char*)vecMat.data(), sizeof(Cvt_Material) * vecMat.size());

	cout << "SUCCESS CONVERT TEX" << endl;
	return true;


}



bool Write_Texture_vector(const aiScene* scene, ofstream& OutFile, const string& inFilePath)
{
	// 재질의 총 개수 기록
	uint32_t numMaterials = scene->mNumMaterials;

	cout << "NumMaterials : " << numMaterials << endl;
	// 경로 조립용 정보
	char szDrive[MAX_PATH] = {}, szDir[MAX_PATH] = {};
	_splitpath_s(inFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	for (size_t i = 0; i < numMaterials; i++)
	{
		aiMaterial* srcAIMat = scene->mMaterials[i];

		// 재질 이름 저장 (고정 크기)
		aiString matName;
		srcAIMat->Get(AI_MATKEY_NAME, matName);
		char szMatName[MAX_PATH] = {};
		strncpy_s(szMatName, matName.C_Str(), _TRUNCATE);
		OutFile.write(szMatName, MAX_PATH);

		// 
		for (size_t j = 0; j < MAX_TEXTURE_SLOT; j++)
		{
			// [확인용 로그] 만약 여기서 iNumTextures가 계속 0이면 FBX 문제임
			if (numMaterials == 0 && j == aiTextureType_DIFFUSE) {
				cout << "이 모델은 Diffuse 텍스처 정보가 아예 없음!" << endl;
			}
			uint32_t numTextures = srcAIMat->GetTextureCount(static_cast<aiTextureType>(j));

			// 이 타입(j)의 텍스처 개수를 먼저 기록
			OutFile.write((char*)&numTextures, sizeof(uint32_t));

			if (numTextures != 0)
				cout << numTextures << " / " << MAX_TEXTURE_SLOT << "  this type's tex Num" << numTextures << endl;

			for (size_t k = 0; k < numTextures; k++)
			{
				aiString aiPath;
				srcAIMat->GetTexture(static_cast<aiTextureType>(j), k, &aiPath);

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

	int isAnim = IsAnim(fbxPath);
	if (isAnim == -1)
	{
		return false; // 1이몀 애니메이션 2면 넌애님
	}

	Assimp::Importer Importer = {};
	const aiScene* srcAIScene = LoadScene_Assimp(Importer, fbxPath, isAnim);
	if (nullptr == srcAIScene)
		return false;

	ofstream OutFile(exportPath, ios::binary);
	if (!OutFile.is_open()) return false;

	g_vecBones.clear();
	if (false == Ready_Bones(srcAIScene->mRootNode, -1, OutFile))
	{
		return false;
	}


	Cvt_Header header = {};
	header.magic = 0x4D534842;//MSHB
	header.isAnim = isAnim ? 1 : 0;
	header.numMeshes = srcAIScene->mNumMeshes;
	header.numMaterial = srcAIScene->mNumMaterials;
	header.numAnimation = srcAIScene->mNumAnimations;
	header.totalNumBone = g_vecBones.size();
	
	
	// 저장
	OutFile.write((char*)&header, sizeof(Cvt_Header));

	// 본을 여기서 저장
	OutFile.write((char*)g_vecBones.data(), sizeof(Cvt_Bone) * g_vecBones.size());



	if (false == Write_Model(srcAIScene, OutFile, isAnim))
	{
		return false;
	}
	if (isAnim && false == Write_Animation(srcAIScene, OutFile))
	{
		return false;
	}

	if (false == Write_Texture_vector(srcAIScene, OutFile, fbxPath))
	{
		return false;
	}

	cout << "done" << endl;

	return true;
}