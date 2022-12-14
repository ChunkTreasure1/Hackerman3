#include "vtpch.h"
#include "DefaultTextureImporter.h"

#include "Volt/Core/Graphics/GraphicsContext.h"
#include "Volt/Utility/DirectXUtils.h"

#include "Volt/Rendering/Texture/Image2D.h"
#include "Volt/Rendering/Texture/Texture2D.h"

#include <stb/stb_image.h>

namespace Volt
{
	Ref<Texture2D> DefaultTextureImporter::ImportTextureImpl(const std::filesystem::path& path)
	{
		int32_t width;
		int32_t height;
		int32_t channels;

		void* data = nullptr;
		bool isHDR = false;

		if (stbi_is_hdr(path.string().c_str()))
		{
			stbi_set_flip_vertically_on_load(0);
			data = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			isHDR = true;
		}
		else
		{
			stbi_set_flip_vertically_on_load(1);
			data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		}

		ImageFormat format = ImageFormat::RGBA;

		if (isHDR)
		{
			format = ImageFormat::RGBA32F;
		}

		Ref<Texture2D> texture = CreateRef<Texture2D>(format, width, height, data);
		return texture;
	}
}