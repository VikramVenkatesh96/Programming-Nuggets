#pragma once
#include <cmath>

namespace Math {

	#define PI 3.1416f
	#define DEGREETORADIAN(D) (D * PI / 180.0f) 

	struct Vector2
	{
		Vector2(float x = 0.0f, float y = 0.0f) : m_x(x), m_y(y)
		{}
		
		Vector2 operator+(const Vector2& other) { return Vector2(m_x + other.m_x, m_y + other.m_y); }
		Vector2 operator-(const Vector2& other) { return Vector2(m_x - other.m_x, m_y - other.m_y); }
		Vector2 operator*(const float& value) { return Vector2(m_x * value, m_y * value); }
		float Dot(const Vector2& other) { return m_x * other.m_x + m_y * other.m_y; }
		float Length() { return sqrtf(m_x * m_x + m_y * m_y); }
		Vector2 Rotate(float angle) {
			return Vector2(m_x * cosf(DEGREETORADIAN(angle)) - m_y * sinf(DEGREETORADIAN(angle)),
				m_x * sinf(DEGREETORADIAN(angle)) + m_y * cosf(DEGREETORADIAN(angle)));
		}
	public:
		float m_x, m_y;
	};
}
