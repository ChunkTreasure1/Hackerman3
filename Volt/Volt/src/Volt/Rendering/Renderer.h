#pragma once

#include "Volt/Core/Base.h"
#include "Volt/Asset/Mesh/SubMesh.h"

#include "Volt/Rendering/Texture/SubTexture2D.h"
#include "Volt/Rendering/RendererStructs.h"
#include "Volt/Rendering/RenderPass.h"
#include "Volt/Scene/Scene.h"

#include <GEM/gem.h>

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace Volt
{
	class SamplerState;
	class ConstantBuffer;
	class StructuredBuffer;
	class Mesh;

	class Shader;
	class Material;
	class SubMaterial;
	class ComputePipeline;
	
	class Texture2D;
	class Framebuffer;
	class Camera;
	class VertexBuffer;
	class IndexBuffer;
	class Font;

	struct SpriteVertex;
	struct BillboardVertex;
	struct LineVertex;
	struct TextVertex;

	struct RenderCommand
	{
		SubMesh subMesh;
		gem::mat4 transform;

		std::vector<gem::mat4> boneTransforms;

		Ref<Mesh> mesh;
		Ref<SubMaterial> material;

		float timeSinceCreation = 0.f;
		uint32_t id = 0;
		uint32_t objectBufferId = 0;
	};

	enum class DepthState : uint32_t
	{
		ReadWrite = 0,
		Read = 1,
		None = 2
	};

	class Renderer
	{
	public:
		struct DefaultData
		{
			Ref<Shader> defaultShader;
			Ref<Texture2D> whiteTexture;
			Ref<Texture2D> emptyNormal;

			Ref<Image2D> brdfLut;
			Ref<Image2D> blackCubeImage;
		};

		struct SectionStatistics
		{
			uint32_t drawCalls = 0;
			uint32_t materialBinds = 0;
			uint32_t vertexIndexBufferBinds = 0;
		};

		struct Statistics
		{
			void Reset()
			{
				drawCalls = 0;
				materialBinds = 0;
				vertexIndexBufferBinds = 0;
				spriteCount = 0;
				billboardCount = 0;
				lineCount = 0;

				perSectionStatistics.clear();
			}

			uint32_t drawCalls = 0;
			uint32_t materialBinds = 0;
			uint32_t vertexIndexBufferBinds = 0;
			uint32_t spriteCount = 0;
			uint32_t billboardCount = 0;
			uint32_t lineCount = 0;

			std::unordered_map<std::string, SectionStatistics> perSectionStatistics;
		};

		struct ProfilingData
		{
			struct SectionPipelineData
			{
				uint64_t vertexCount;
				uint64_t primitiveCount;
				uint64_t psInvocations;
				uint64_t vsInvocations;
			};

			uint32_t currentFrame = 0;
			uint32_t lastFrame = 0;

			uint32_t currentQueryIndex = 0;

			std::array<ComPtr<ID3D11Query>, 2> disjointQuery;
			std::array<ComPtr<ID3D11Query>, 2> beginFrameQuery;
			std::array<ComPtr<ID3D11Query>, 2> endFrameQuery;

			std::array<std::unordered_map<std::string, ComPtr<ID3D11Query>>, 2> sectionTimeQueries;
			std::array<std::unordered_map<std::string, ComPtr<ID3D11Query>>, 2> sectionPipelineQueries;

			std::array<std::vector<std::string>, 2> sectionNames;
			std::array<std::vector<float>, 2> sectionTimes;

			std::unordered_map<std::string, float> sectionAverageTimes;
			std::unordered_map<std::string, float> sectionTotalAverageTimes;

			std::array<std::unordered_map<std::string, SectionPipelineData>, 2> sectionPipelineDatas;
		
			std::array<float, 2> frameGPUTime;
			float frameAverageGPUTime;
			float frameTotalAverageGPUTime;
		};

		struct Settings
		{
			float exposure = 1.f;
			float ambianceMultiplier = 0.5f;
			float bloomWeight = 0.04f;
		};

		static void Initialize();
		static void InitializeBuffers();
		static void Shutdown();

		static void Begin();
		static void End();

		static void BeginPass(const RenderPass& aRenderPass, Ref<Camera> camera, bool shouldClear = true);
		static void EndPass();

		static void BeginSection(const std::string& name);
		static void EndSection(const std::string& name);

		static void ResetStatistics();

		static void Submit(Ref<Mesh> aMesh, const gem::mat4& aTransform, uint32_t aId = 0, float aTimeSinceCreation = 0.f);
		static void Submit(Ref<Mesh> aMesh, uint32_t subMeshIndex, const gem::mat4& aTransform, uint32_t aId = 0, float aTimeSinceCreation = 0.f);
		static void Submit(Ref<Mesh> aMesh, Ref<Material> aMaterial, const gem::mat4& aTransform, uint32_t aId = 0, float aTimeSinceCreation = 0.f);
		static void Submit(Ref<Mesh> aMesh, Ref<SubMaterial> aMaterial, const gem::mat4& aTransform, uint32_t aId = 0, float aTimeSinceCreation = 0.f);
		static void Submit(Ref<Mesh> aMesh, const gem::mat4& aTransform, const std::vector<gem::mat4>& aBoneTransforms, uint32_t aId = 0, float aTimeSinceCreation = 0.f);

		static void SubmitSprite(Ref<Texture2D> aTexture, const gem::mat4& aTransform, uint32_t id = 0, const gem::vec4& aColor = { 1.f, 1.f, 1.f, 1.f });
		static void SubmitSprite(const gem::mat4& aTransform, const gem::vec4& aColor, uint32_t id = 0);

		static void SubmitSprite(const SubTexture2D& aTexture, const gem::mat4& aTransform, uint32_t aId = 0, const gem::vec4& aColor = { 1.f, 1.f, 1.f, 1.f });

		static void SubmitBillboard(Ref<Texture2D> aTexture, const gem::vec3& aPosition, const gem::vec3& aScale, uint32_t aId = 0, const gem::vec4& aColor = { 1.f, 1.f, 1.f, 1.f });
		static void SubmitBillboard(const gem::vec3& aPosition, const gem::vec3& aScale, const gem::vec4& aColor, uint32_t aId = 0);

		static void SubmitLine(const gem::vec3& aStart, const gem::vec3& aEnd, const gem::vec4& aColor = { 1.f, 1.f, 1.f, 1.f });

		static void SubmitLight(const PointLight& pointLight);
		static void SubmitLight(const DirectionalLight& dirLight);

		static void SubmitString(const std::string& aString, const Ref<Font> aFont, const gem::mat4& aTransform, float aMaxWidth, const gem::vec4& aColor = { 1.f });

		static void SetAmbianceMultiplier(float multiplier); // #TODO: we should probably not do it like this

		static void DrawFullscreenTriangleWithShader(Ref<Shader> aShader);
		static void DrawFullscreenQuadWithShader(Ref<Shader> aShader);
		static void DrawMesh(Ref<Mesh> aMesh, const gem::mat4& aTransform);
		static void DrawMesh(Ref<Mesh> aMesh, uint32_t subMeshIndex, const gem::mat4& aTransform);

		static void DispatchRenderCommands();
		static void DispatchRenderCommandsInstanced();

		static void DispatchLines();
		static void DispatchText();
		static void DispatchSpritesWithShader(Ref<Shader> aShader);
		static void DispatchBillboardsWithShader(Ref<Shader> aShader);

		static void ExecuteFullscreenPass(Ref<ComputePipeline> aComputePipeline, Ref<Framebuffer> aFramebuffer);

		static void SetSceneData(const SceneData& aSceneData);
		static void SetDepthState(DepthState aDepthState);
		static SceneEnvironment GenerateEnvironmentMap(AssetHandle aTextureHandle);

		inline static const DefaultData& GetDefaultData() { return *myDefaultData; }
		inline static const Statistics& GetStatistics() { return myRendererData->statistics; }
		inline static Settings& GetSettings() { return myRendererData->settings; }
		inline static ProfilingData& GetProfilingData() { return myRendererData->profilingData; }

	private:
		Renderer() = delete;

		static void CreateDepthStates();
		static void CreateDefaultBuffers();
		static void CreateDefaultData();
		static void CreateSpriteData();
		static void CreateBillboardData();
		static void CreateLineData();
		static void CreateTextData();
		static void CreateProfilingData();
		static void CreateInstancingData();

		static void GenerateBRDFLut();

		static void UpdatePerPassBuffers();
		static void UpdatePerFrameBuffers();

		static void UploadObjectData();

		static void SortRenderCommands();
		static void CollectRenderCommands();

		struct Samplers
		{
			Ref<SamplerState> linearWrap;
			Ref<SamplerState> linearPointWrap;

			Ref<SamplerState> pointWrap;
			Ref<SamplerState> pointLinearWrap;

			Ref<SamplerState> linearClamp;
			Ref<SamplerState> linearPointClamp;

			Ref<SamplerState> pointClamp;
			Ref<SamplerState> pointLinearClamp;

			Ref<SamplerState> anisotropic;
			Ref<SamplerState> shadow;
		};

		struct InstancedRenderCommand
		{
			SubMesh subMesh;

			std::vector<gem::mat4> boneTransforms;
			std::vector<InstanceData> objectDataIds;

			Ref<Mesh> mesh;
			Ref<SubMaterial> material;

			uint32_t count = 0;
		};

		struct SpriteData
		{
			inline static constexpr uint32_t MAX_QUADS = 200000;
			inline static constexpr uint32_t MAX_VERTICES = MAX_QUADS * 4;
			inline static constexpr uint32_t MAX_INDICES = MAX_QUADS * 6;

			Ref<VertexBuffer> vertexBuffer;
			Ref<IndexBuffer> indexBuffer;
			Ref<Texture2D> textureSlots[32];

			gem::vec4 vertices[4];
			gem::vec2 texCoords[4];
			uint32_t indexCount = 0;
			uint32_t textureSlotIndex = 1;

			SpriteVertex* vertexBufferBase = nullptr;
			SpriteVertex* vertexBufferPtr = nullptr;
		};

		struct LineData
		{
			inline static constexpr uint32_t MAX_LINES = 200000;
			inline static constexpr uint32_t MAX_VERTICES = MAX_LINES * 2;
			inline static constexpr uint32_t MAX_INDICES = MAX_LINES * 2;

			Ref<VertexBuffer> vertexBuffer;
			Ref<IndexBuffer> indexBuffer;
			Ref<Shader> shader;

			LineVertex* vertexBufferBase = nullptr;
			LineVertex* vertexBufferPtr = nullptr;

			uint32_t indexCount = 0;
		};

		struct BillboardData
		{
			inline static constexpr uint32_t MAX_BILLBOARDS = 1000000;

			Ref<VertexBuffer> vertexBuffer;
			Ref<Texture2D> textureSlots[32];

			uint32_t textureSlotIndex = 1;
			uint32_t vertexCount = 0;

			BillboardVertex* vertexBufferBase = nullptr;
			BillboardVertex* vertexBufferPtr = nullptr;
		};

		struct TextData
		{
			inline static constexpr uint32_t MAX_QUADS = 200000;
			inline static constexpr uint32_t MAX_VERTICES = MAX_QUADS * 4;
			inline static constexpr uint32_t MAX_INDICES = MAX_QUADS * 6;

			Ref<VertexBuffer> vertexBuffer;
			Ref<IndexBuffer> indexBuffer;
			Ref<Texture2D> textureSlots[32];
			Ref<Shader> shader;

			gem::vec4 vertices[4];
			uint32_t indexCount = 0;
			uint32_t textureSlotIndex = 0;

			TextVertex* vertexBufferBase = nullptr;
			TextVertex* vertexBufferPtr = nullptr;
		};

		struct InstancingData
		{
			Ref<VertexBuffer> instancedVertexBuffer;
			std::vector<InstancedRenderCommand> passRenderCommands;
		};

		struct RendererData
		{
			inline static constexpr uint32_t MAX_OBJECTS_PER_FRAME = 20000;

			std::vector<RenderCommand> renderCommands;

			std::vector<PointLight> pointLights;
			DirectionalLight directionalLight{};
			Statistics statistics{};
			Settings settings{};
			ProfilingData profilingData{};
			InstancingData instancingData{};

			Samplers samplers{};
			RenderPass currentPass{};
			Ref<Camera> currentPassCamera;
			SceneData sceneData;

			SpriteData spriteData{};
			BillboardData billboardData{};
			LineData lineData{};
			TextData textData{};

			// Fullscreen quad
			Ref<VertexBuffer> quadVertexBuffer;
			Ref<IndexBuffer> quadIndexBuffer;

			std::unordered_map<AssetHandle, SceneEnvironment> environmentCache;
		};

		inline static std::vector<ComPtr<ID3D11DepthStencilState>> myDepthStates;
		inline static Scope<DefaultData> myDefaultData;
		inline static Scope<RendererData> myRendererData;
	};
}