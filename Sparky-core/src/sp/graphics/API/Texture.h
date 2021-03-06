#pragma once

#include "sp/Types.h"
#include "sp/String.h"

namespace sp { namespace graphics { namespace API {

	enum class SP_API TextureWrap
	{
		NONE = 0,
		REPEAT,
		CLAMP,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER
	};

	enum class SP_API TextureFilter
	{
		NONE = 0,
		LINEAR,
		LINEAR_MIPMAP,
		NEAREST
	};

	enum class SP_API TextureFormat
	{
		NONE = 0,
		RGB,
		RGBA,
		RGB16F,
		RGBA32F,
		LUMINANCE,
		LUMINANCE_ALPHA
	};

	enum class SP_API TextureType
	{
		NONE = 0,
		FLOAT,
		UNSIGNED_BYTE,
		BYTE,
		INT,
		DOUBLE
	};

	struct SP_API TextureParameters
	{
		TextureFormat format;
		TextureFilter filter;
		TextureWrap wrap;
		TextureType type;
		bool mipmap;
		int16 samples;

		TextureParameters()
		{
			format = TextureFormat::RGBA;
			filter = TextureFilter::LINEAR;
			wrap = TextureWrap::CLAMP;
			type = TextureType::UNSIGNED_BYTE;
		}

		TextureParameters(TextureFormat format, TextureFilter filter, TextureWrap wrap, TextureType type, bool mipmap = true, int16 samples = 0)
			: format(format), filter(filter), wrap(wrap), type(type), mipmap(mipmap), samples(samples)
		{
		}

		TextureParameters(TextureFormat format, TextureFilter filter, TextureWrap wrap)
			: TextureParameters(format, filter, wrap, TextureType::UNSIGNED_BYTE)
		{
		}

		TextureParameters(TextureFilter filter)
			: TextureParameters(TextureFormat::RGBA, filter, TextureWrap::CLAMP)
		{
		}

		TextureParameters(TextureFilter filter, TextureWrap wrap)
			: format(TextureFormat::RGBA), filter(filter), wrap(wrap)
		{
		}
	};

	struct SP_API TextureLoadOptions
	{
		bool flipX;
		bool flipY;

		TextureLoadOptions()
		{
			flipX = false;
			flipY = false;
		}

		TextureLoadOptions(bool flipX, bool flipY)
			: flipX(flipX), flipY(flipY)
		{
		}
	};

	class SP_API Texture
	{
	protected:
		static TextureWrap s_WrapMode;
		static TextureFilter s_FilterMode;
	public:
		virtual ~Texture() {}

		virtual void Bind(uint slot = 0) const = 0;
		virtual void Unbind(uint slot = 0) const = 0;

		virtual const String& GetName() const = 0;
		virtual const String& GetFilepath() const = 0;
	public:
		inline static void SetWrap(TextureWrap mode) { s_WrapMode = mode; }
		inline static void SetFilter(TextureFilter mode) { s_FilterMode = mode; }
	public:
		static byte GetStrideFromFormat(TextureFormat format);
	};

} } }