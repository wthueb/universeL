#pragma once

#include "..\ImGUI\imgui.h"

struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

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

	Color(unsigned char r, unsigned char g, unsigned char b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color operator / (float div)
	{
		Color color = *this;

		color.r = static_cast<unsigned char>(color.r / div);
		color.g = static_cast<unsigned char>(color.g / div);
		color.b = static_cast<unsigned char>(color.b / div);

		return color;
	}

	Color& operator /= (float div)
	{
		Color& color = *this;

		color.r = static_cast<unsigned char>(color.r / div);
		color.g = static_cast<unsigned char>(color.g / div);
		color.b = static_cast<unsigned char>(color.b / div);

		return color;

	}

	Color& operator *= (float coeff)
	{
		Color& color = *this;

		color.r = static_cast<unsigned char>(color.r * coeff);
		color.g = static_cast<unsigned char>(color.g * coeff);
		color.b = static_cast<unsigned char>(color.b * coeff);

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
			static_cast<unsigned char>(color.Value.x * 255),
			static_cast<unsigned char>(color.Value.y * 255),
			static_cast<unsigned char>(color.Value.z * 255),
			static_cast<unsigned char>(color.Value.w * 255)
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
