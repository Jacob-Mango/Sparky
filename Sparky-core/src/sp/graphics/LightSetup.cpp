#include "sp/sp.h"
#include "LightSetup.h"

namespace sp {
	namespace graphics {

		LightSetup::LightSetup()
		{

		}

		LightSetup::~LightSetup()
		{
			for (uint i = 0; i < m_Lights.size(); i++)
				delete m_Lights[i];
		}

		Light* LightSetup::Add(Light* light)
		{
			m_Lights.push_back(light);
			return light;
		}

		void LightSetup::Remove(Light* light)
		{
			for (uint i = 0; i < m_Lights.size(); i++)
			{
				if (m_Lights[i] == light)
				{
					m_Lights.erase(m_Lights.begin() + i);
					break;
				}
			}
		}

		byte* LightSetup::GetLightData(uint start, uint max) {
			union Lights {
				Light* lights;
				byte* data;

				Lights(uint max) {
					lights = new Light[max];
					data = new byte[max * sizeof(Light)];
				}
			} lights(max);

			for (int i = start; i < start + max; i++) {
				if (m_Lights.size() <= i) {
					lights.lights[i] = Light();
				}
				else {
					lights.lights[i] = *m_Lights[i];
				}
			}
			return lights.data;
		}

	}
}