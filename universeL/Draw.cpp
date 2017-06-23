#include "Draw.h"

#define _USE_MATH_DEFINES
#include <math.h>

void Draw::Circle(Vector2D position, float points, float radius, Color color)
{
	float step = static_cast<float>(M_PI * 2.0f / points);

	for (float a = 0; a < M_PI * 2.0f; a += step)
	{
		Vector2D start(radius * cosf(a) + position.x, radius * sinf(a) + position.y);
		Vector2D end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y);
		Line(start, end, color);
	}
}

void Draw::FilledCircle(Vector2D position, float points, float radius, Color color)
{
	std::vector<Vertex_t> vertices;
	float step = static_cast<float>(M_PI * 2.0f / points);

	for (float a = 0; a < M_PI * 2.0f; a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + position.x, radius * sinf(a) + position.y)));

	Draw::TexturedPolygon(static_cast<int>(points), vertices.data(), color);
}

void Draw::Circle3D(Vector position, float points, float radius, Color color)
{
	float step = static_cast<float>(M_PI * 2.0f / points);

	std::vector<Vector> points3d;
	for (float a = 0; a < M_PI * 2.0f; a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (Interfaces::DebugOverlay()->ScreenPosition(start, start2d) || Interfaces::DebugOverlay()->ScreenPosition(end, end2d))
			return;

		Draw::Line(Vector2D(start2d.x, start2d.y), Vector2D(end2d.x, end2d.y), color);
	}
}

void Draw::FilledRectangle(int x0, int y0, int x1, int y1, Color col)
{
	Interfaces::MatSurface()->DrawSetColor(col);
	Interfaces::MatSurface()->DrawFilledRect(x0, y0, x1, y1);
}

void Draw::FilledRectangle(Vector2D start_pos, Vector2D end_pos, Color col)
{
	FilledRectangle(static_cast<int>(start_pos.x), static_cast<int>(start_pos.y), static_cast<int>(end_pos.x), static_cast<int>(end_pos.y), col);
}

void Draw::Rectangle(int x0, int y0, int x1, int y1, Color col)
{
	Interfaces::MatSurface()->DrawSetColor(col);
	Interfaces::MatSurface()->DrawOutlinedRect(x0, y0, x1, y1);
}

void Draw::Rectangle(Vector2D start_pos, Vector2D end_pos, Color col)
{
	Rectangle(static_cast<int>(start_pos.x), static_cast<int>(start_pos.y), static_cast<int>(end_pos.x), static_cast<int>(end_pos.y), col);
}

void Draw::Line(int x0, int y0, int x1, int y1, Color color)
{
	Interfaces::MatSurface()->DrawSetColor(color);
	Interfaces::MatSurface()->DrawLine(x0, y0, x1, y1);
}
void Draw::Line(Vector2D startpos, Vector2D endpos, Color color)
{
	Line(static_cast<int>(startpos.x), static_cast<int>(startpos.y), static_cast<int>(endpos.x), static_cast<int>(endpos.y), color);
}

void Draw::PolyLine(int* px, int* py, int num_points, Color col)
{
	Interfaces::MatSurface()->DrawSetColor(col);
	Interfaces::MatSurface()->DrawPolyLine(px, py, num_points);
}

void Draw::PolyLine(Vertex_t* vertice, int num_points, Color col)
{
	static int* points_x = new int[num_points];
	static int* points_y = new int[num_points];
	for (int i = 0; i < num_points; ++i)
	{
		points_x[i] = static_cast<int>(vertice[i].m_Position.x);
		points_y[i] = static_cast<int>(vertice[i].m_Position.y);
	}
	PolyLine(points_x, points_y, num_points, col);
}

void Draw::TexturedPolygon(int n, Vertex_t* vertice, Color col)
{
	static int texture_id = Interfaces::MatSurface()->CreateNewTextureID(true);
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	Interfaces::MatSurface()->DrawSetTextureRGBA(texture_id, buf, 1, 1);
	Interfaces::MatSurface()->DrawSetColor(col);
	Interfaces::MatSurface()->DrawSetTexture(texture_id);
	Interfaces::MatSurface()->DrawTexturedPolygon(n, vertice);
}

void Draw::TextW(int x, int y, const wchar_t* text, HFont font, Color col)
{
	Interfaces::MatSurface()->DrawSetTextPos(x, y);
	Interfaces::MatSurface()->DrawSetTextFont(font);
	Interfaces::MatSurface()->DrawSetTextColor(col);
	Interfaces::MatSurface()->DrawPrintText(text, wcslen(text));
}

void Draw::TextW(Vector2D pos, const wchar_t* text, HFont font, Color col)
{
	TextW(static_cast<int>(pos.x), static_cast<int>(pos.y), text, font, col);
}

void Draw::Text(int x, int y, const char* text, HFont font, Color col)
{
	std::wstring wc = Utils::StringToWstring(text);

	Interfaces::MatSurface()->DrawSetTextPos(x, y);
	Interfaces::MatSurface()->DrawSetTextFont(font);
	Interfaces::MatSurface()->DrawSetTextColor(col);
	Interfaces::MatSurface()->DrawPrintText(wc.c_str(), wcslen(wc.c_str()));
}

void Draw::Text(Vector2D pos, const char* text, HFont font, Color col)
{
	Text(static_cast<int>(pos.x), static_cast<int>(pos.y), text, font, col);
}

void Draw::GetTextWSize(const wchar_t* text, HFont font, int& wide, int& tall)
{
	Interfaces::MatSurface()->GetTextSize(font, text, wide, tall);
}

void Draw::GetTextSize(const char* text, HFont font, int& wide, int& tall)
{
	std::wstring wc = Utils::StringToWstring(text);

	Interfaces::MatSurface()->GetTextSize(font, wc.c_str(), wide, tall);
}

Vector2D Draw::GetTextWSize(const wchar_t* text, HFont font)
{
	int x_res, y_res;
	Interfaces::MatSurface()->GetTextSize(font, text, x_res, y_res);
	return Vector2D(static_cast<float>(x_res), static_cast<float>(y_res));
}

Vector2D Draw::GetTextSize(const char* text, HFont font)
{
	std::wstring wc = Utils::StringToWstring(text);

	int x_res, y_res;
	Interfaces::MatSurface()->GetTextSize(font, wc.c_str(), x_res, y_res);
	return Vector2D(static_cast<float>(x_res), static_cast<float>(y_res));
}

HFont Draw::CreateFont(const char* fontName, int size, int flag)
{
	HFont newFont = Interfaces::MatSurface()->CreateFont();
	Interfaces::MatSurface()->SetFontGlyphSet(newFont, fontName, size, 0, 0, 0, flag);
	return newFont;
}
