#pragma once

namespace sp {

	struct Timestep
	{
	private:
		float m_Timestep;
		float m_LastTime;
		float m_Elapsed;
	public:
		inline Timestep(float initialTime)
			: m_Timestep(0.0f), m_LastTime(initialTime)
		{
		}

		inline void Update(float currentTime)
		{
			m_Timestep = currentTime - m_LastTime;
			m_LastTime = currentTime;
			m_Elapsed += m_Timestep;
		}

		inline float GetMillis() const { return m_Timestep; }
		inline float GetElapsedMillis() const { return m_Elapsed; }

		inline float GetSeconds() const { return m_Timestep * 0.001f; }
		inline float GetElapsedSeconds() const { return m_Elapsed * 0.001f; }
	};

}