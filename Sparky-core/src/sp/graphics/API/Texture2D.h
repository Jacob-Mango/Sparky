#pragma once

#include "Texture.h"

namespace sp { namespace graphics { namespace API {

	class SP_API Texture2D : public Texture
	{
	protected:
		String m_Name;
		String m_FileName;
		uint m_Handle;
		uint m_Width, m_Height;
		uint m_BitsPerPixel;
		byte* m_Pixels = nullptr;
		TextureParameters m_Parameters;
		TextureLoadOptions m_LoadOptions;
		TextureType m_Type;
	public:
		virtual void SetData(const void* pixels) = 0;
		virtual void SetData(uint color) = 0;

		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;
	public:
		static Texture2D* Create(uint width, uint height, TextureParameters parameters = TextureParameters(), TextureLoadOptions loadOptions = TextureLoadOptions(), TextureType type = TextureType::UNSIGNED_BYTE);
		static Texture2D* CreateFromFile(const String& filepath, TextureParameters parameters = TextureParameters(), TextureLoadOptions loadOptions = TextureLoadOptions(), TextureType type = TextureType::UNSIGNED_BYTE);
		static Texture2D* CreateFromFile(const String& filepath, TextureLoadOptions loadOptions);
		static Texture2D* CreateFromFile(const String& name, const String& filepath, TextureParameters parameters = TextureParameters(), TextureLoadOptions loadOptions = TextureLoadOptions(), TextureType type = TextureType::UNSIGNED_BYTE);
		static Texture2D* CreateFromFile(const String& name, const String& filepath, TextureLoadOptions loadOptions);
	};

} } }