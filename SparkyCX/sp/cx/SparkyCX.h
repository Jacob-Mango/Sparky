#pragma once

#include <msclr\marshal_cppstd.h>

namespace sp {
	namespace cx {

		static std::string StringToStdString(System::String^ string)
		{
			return msclr::interop::marshal_as<std::string>(string);
		}

		static System::String^ StdStringToString(const std::string& string)
		{
			return msclr::interop::marshal_as<System::String^>(string);
		}

		static const char* StringToCharArray(System::String^ string)
		{
			char* result = new char[string->Length + 1];
			sprintf(result, "%s", string);
			return result;
		}

		template<typename T>
		public ref class ManagedClass
		{
		protected:
			T* m_Instance;
		public:
			ManagedClass()
			{
				//	m_Instance = new T();
			}

			ManagedClass(T* instance)
				: m_Instance(instance)
			{
			}

			virtual ~ManagedClass()
			{
				if (m_Instance != nullptr)
				{
					delete m_Instance;
					m_Instance = nullptr;
				}
			}

			!ManagedClass()
			{
				if (m_Instance != nullptr)
				{
					delete m_Instance;
					m_Instance = nullptr;
				}
			}

			T* GetHandle()
			{
				return m_Instance;
			}

		};
	}
}