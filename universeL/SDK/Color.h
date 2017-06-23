#pragma once

#include "..\ImGUI\imgui.h"

struct Color
{
	int r;
	int g;
	int b;
	int a;

	Color()
	{
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	Color(float arr[4])
	{
		this->r = static_cast<int>(arr[0] * 255);
		this->g = static_cast<int>(arr[1] * 255);
		this->b = static_cast<int>(arr[2] * 255);
		this->a = static_cast<int>(arr[3] * 255);
	}

	Color(int r, int g, int b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Color(int r, int g, int b, int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	DWORD GetARGB() const
	{
		int a0 = a >> 24;
		int r0 = r >> 16;
		int g0 = g >> 8;
		int b0 = b;

		return a0 | r0 | g0 | b0;
	}

	Color operator / (float div)
	{
		Color color = *this;
		color.r = static_cast<int>(color.r / div);
		color.g = static_cast<int>(color.g / div);
		color.b = static_cast<int>(color.b / div);
		return color;
	}

	Color& operator /= (float div)
	{
		Color& color = *this;
		color.r = static_cast<int>(color.r / div);
		color.g = static_cast<int>(color.g / div);
		color.b = static_cast<int>(color.b / div);
		return color;
	}

	Color& operator *= (float coeff)
	{
		Color& color = *this;
		color.r = static_cast<int>(color.r * coeff);
		color.g = static_cast<int>(color.g * coeff);
		color.b = static_cast<int>(color.b * coeff);
		return color;
	}

	static Color FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - static_cast<int>(h);
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return Color(
				static_cast<unsigned char>(brightness * 255),
				static_cast<unsigned char>(t * 255),
				static_cast<unsigned char>(p * 255)
			);
		}
		else if (h < 2)
		{
			return Color(
				static_cast<unsigned char>(q * 255),
				static_cast<unsigned char>(brightness * 255),
				static_cast<unsigned char>(p * 255)
			);
		}
		else if (h < 3)
		{
			return Color(
				static_cast<unsigned char>(p * 255),
				static_cast<unsigned char>(brightness * 255),
				static_cast<unsigned char>(t * 255)
			);
		}
		else if (h < 4)
		{
			return Color(
				static_cast<unsigned char>(p * 255),
				static_cast<unsigned char>(q * 255),
				static_cast<unsigned char>(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return Color(
				static_cast<unsigned char>(t * 255),
				static_cast<unsigned char>(p * 255),
				static_cast<unsigned char>(brightness * 255)
			);
		}
		else
		{
			return Color(
				static_cast<unsigned char>(brightness * 255),
				static_cast<unsigned char>(p * 255),
				static_cast<unsigned char>(q * 255)
			);
		}
	}

	static Color FromImColor(ImColor color)
	{
		return Color(
			static_cast<int>(color.Value.x * 255),
			static_cast<int>(color.Value.y * 255),
			static_cast<int>(color.Value.z * 255),
			static_cast<int>(color.Value.w * 255)
		);
	}

	static ImColor ToImColor(Color color)
	{
		return ImColor(
			color.r / 255.f,
			color.g / 255.f,
			color.b / 255.f,
			color.a / 255.f
		);
	}
};
