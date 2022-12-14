#include "vtpch.h"
#include "Framebuffer.h"

#include "Volt/Core/Graphics/GraphicsContext.h"
#include "Volt/Utility/ImageUtility.h"

namespace Volt
{
	Framebuffer::Framebuffer(const FramebufferSpecification& aSpecification)
		: mySpecification(aSpecification), myWidth(aSpecification.width), myHeight(aSpecification.height)
	{
		if (mySpecification.existingDepth)
		{
			myOwnsDepth = false;
			myDepthAttachmentImage = mySpecification.existingDepth;
		}

		uint32_t attachmentIndex = 0;
		for (auto& attachment : mySpecification.attachments)
		{
			if (mySpecification.existingImages.find(attachmentIndex) != mySpecification.existingImages.end())
			{
				if (!Utility::IsDepthFormat(mySpecification.existingImages[attachmentIndex]->GetFormat()))
				{
					myColorAttachmentImages.emplace_back();
				}
			}
			else if (myOwnsDepth && (Utility::IsDepthFormat(attachment.format) || Utility::IsTypeless(attachment.format)))
			{
				ImageSpecification spec{};
				spec.format = attachment.format;
				spec.usage = attachment.storageCompatible ? ImageUsage::AttachmentStorage : ImageUsage::Attachment;
				spec.width = myWidth;
				spec.height = myHeight;
				spec.readable = attachment.readable;
				spec.writeable = attachment.writeable;
				spec.debugName = attachment.debugName;

				spec.typelessTargetFormat = attachment.format == ImageFormat::R16Typeless ? ImageFormat::R16F : ImageFormat::R32F;
				spec.typelessViewFormat = attachment.format == ImageFormat::R16Typeless ? ImageFormat::DEPTH16U : ImageFormat::DEPTH32F;

				myDepthAttachmentImage = Image2D::Create(spec);
			}
			else if (!Utility::IsDepthFormat(attachment.format))
			{
				ImageSpecification spec{};
				spec.format = attachment.format;
				spec.usage = attachment.storageCompatible ? ImageUsage::AttachmentStorage : ImageUsage::Attachment;
				spec.width = myWidth;
				spec.height = myHeight;
				spec.readable = attachment.readable;
				spec.writeable = attachment.writeable;
				spec.debugName = attachment.debugName;

				myColorAttachmentImages.emplace_back(Image2D::Create(spec));
			}

			attachmentIndex++;
		}

		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		Release();
	}

	void Framebuffer::Invalidate()
	{
		Release();

		auto device = GraphicsContext::GetDevice();
		D3D11_BLEND_DESC blendDesc{};
		blendDesc.IndependentBlendEnable = true;

		const bool createImages = myColorAttachmentImages.empty();

		uint32_t attachmentIndex = 0;
		for (const auto& attachment : mySpecification.attachments)
		{
			if ((Utility::IsDepthFormat(attachment.format) || Utility::IsTypeless(attachment.format)) && myOwnsDepth)
			{
				Ref<Image2D> image = myDepthAttachmentImage;
				auto& imageSpec = const_cast<ImageSpecification&>(myDepthAttachmentImage->GetSpecification());
				imageSpec.width = myWidth;
				imageSpec.height = myHeight;

				myDepthAttachmentImage->Invalidate();
			}
			else if (!Utility::IsDepthFormat(attachment.format))
			{
				Ref<Image2D> colorAttachment;
				bool existingImage = false;

				if (mySpecification.existingImages.find(attachmentIndex) != mySpecification.existingImages.end())
				{
					colorAttachment = mySpecification.existingImages[attachmentIndex];
					myColorAttachmentImages[attachmentIndex] = colorAttachment;
					existingImage = true;
				}
				else
				{
					if (createImages)
					{
						ImageSpecification spec{};
						spec.format = attachment.format;
						spec.usage = attachment.storageCompatible ? ImageUsage::AttachmentStorage : ImageUsage::Attachment;
						spec.width = myWidth;
						spec.height = myHeight;
						spec.readable = attachment.readable;
						spec.writeable = attachment.writeable;
						spec.debugName = attachment.debugName;

						colorAttachment = myColorAttachmentImages.emplace_back(Image2D::Create(spec));
					}
					else
					{
						Ref<Image2D> image = myColorAttachmentImages[attachmentIndex];
						ImageSpecification& spec = const_cast<ImageSpecification&>(image->GetSpecification());

						spec.width = myWidth;
						spec.height = myHeight;

						colorAttachment = image;
						colorAttachment->Invalidate(nullptr);
					}
				}

				blendDesc.RenderTarget[attachmentIndex] = Utility::GetBlendDescFromBlendType(attachment.blending);

				if (!Utility::IsFloatFormat(attachment.format))
				{
					blendDesc.RenderTarget[attachmentIndex].BlendEnable = false;
				}

				attachmentIndex++;
			}
		}

		device->CreateBlendState(&blendDesc, myBlendState.GetAddressOf());

		myViewport.Width = (float)myWidth;
		myViewport.Height = (float)myHeight;
		myViewport.TopLeftX = 0;
		myViewport.TopLeftY = 0;
		myViewport.MinDepth = 0;
		myViewport.MaxDepth = 1;
	}

	void Framebuffer::Bind()
	{
		auto context = GraphicsContext::GetContext();

		std::vector<ID3D11RenderTargetView*> rtvs;
		for (const auto& image : myColorAttachmentImages)
		{
			rtvs.emplace_back(image->GetRTV().Get());
		}

		context->OMSetBlendState(myBlendState.Get(), nullptr, 0xffffffff);
		context->OMSetRenderTargets((uint32_t)myColorAttachmentImages.size(), rtvs.data(), myDepthAttachmentImage ? myDepthAttachmentImage->GetDSV().Get() : nullptr);
		context->RSSetViewports(1, &myViewport);
	}

	void Framebuffer::Unbind()
	{
		auto context = GraphicsContext::GetContext();
		std::vector<ID3D11RenderTargetView*> views(myColorAttachmentImages.size(), nullptr);

		context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		context->OMSetRenderTargets((uint32_t)myColorAttachmentImages.size(), views.data(), nullptr);
	}

	void Framebuffer::Clear()
	{
		auto context = GraphicsContext::GetContext();

		uint32_t attachmentIndex = 0;
		for (const auto& image : myColorAttachmentImages)
		{
			if (mySpecification.existingImages.find(attachmentIndex) == mySpecification.existingImages.end() && !Utility::IsTypeless(mySpecification.attachments[attachmentIndex].format))
			{
				context->ClearRenderTargetView(image->GetRTV().Get(), (float*)&mySpecification.attachments[attachmentIndex].clearColor);
			}
			attachmentIndex++;
		}

		if (myDepthAttachmentImage && myOwnsDepth)
		{
			context->ClearDepthStencilView(myDepthAttachmentImage->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		}
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		myWidth = width;
		myHeight = height;

		Invalidate();
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& aSpecification)
	{
		return CreateRef<Framebuffer>(aSpecification);
	}

	void Framebuffer::Release()
	{
		uint32_t attachmentIndex = 0;
		for (const auto& image : myColorAttachmentImages)
		{
			if (mySpecification.existingImages.find(attachmentIndex) != mySpecification.existingImages.end())
			{
				continue;
			}

			image->Release();
			attachmentIndex++;
		}

		if (myDepthAttachmentImage && myOwnsDepth)
		{
			myDepthAttachmentImage->Release();
		}

		myBlendState = nullptr;
	}
}