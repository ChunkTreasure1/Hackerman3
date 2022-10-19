#include "vtpch.h"
#include "FbxImporter.h"

#include "Volt/Log/Log.h"

#include "Volt/Asset/Mesh/Mesh.h"
#include "Volt/Asset/Mesh/Material.h"
#include "Volt/Asset/Mesh/SubMaterial.h"

#include "Volt/Asset/Animation/Skeleton.h"
#include "Volt/Asset/Animation/Animation.h"

#include "Volt/Rendering/Shader/ShaderRegistry.h"

#include <GEM/gem.h>

namespace Volt
{
	namespace Utility
	{
		uint32_t GetFramesPerSecond(FbxTime::EMode aMode)
		{
			switch (aMode)
			{
				case FbxTime::eDefaultMode: return 0;
				case FbxTime::eFrames120: return 120;
				case FbxTime::eFrames100: return 100;
				case FbxTime::eFrames60: return 60;
				case FbxTime::eFrames50: return 50;
				case FbxTime::eFrames48: return 48;
				case FbxTime::eFrames30: return 30;
				case FbxTime::eFrames30Drop: return 30;
				case FbxTime::eNTSCDropFrame: return 30;
				case FbxTime::eNTSCFullFrame: return 30;
				case FbxTime::ePAL: return 24;
				case FbxTime::eFrames24: return 24;
				case FbxTime::eFrames1000: return 1000;
				case FbxTime::eFilmFullFrame: return 0;
				case FbxTime::eCustom: return 0;
				case FbxTime::eFrames96: return 96;
				case FbxTime::eFrames72: return 72;
				case FbxTime::eFrames59dot94: return 60;
				case FbxTime::eFrames119dot88: return 120;
			}

			return 0;
		}

		gem::mat4 FbxMToMatrix(const FbxAMatrix& aMatrix)
		{
			gem::mat4 result;
			for (uint32_t i = 0; i < 4; i++)
			{
				FbxVector4 column = aMatrix.GetColumn(i);
				result[i] = gem::vec4((float)column[0], (float)column[1], (float)column[2], (float)column[3]);
			}

			return result;
		}
	}

	Ref<Mesh> FbxImporter::ImportMeshImpl(const std::filesystem::path& path)
	{
		FbxManager* sdkManager = FbxManager::Create();
		FbxIOSettings* ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);

		ioSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
		ioSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
		ioSettings->SetBoolProp(IMP_FBX_LINK, false);
		ioSettings->SetBoolProp(IMP_FBX_SHAPE, false);
		ioSettings->SetBoolProp(IMP_FBX_GOBO, false);
		ioSettings->SetBoolProp(IMP_FBX_ANIMATION, false);
		ioSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

		sdkManager->SetIOSettings(ioSettings);

		fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(sdkManager, "");

		bool importStatus = importer->Initialize(path.string().c_str(), -1, sdkManager->GetIOSettings());
		if (!importStatus)
		{
			VT_CORE_ERROR("Unable to import file {0}!", path.string().c_str());
			return nullptr;
		}

		FbxScene* fbxScene = FbxScene::Create(sdkManager, "Scene");

		fbxsdk::FbxAxisSystem axisSystem(fbxsdk::FbxAxisSystem::eDirectX);

		importer->Import(fbxScene);
		axisSystem.DeepConvertScene(fbxScene);

		FbxNode* rootNode = fbxScene->GetRootNode();

		std::vector<FbxNode*> geomNodes;
		FetchGeometryNodes(rootNode, geomNodes);

		Ref<Mesh> mesh = CreateRef<Mesh>();
		Ref<Skeleton> skeleton = CreateRef<Skeleton>();
		mesh->myMaterial = CreateRef<Material>();
		mesh->myMaterial->myName = path.stem().string() + "_mat";

		ProcessSkeletonHierarchy(rootNode, skeleton);

		std::unordered_multimap<uint32_t, std::pair<uint32_t, float>> controlPointWeights;

		for (auto node : geomNodes)
		{
			FbxGeometryConverter geomConverter(sdkManager);
			if (!geomConverter.Triangulate(node->GetNodeAttribute(), true, true))
			{
				geomConverter.Triangulate(node->GetNodeAttribute(), true, false);
			}

			ProcessJoints(node, controlPointWeights, skeleton);
			ProcessMesh(node->GetMesh(), skeleton, fbxScene, controlPointWeights, mesh);
		}

		mesh->Construct();

		importer->Destroy();

		return mesh;
	}

	Ref<Skeleton> FbxImporter::ImportSkeletonImpl(const std::filesystem::path& path)
	{
		FbxManager* sdkManager = FbxManager::Create();
		FbxIOSettings* ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);

		ioSettings->SetBoolProp(IMP_FBX_MATERIAL, false);
		ioSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
		ioSettings->SetBoolProp(IMP_FBX_LINK, false);
		ioSettings->SetBoolProp(IMP_FBX_SHAPE, false);
		ioSettings->SetBoolProp(IMP_FBX_GOBO, false);
		ioSettings->SetBoolProp(IMP_FBX_ANIMATION, false);
		ioSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

		sdkManager->SetIOSettings(ioSettings);

		fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(sdkManager, "");

		bool importStatus = importer->Initialize(path.string().c_str(), -1, sdkManager->GetIOSettings());
		if (!importStatus)
		{
			VT_CORE_ERROR("Unable to import file {0}!", path.string().c_str());
			return nullptr;
		}

		FbxScene* fbxScene = FbxScene::Create(sdkManager, "Scene");

		fbxsdk::FbxAxisSystem axisSystem(fbxsdk::FbxAxisSystem::eDirectX);

		importer->Import(fbxScene);
		axisSystem.DeepConvertScene(fbxScene);

		FbxNode* rootNode = fbxScene->GetRootNode();
		Ref<Skeleton> skeleton = CreateRef<Skeleton>();

		ProcessSkeletonHierarchy(rootNode, skeleton);

		if (skeleton->myJoints.empty())
		{
			return nullptr;
		}

		std::unordered_multimap<uint32_t, std::pair<uint32_t, float>> controlPointWeights;

		std::vector<FbxNode*> geomNodes;
		FetchGeometryNodes(rootNode, geomNodes);

		for (const auto& node : geomNodes)
		{
			ProcessJoints(node, controlPointWeights, skeleton);
		}

		skeleton->myName = rootNode->GetName();

		importer->Destroy();
		return skeleton;
	}

	Ref<Animation> FbxImporter::ImportAnimationImpl(const std::filesystem::path& path)
	{
		FbxManager* sdkManager = FbxManager::Create();
		FbxIOSettings* ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);

		ioSettings->SetBoolProp(IMP_FBX_MATERIAL, false);
		ioSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
		ioSettings->SetBoolProp(IMP_FBX_LINK, false);
		ioSettings->SetBoolProp(IMP_FBX_SHAPE, false);
		ioSettings->SetBoolProp(IMP_FBX_GOBO, false);
		ioSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
		ioSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

		sdkManager->SetIOSettings(ioSettings);

		fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(sdkManager, "");

		bool importStatus = importer->Initialize(path.string().c_str(), -1, sdkManager->GetIOSettings());
		if (!importStatus)
		{
			VT_CORE_ERROR("Unable to import file {0}!", path.string().c_str());
			return nullptr;
		}

		FbxScene* fbxScene = FbxScene::Create(sdkManager, "Scene");

		fbxsdk::FbxAxisSystem axisSystem(fbxsdk::FbxAxisSystem::eDirectX);

		importer->Import(fbxScene);
		axisSystem.DeepConvertScene(fbxScene);

		FbxNode* rootNode = fbxScene->GetRootNode();
		Ref<Skeleton> skeleton = CreateRef<Skeleton>();
		Ref<Animation> animation = CreateRef<Animation>();

		ProcessSkeletonHierarchy(rootNode, skeleton);

		const auto timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		animation->myFramesPerSecond = Utility::GetFramesPerSecond(timeMode);

		if (skeleton->myJoints.empty())
		{
			return nullptr;
		}

		FbxNode* skeletonRoot = fbxScene->FindNodeByName(skeleton->myJoints[0].name.c_str());

		const FbxVector4 translation = skeletonRoot->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 rotation = skeletonRoot->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 scale = skeletonRoot->GetGeometricScaling(FbxNode::eSourcePivot);
		const FbxAMatrix rootTransform = FbxAMatrix(translation, rotation, scale);

		FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
		if (!animStack)
		{
			return nullptr;
		}

		FbxString stackName = animStack->GetName();
		FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(stackName);
		if (takeInfo)
		{
			const FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			const FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

			const auto startFrame = start.GetFrameCount(timeMode);
			const auto endFrame = end.GetFrameCount(timeMode);

			const FbxLongLong animationLength = endFrame - startFrame + 1;
			animation->myDuration = (float)animationLength / (float)animation->myFramesPerSecond;

			animation->myFrames.resize(animationLength);

			uint32_t localFrameCounter = 0;

			for (FbxLongLong t = startFrame; t <= endFrame; t++)
			{
				FbxTime time;
				time.SetFrame(t, timeMode);

				animation->myFrames[localFrameCounter].localTransforms.resize(skeleton->myJoints.size());

				for (uint32_t j = 0; j < (uint32_t)skeleton->myJoints.size(); j++)
				{
					FbxNode* jointNode = fbxScene->FindNodeByName(skeleton->myJoints[j].name.c_str());
					const FbxAMatrix localTransform = jointNode->EvaluateLocalTransform(time);

					const gem::mat4 localMat = Utility::FbxMToMatrix(localTransform);

					animation->myFrames[localFrameCounter].localTransforms[j] = localMat;
				}

				localFrameCounter++;
			}
		}
		else
		{
			VT_CORE_ERROR("No animation found in file {0}!", path.string());
		}

		if (animation->myFrames.empty())
		{
			VT_CORE_ERROR("Unable to load animation in file {0}!", path.string());
		}

		importer->Destroy();
		return animation;
	}

	void FbxImporter::ProcessMesh(FbxMesh* fbxMesh, Ref<Skeleton> skeleton, FbxScene* aScene, std::unordered_multimap<uint32_t, std::pair<uint32_t, float>>& aControlPointWeights, Ref<Mesh> mesh)
	{
		if (!fbxMesh)
		{
			return;
		}

		if (fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0)
		{
			bool result = fbxMesh->GenerateTangentsData(0, true, false);
		}

		Ref<SubMaterial> material;
		int32_t matIndex = 0;

		for (int32_t meshMatIndex = 0; meshMatIndex < fbxMesh->GetNode()->GetMaterialCount() || meshMatIndex == 0; meshMatIndex++)
		{
			for (int32_t sceneMatIndex = 0; sceneMatIndex < aScene->GetMaterialCount(); sceneMatIndex++)
			{
				FbxSurfaceMaterial* sceneMaterial = aScene->GetMaterial(sceneMatIndex);
				FbxSurfaceMaterial* meshMaterial = fbxMesh->GetNode()->GetMaterial(meshMatIndex);

				if (sceneMaterial == meshMaterial)
				{
					if (mesh->myMaterial->mySubMaterials.find(sceneMatIndex) == mesh->myMaterial->mySubMaterials.end())
					{
						material = SubMaterial::Create(sceneMaterial->GetName(), sceneMatIndex, ShaderRegistry::Get("Deferred"));
						mesh->myMaterial->mySubMaterials[sceneMatIndex] = material;
					}
					matIndex = sceneMatIndex;
				}
			}
		}

		const FbxVector4* ctrlPoints = fbxMesh->GetControlPoints();
		const uint32_t triangleCount = fbxMesh->GetPolygonCount();
		uint32_t vertexCount = 0;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve(triangleCount * 3);
		indices.reserve(triangleCount);

		for (uint32_t i = 0; i < triangleCount; i++)
		{
			const int32_t polySize = fbxMesh->GetPolygonSize(i);
			VT_CORE_ASSERT(polySize == 3, "Mesh must be fully triangulated!");

			for (int32_t j = 0; j < polySize; j++)
			{
				Vertex vertex;
				const int32_t ctrlPointIndex = fbxMesh->GetPolygonVertex(i, j);

				vertex.position.x = (float)ctrlPoints[ctrlPointIndex][0];
				vertex.position.y = (float)ctrlPoints[ctrlPointIndex][1];
				vertex.position.z = (float)ctrlPoints[ctrlPointIndex][2];

				const int32_t numUVs = fbxMesh->GetElementUVCount();
				const int32_t texUvIndex = fbxMesh->GetTextureUVIndex(i, j);

				for (int32_t uv = 0; uv < numUVs && uv < 4; uv++)
				{
					FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV(uv);
					const auto coord = uvElement->GetDirectArray().GetAt(texUvIndex);

					vertex.texCoords[uv].x = (float)coord.mData[0];
					vertex.texCoords[uv].y = 1.f - (float)coord.mData[1];
				}

				ReadNormal(fbxMesh, ctrlPointIndex, vertexCount, vertex.normal);
				ReadTangent(fbxMesh, ctrlPointIndex, vertexCount, vertex.tangent);
				ReadBitangent(fbxMesh, ctrlPointIndex, vertexCount, vertex.bitangent);

				if (!skeleton->myJoints.empty())
				{
					typedef std::unordered_multimap<uint32_t, std::pair<uint32_t, float>>::iterator mmIterator;

					std::pair<mmIterator, mmIterator> values = aControlPointWeights.equal_range(ctrlPointIndex);
					int32_t idx = 0;
					for (mmIterator it = values.first; it != values.second && idx < 4; ++it)
					{
						std::pair<uint32_t, float> boneAndWeight = it->second;

						switch (idx)
						{
							case 0:
								vertex.influences.x = boneAndWeight.first;
								vertex.weights.x = boneAndWeight.second;
								break;

							case 1:
								vertex.influences.y = boneAndWeight.first;
								vertex.weights.y = boneAndWeight.second;
								break;

							case 2:
								vertex.influences.z = boneAndWeight.first;
								vertex.weights.z = boneAndWeight.second;
								break;

							case 3:
								vertex.influences.w = boneAndWeight.first;
								vertex.weights.w = boneAndWeight.second;
								break;
						}
						idx++;
					}
				}

				int32_t windCorrection;
				switch (vertexCount)
				{
				case 1:
					windCorrection = 2;
					break;

				case 2:
					windCorrection = 1;
					break;

				default:
					windCorrection = vertexCount;
					break;
				}

				const int32_t windedPolygonIndex = i * 3 + windCorrection;

				fbxsdk::FbxColor fbxColors[4];
				const int32_t numColors = fbxMesh->GetElementVertexColorCount();
				for (int32_t col = 0; col < numColors && col < 4; col++)
				{
					FbxGeometryElementVertexColor* colElement = fbxMesh->GetElementVertexColor(col);
					switch (colElement->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
					{
						switch (colElement->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							fbxColors[col] = colElement->GetDirectArray().GetAt(ctrlPointIndex);
						}
						break;
						case FbxGeometryElement::eIndexToDirect:
						{
							const int Idx = colElement->GetIndexArray().GetAt(ctrlPointIndex);
							fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
						}
						break;
						default:
							throw std::exception("Invalid Reference Mode!");
						}
					}
					break;

					case FbxGeometryElement::eByPolygonVertex:
					{
						switch (colElement->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							fbxColors[col] = colElement->GetDirectArray().GetAt(windedPolygonIndex);
						}
						break;
						case FbxGeometryElement::eIndexToDirect:
						{
							const int Idx = colElement->GetIndexArray().GetAt(windedPolygonIndex);
							fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
						}
						break;
						default:
							throw std::exception("Invalid Reference Mode!");
						}
					}
					break;
					}
				}

				for (uint32_t i = 0; i < 4; i++)
				{
					vertex.color[i].x = (float)fbxColors[i].mRed;
					vertex.color[i].y = (float)fbxColors[i].mGreen;
					vertex.color[i].z = (float)fbxColors[i].mBlue;
					vertex.color[i].w = (float)fbxColors[i].mAlpha;
				}

				size_t size = vertices.size();
				size_t i = 0;

				for (i = 0; i < size; i++)
				{
					if (vertex == vertices[i])
					{
						break;
					}
				}

				if (i == size)
				{
					vertices.emplace_back(vertex);
				}

				indices.emplace_back((uint32_t)i);
				vertexCount++;
			}
		}

		// Copy new vertices into vector
		{
			size_t preVertexCount = mesh->myVertices.size();
			size_t preIndexCount = mesh->myIndices.size();

			mesh->myVertices.resize(preVertexCount + vertices.size());
			mesh->myIndices.resize(mesh->myIndices.size() + indices.size());

			memcpy_s(&mesh->myVertices[preVertexCount], sizeof(Vertex) * vertices.size(), vertices.data(), sizeof(Vertex) * vertices.size());
			memcpy_s(&mesh->myIndices[preIndexCount], sizeof(uint32_t) * indices.size(), indices.data(), sizeof(uint32_t) * indices.size());

			const auto fbxTranslation = fbxMesh->GetNode(0)->LclTranslation.Get();
			const auto fbxRotation = fbxMesh->GetNode(0)->LclRotation.Get();
			const auto fbxScale = fbxMesh->GetNode(0)->LclScaling.Get();

			const gem::vec3 position = { (float)fbxTranslation[0], (float)fbxTranslation[1], (float)fbxTranslation[2] };
			const gem::vec3 rotation = { (float)fbxRotation[0], (float)fbxRotation[1], (float)fbxRotation[2] };
			const gem::vec3 scale = { (float)fbxScale[0], (float)fbxScale[1], (float)fbxScale[2] };

			const gem::mat4 localTransform = gem::translate(gem::mat4(1.f), position) *
				gem::mat4_cast(gem::quat(rotation)) * gem::scale(gem::mat4(1.f), scale);

			auto& submesh = mesh->mySubMeshes.emplace_back();
			submesh.vertexCount = (uint32_t)vertices.size();
			submesh.indexCount = (uint32_t)indices.size();
			submesh.indexStartOffset = (uint32_t)preIndexCount;
			submesh.vertexStartOffset = (uint32_t)preVertexCount;
			submesh.materialIndex = matIndex;
			submesh.transform = localTransform;
			submesh.GenerateHash();
		}
	}

	void FbxImporter::FetchGeometryNodes(FbxNode* node, std::vector<FbxNode*>& outNodes)
	{
		if (node->GetNodeAttribute())
		{
			switch (node->GetNodeAttribute()->GetAttributeType())
			{
				case FbxNodeAttribute::eMesh:
				{
					if (node->GetMesh())
					{
						outNodes.emplace_back(node);
					}
				}
			}
		}

		for (uint32_t i = 0; i < (uint32_t)node->GetChildCount(); i++)
		{
			FetchGeometryNodes(node->GetChild(i), outNodes);
		}
	}

	void FbxImporter::ReadNormal(FbxMesh* mesh, int32_t ctrlPointIndex, int32_t vertCount, gem::vec3& normal)
	{
		if (mesh->GetElementNormalCount() < 1)
		{
			return;
		}

		FbxGeometryElementNormal* fbxNormal = mesh->GetElementNormal(0);

		switch (fbxNormal->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (fbxNormal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						normal.x = (float)fbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[0];
						normal.y = (float)fbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[1];
						normal.z = (float)fbxNormal->GetDirectArray().GetAt(ctrlPointIndex)[2];

						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxNormal->GetIndexArray().GetAt(ctrlPointIndex);
						normal.x = (float)fbxNormal->GetDirectArray().GetAt(id)[0];
						normal.y = (float)fbxNormal->GetDirectArray().GetAt(id)[1];
						normal.z = (float)fbxNormal->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}

			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (fbxNormal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						normal.x = (float)fbxNormal->GetDirectArray().GetAt(vertCount)[0];
						normal.y = (float)fbxNormal->GetDirectArray().GetAt(vertCount)[1];
						normal.z = (float)fbxNormal->GetDirectArray().GetAt(vertCount)[2];
						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxNormal->GetIndexArray().GetAt(vertCount);
						normal.x = (float)fbxNormal->GetDirectArray().GetAt(id)[0];
						normal.y = (float)fbxNormal->GetDirectArray().GetAt(id)[1];
						normal.z = (float)fbxNormal->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}

			break;
		}
	}

	void FbxImporter::ReadTangent(FbxMesh* mesh, int32_t ctrlPointIndex, int32_t vertCount, gem::vec3& tangent)
	{
		if (mesh->GetElementTangentCount() < 1)
		{
			return;
		}

		FbxGeometryElementTangent* fbxTangent = mesh->GetElementTangent(0);

		switch (fbxTangent->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (fbxTangent->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						tangent.x = (float)fbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[0];
						tangent.y = (float)fbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[1];
						tangent.z = (float)fbxTangent->GetDirectArray().GetAt(ctrlPointIndex)[2];

						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxTangent->GetIndexArray().GetAt(ctrlPointIndex);
						tangent.x = (float)fbxTangent->GetDirectArray().GetAt(id)[0];
						tangent.y = (float)fbxTangent->GetDirectArray().GetAt(id)[1];
						tangent.z = (float)fbxTangent->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}

			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (fbxTangent->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						tangent.x = (float)fbxTangent->GetDirectArray().GetAt(vertCount)[0];
						tangent.y = (float)fbxTangent->GetDirectArray().GetAt(vertCount)[1];
						tangent.z = (float)fbxTangent->GetDirectArray().GetAt(vertCount)[2];
						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxTangent->GetIndexArray().GetAt(vertCount);
						tangent.x = (float)fbxTangent->GetDirectArray().GetAt(id)[0];
						tangent.y = (float)fbxTangent->GetDirectArray().GetAt(id)[1];
						tangent.z = (float)fbxTangent->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}

			break;
		}
	}

	void FbxImporter::ReadBitangent(FbxMesh* mesh, int32_t ctrlPointIndex, int32_t vertCount, gem::vec3& bitangent)
	{
		if (mesh->GetElementBinormalCount() < 1)
		{
			return;
		}

		FbxGeometryElementBinormal* fbxBitangent = mesh->GetElementBinormal(0);

		switch (fbxBitangent->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (fbxBitangent->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						bitangent.x = (float)fbxBitangent->GetDirectArray().GetAt(ctrlPointIndex)[0];
						bitangent.y = (float)fbxBitangent->GetDirectArray().GetAt(ctrlPointIndex)[1];
						bitangent.z = (float)fbxBitangent->GetDirectArray().GetAt(ctrlPointIndex)[2];

						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxBitangent->GetIndexArray().GetAt(ctrlPointIndex);
						bitangent.x = (float)fbxBitangent->GetDirectArray().GetAt(id)[0];
						bitangent.y = (float)fbxBitangent->GetDirectArray().GetAt(id)[1];
						bitangent.z = (float)fbxBitangent->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}

			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (fbxBitangent->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						bitangent.x = (float)fbxBitangent->GetDirectArray().GetAt(vertCount)[0];
						bitangent.y = (float)fbxBitangent->GetDirectArray().GetAt(vertCount)[1];
						bitangent.z = (float)fbxBitangent->GetDirectArray().GetAt(vertCount)[2];
						break;
					}

					case FbxGeometryElement::eIndexToDirect:
					{
						int32_t id = fbxBitangent->GetIndexArray().GetAt(vertCount);
						bitangent.x = (float)fbxBitangent->GetDirectArray().GetAt(id)[0];
						bitangent.y = (float)fbxBitangent->GetDirectArray().GetAt(id)[1];
						bitangent.z = (float)fbxBitangent->GetDirectArray().GetAt(id)[2];

						break;
					}

					default:
						break;
				}

				break;
			}
			break;
		}
	}

	void FbxImporter::ProcessSkeletonHierarchy(FbxNode* aNode, Ref<Skeleton> aSkeleton)
	{
		for (int32_t i = 0; i < aNode->GetChildCount(); i++)
		{
			FbxNode* currNode = aNode->GetChild(i);
			ProcessSkeletonHierarchyRecursively(currNode, 0, -1, aSkeleton);
		}

		aSkeleton->myInverseBindPoses.resize(aSkeleton->myJoints.size());
	}

	void FbxImporter::ProcessSkeletonHierarchyRecursively(FbxNode* aNode, int32_t aIndex, int32_t aParent, Ref<Skeleton> aSkeleton)
	{
		if (aNode->GetNodeAttribute() && aNode->GetNodeAttribute()->GetAttributeType() && aNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			Skeleton::Joint currJoint;
			currJoint.parentIndex = aParent;
			currJoint.name = aNode->GetName();
			aSkeleton->myJoints.emplace_back(currJoint);
		}
		for (int i = 0; i < aNode->GetChildCount(); i++)
		{
			ProcessSkeletonHierarchyRecursively(aNode->GetChild(i), (int32_t)aSkeleton->myJoints.size(), aIndex, aSkeleton);
		}
	}

	uint32_t FbxImporter::FindJointIndexByName(const std::string& aName, Ref<Skeleton> aSkeleton)
	{
		for (uint32_t i = 0; i < aSkeleton->myJoints.size(); i++)
		{
			if (aSkeleton->myJoints[i].name == aName)
			{
				return i;
			}
		}

		return -1;
	}

	void FbxImporter::ProcessJoints(FbxNode* aNode, std::unordered_multimap<uint32_t, std::pair<uint32_t, float>>& aControlPointWeights, Ref<Skeleton> aSkeleton)
	{
		FbxMesh* currMesh = aNode->GetMesh();
		uint32_t numDeformers = currMesh->GetDeformerCount();

		const FbxVector4 lT = aNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = aNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = aNode->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geomTransform = FbxAMatrix(lT, lR, lS);

		for (uint32_t deformerIndex = 0; deformerIndex < numDeformers; deformerIndex++)
		{
			FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
			if (!currSkin)
			{
				continue;
			}

			uint32_t numClusters = currSkin->GetClusterCount();
			for (uint32_t clusterIndex = 0; clusterIndex < numClusters; clusterIndex++)
			{
				FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
				std::string jointName = currCluster->GetLink()->GetName();
				uint32_t currentJoint = FindJointIndexByName(jointName, aSkeleton);

				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix globalBindposeInverseMatrix;

				currCluster->GetTransformMatrix(transformMatrix);
				currCluster->GetTransformLinkMatrix(transformLinkMatrix);
				globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geomTransform;
				globalBindposeInverseMatrix = globalBindposeInverseMatrix.Transpose();

				aSkeleton->myInverseBindPoses[currentJoint] = Utility::FbxMToMatrix(globalBindposeInverseMatrix);

				uint32_t numIndices = currCluster->GetControlPointIndicesCount();
				for (uint32_t i = 0; i < numIndices; i++)
				{
					uint32_t controlPoint = currCluster->GetControlPointIndices()[i];
					float weight = static_cast<float>(currCluster->GetControlPointWeights()[i]);

					aControlPointWeights.insert({ controlPoint, { currentJoint, weight } });
				}
			}
		}
	}
}