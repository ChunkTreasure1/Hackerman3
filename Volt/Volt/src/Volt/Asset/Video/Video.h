#pragma once

#include "Volt/Asset/Asset.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libpostproc/postprocess.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
#include <wtypes.h>
}

namespace Volt
{
	enum class VideoStatus
	{
		Stopped,
		Playing,
		End
	};


	class Texture2D;
	class Video : public Asset
	{
	public:
		Video() = default;
		Video(const std::filesystem::path& path);
		~Video() override;

		void Play(bool shouldLoop = false);
		void Pause();
		void Stop();

		void Update(float deltaTime);
		inline const VideoStatus GetStatus() const { return myStatus; }
		inline const Ref<Texture2D> GetTexture() const { return myTexture; }

		static AssetType GetStaticType() { return AssetType::Video; }
		AssetType GetType() override { return GetStaticType(); };

	private:
		struct ReaderState
		{
			AVFormatContext* formatContext = nullptr;
			AVCodecContext* decoderContext = nullptr;

			AVFrame* avFrame = nullptr;
			AVFrame* avFrameBGR = nullptr;

			AVPacket avPacket;
			SwsContext* swsContext = nullptr;

			const AVCodec* codec = nullptr;

			uint32_t width = 0;
			uint32_t height = 0;
			int32_t videoStreamIndex = -1;
			AVRational timeBase{};
		};

		int32_t ReadNextFrame();
	
		void Release();
		void Restart();
		bool GetFrameData(uint32_t*& buffer);

		Ref<Texture2D> myTexture;
		VideoStatus myStatus = VideoStatus::Stopped;

		int32_t myNumberOfBytes = 0;
		int32_t myGotFrame = 0;
		uint8_t* myBuffer = nullptr;
	
		float myCurrentFrameTime = 0.f;
		bool myIsPlaying = false;
		bool myShouldLoop = false;

		ReaderState myReaderState;
	};
}