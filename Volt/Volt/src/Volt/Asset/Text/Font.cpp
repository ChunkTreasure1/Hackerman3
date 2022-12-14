#include "vtpch.h"
#include "Font.h"

#include "Volt/Asset/Text/MSDFData.h"
#include "Volt/Log/Log.h"
#include "Volt/Core/Base.h"
#include "Volt/Core/Buffer.h"

#include "Volt/Rendering/Texture/Texture2D.h"

#include "Volt/Utility/FileSystem.h"

namespace Volt
{
	namespace Utility
	{
		const std::filesystem::path GetAndCreateCacheFolder()
		{
			const std::filesystem::path cachePath = FileSystem::GetAssetsPath() / "Cache" / "Fonts";
			if (!std::filesystem::exists(cachePath))
			{
				std::filesystem::create_directories(cachePath);
			}

			return cachePath;
		}

		const std::filesystem::path GetCachePath(const std::string& fontName)
		{
			return GetAndCreateCacheFolder() / (fontName + ".cached");
		}
	}

	constexpr float DEFAULT_ANGLE_THRESHOLD = 3.f;
	constexpr float DEFAULT_MITER_LIMIT = 1.f;
	constexpr uint64_t LCG_MULTIPLIER = 6364136223846793005ull;
	constexpr uint64_t LCG_INCREMENT = 1442695040888963407ull;
	constexpr uint32_t THREADS = 8;

	struct FontInput
	{
		std::string filename;
		std::string charsetFilename;
		std::string fontName;
		msdf_atlas::GlyphIdentifierType glyphType;
		float scale;
	};

	struct Configuration
	{
		msdf_atlas::ImageType imageType;
		msdf_atlas::ImageFormat imageFormat;
		msdf_atlas::YDirection yDirection;
		msdf_atlas::GeneratorAttributes generatorAttribs;
		int32_t width = 0;
		int32_t height = 0;
		float emSize = 0.f;
		float pxRange = 0.f;
		float angleThreshold = 0.f;
		float miterLimit = 0.f;

		bool expensiveColoring;
		uint64_t coloringSeed;
		void (*edgeColoring)(msdfgen::Shape&, double, unsigned long long);
	};

	template<typename T, typename S, int32_t N, msdf_atlas::GeneratorFunction<S, N> GEN_FN>
	static Ref<Texture2D> CreateAndCacheAtlas(const std::string& aFontName, float aFontSize, const std::vector<msdf_atlas::GlyphGeometry>& aGlyphs, const msdf_atlas::FontGeometry& aFontGeom, const Configuration& aConfig)
	{
		msdf_atlas::ImmediateAtlasGenerator<S, N, GEN_FN, msdf_atlas::BitmapAtlasStorage<T, N>> generator(aConfig.width, aConfig.height);
		generator.setAttributes(aConfig.generatorAttribs);
		generator.setThreadCount(THREADS);
		generator.generate(aGlyphs.data(), (int32_t)aGlyphs.size());

		auto bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();
		Ref<Texture2D> texture = Texture2D::Create(ImageFormat::RGBA32F, bitmap.width, bitmap.height, bitmap.pixels);

		// Cache
		{
			const auto cachePath = Utility::GetCachePath(aFontName);
			const size_t size = sizeof(T) * N * bitmap.width * bitmap.height;
			Font::FontHeader header;
			header.width = (uint32_t)bitmap.width;
			header.height = (uint32_t)bitmap.height;

			Buffer buffer{ size + sizeof(Font::FontHeader) };
			buffer.Copy(&header, sizeof(Font::FontHeader));
			buffer.Copy(bitmap.pixels, size, sizeof(Font::FontHeader));
			Buffer::WriteToFile(buffer, cachePath);

			buffer.Release();
		}

		return texture;
	}

	Font::Font(const std::filesystem::path& aPath)
	{
		myMSDFData = new MSDFData();
		this->path = aPath;

		int32_t result = 0;
		FontInput fontInput = {};
		Configuration config = {};

		fontInput.glyphType = msdf_atlas::GlyphIdentifierType::UNICODE_CODEPOINT;
		fontInput.scale = -1.f;
		fontInput.filename = aPath.string();

		config.imageType = msdf_atlas::ImageType::MTSDF;
		config.imageFormat = msdf_atlas::ImageFormat::BINARY_FLOAT;
		config.yDirection = msdf_atlas::YDirection::BOTTOM_UP;
		config.edgeColoring = msdfgen::edgeColoringInkTrap;
		config.generatorAttribs.config.overlapSupport = true;
		config.generatorAttribs.scanlinePass = true;
		config.angleThreshold = DEFAULT_ANGLE_THRESHOLD;
		config.miterLimit = DEFAULT_MITER_LIMIT;
		config.emSize = 40.f;

		///// Load fonts /////
		int32_t fixedWidth = -1;
		int32_t fixedHeight = -1;
		float minEmSize = 0.f;
		float rangeValue = 2.f;

		msdf_atlas::TightAtlasPacker::DimensionsConstraint atlasSizeConstraint = msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE;

		bool anyCodepointsAvailable = false;

		class FontHolder
		{
		public:
			FontHolder()
				: myFreeTypeHandle(msdfgen::initializeFreetype())
			{}

			~FontHolder()
			{
				if (myFreeTypeHandle)
				{
					if (myFontHandle)
					{
						msdfgen::destroyFont(myFontHandle);
					}

					msdfgen::deinitializeFreetype(myFreeTypeHandle);
				}
			}

			bool Load(const std::string& fontFilename)
			{
				if (myFreeTypeHandle && !fontFilename.empty())
				{
					if (myFontFilename == fontFilename)
					{
						return true;
					}

					if (myFontHandle)
					{
						msdfgen::destroyFont(myFontHandle);
					}

					if ((myFontHandle = msdfgen::loadFont(myFreeTypeHandle, fontFilename.c_str())))
					{
						myFontFilename = fontFilename;
						return true;
					}

					myFontFilename = "";
				}
				return false;
			}

			operator msdfgen::FontHandle* () const
			{
				return myFontHandle;
			}

		private:
			msdfgen::FreetypeHandle* myFreeTypeHandle = nullptr;
			msdfgen::FontHandle* myFontHandle = nullptr;
			std::string myFontFilename;
		} font;

		bool success = font.Load(fontInput.filename);
		VT_CORE_ASSERT(success, "Font did not load correctly!");

		if (fontInput.scale <= 0.f)
		{
			fontInput.scale = 1.f;
		}

		// Load charset
		msdf_atlas::Charset charset;

		// From ImGui
		static const uint32_t charsetRanges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			0x2DE0, 0x2DFF, // Cyrillic Extended-A
			0xA640, 0xA69F, // Cyrillic Extended-B
			0,
		};

		for (int32_t range = 0; range < 8; range += 2)
		{
			for (uint32_t c = charsetRanges[range]; c <= charsetRanges[range + 1]; c++)
			{
				charset.add(c);
			}
		}

		// Load glyphs
		myMSDFData->fontGeometry = msdf_atlas::FontGeometry(&myMSDFData->glyphs);
		int32_t glyphsLoaded = -1;

		switch (fontInput.glyphType)
		{
			case msdf_atlas::GlyphIdentifierType::GLYPH_INDEX:
			{
				glyphsLoaded = myMSDFData->fontGeometry.loadGlyphset(font, (double)fontInput.scale, charset);
				break;
			}

			case msdf_atlas::GlyphIdentifierType::UNICODE_CODEPOINT:
			{
				glyphsLoaded = myMSDFData->fontGeometry.loadCharset(font, (double)fontInput.scale, charset);
				anyCodepointsAvailable |= glyphsLoaded > 0;
				break;
			}
		}

		VT_CORE_ASSERT(glyphsLoaded >= 0, "Unable to load glyphs!");

		if (!fontInput.fontName.empty())
		{
			myMSDFData->fontGeometry.setName(fontInput.fontName.c_str());
		}

		// Determine atlas dimensions, scale and range
		const float pxRange = rangeValue;
		const bool fixedDimensions = fixedWidth >= 0 && fixedHeight >= 0;
		const bool fixedScale = config.emSize > 0.f;

		msdf_atlas::TightAtlasPacker atlasPacker;
		if (fixedDimensions)
		{
			atlasPacker.setDimensions(fixedWidth, fixedHeight);
		}
		else
		{
			atlasPacker.setDimensionsConstraint(atlasSizeConstraint);
		}

		if (fixedScale)
		{
			atlasPacker.setScale((double)config.emSize);
		}
		else
		{
			atlasPacker.setMinimumScale(minEmSize);
		}

		atlasPacker.setPadding(config.imageType == msdf_atlas::ImageType::MSDF || config.imageType == msdf_atlas::ImageType::MTSDF ? 0 : -1);
		atlasPacker.setPixelRange(pxRange);
		atlasPacker.setMiterLimit((double)config.miterLimit);

		if (int32_t remaining = atlasPacker.pack(myMSDFData->glyphs.data(), (int32_t)myMSDFData->glyphs.size()))
		{
			if (remaining < 0)
			{
				VT_CORE_ASSERT(false, "Invalid number");
			}
			else
			{
				VT_CORE_ERROR("Could not fit {0} out of {1} glyphs in atlas!", remaining, (int32_t)myMSDFData->glyphs.size());
				VT_CORE_ASSERT(false, "Invalid number");
			}
		}

		atlasPacker.getDimensions(config.width, config.height);
		VT_CORE_ASSERT(config.width > 0 && config.height > 0, "Invalid dimensions");

		config.emSize = (float)atlasPacker.getScale();
		config.pxRange = (float)atlasPacker.getPixelRange();

		// Edge coloring
		if (config.imageType == msdf_atlas::ImageType::MSDF || config.imageType == msdf_atlas::ImageType::MTSDF)
		{
			if (config.expensiveColoring)
			{
				msdf_atlas::Workload([&glyphs = myMSDFData->glyphs, &config](int i, int threadNo) -> bool
					{
						uint64_t glyphSeed = (LCG_MULTIPLIER * (config.coloringSeed ^ i) + LCG_INCREMENT) * !!config.coloringSeed;
						glyphs[i].edgeColoring(config.edgeColoring, config.angleThreshold, glyphSeed);
						return true;
					}, (int)myMSDFData->glyphs.size()).finish(THREADS);
			}
			else
			{
				uint64_t glyphSeed = config.coloringSeed;
				for (msdf_atlas::GlyphGeometry& glyph : myMSDFData->glyphs)
				{
					glyphSeed += LCG_MULTIPLIER;
					glyph.edgeColoring(config.edgeColoring, (double)config.angleThreshold, glyphSeed);
				}
			}
		}

		std::string fontName = path.filename().string();
		const std::filesystem::path cachePath = Utility::GetCachePath(fontName);

		Ref<Texture2D> texture;

		if (FileSystem::Exists(cachePath))
		{
			Buffer data = Buffer::ReadFromFile(cachePath);
			if (data.IsValid())
			{
				Font::FontHeader header = *data.As<Font::FontHeader>();
				texture = Texture2D::Create(ImageFormat::RGBA32F, header.width, header.height, data.As<void*>(sizeof(Font::FontHeader)));

				data.Release();
			}
		}
		else
		{
			bool floatingPointFormat = true;
			switch (config.imageType)
			{
				case msdf_atlas::ImageType::MSDF:
				{
					if (floatingPointFormat)
					{
						texture = CreateAndCacheAtlas<float, float, 3, msdf_atlas::msdfGenerator>(fontName, (float)config.emSize, myMSDFData->glyphs, myMSDFData->fontGeometry, config);
					}
					else
					{
						texture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(fontName, (float)config.emSize, myMSDFData->glyphs, myMSDFData->fontGeometry, config);
					}

					break;
				}

				case msdf_atlas::ImageType::MTSDF:
				{
					if (floatingPointFormat)
					{
						texture = CreateAndCacheAtlas<float, float, 4, msdf_atlas::mtsdfGenerator>(fontName, (float)config.emSize, myMSDFData->glyphs, myMSDFData->fontGeometry, config);
					}
					else
					{
						texture = CreateAndCacheAtlas<uint8_t, float, 4, msdf_atlas::mtsdfGenerator>(fontName, (float)config.emSize, myMSDFData->glyphs, myMSDFData->fontGeometry, config);
					}

					break;
				}
			}

		}
		myAtlas = texture;
	}

	Font::~Font()
	{
		delete myMSDFData;
	}
}