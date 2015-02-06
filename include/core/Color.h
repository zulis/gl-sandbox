#pragma once

#include "core/Math.h"

class Color
{
public:
	Color();
	Color(float r, float g, float b, float a = 1.0f);
	Color(const vec4& color);
	Color(const vec3& color);

	float r, g, b, a;

	static Color black();
	static Color gray(float value);
	static Color red();
	static Color green();
	static Color blue();
	static Color yellow();
	static Color white();
	static Color pink();
	static Color normal();

	operator Color* ();
	operator const Color* () const;

	vec3 rgb() const;
	vec4 rgba() const;

};

//=========================================================================
Color::Color()
{
	this->r = 0.0f;
	this->g = 0.0f;
	this->b = 0.0f;
	this->a = 1.0f;
}

//=========================================================================
Color::Color(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

//=========================================================================
Color::Color(const vec4& color)
{
	this->r = color.x;
	this->g = color.y;
	this->b = color.z;
	this->a = color.w;
}

//=========================================================================
Color::Color(const vec3& color)
{
	this->r = color.x;
	this->g = color.y;
	this->b = color.z;
	this->a = 1.0f;
}

//=========================================================================
Color Color::black()
{
	return Color(0, 0, 0);
}

//=========================================================================
Color Color::gray(float value)
{
	return Color(value, value, value);
}

//=========================================================================
Color Color::red()
{
	return Color(1, 0, 0);
}

//=========================================================================
Color Color::green()
{
	return Color(0, 1, 0);
}

//=========================================================================
Color Color::blue()
{
	return Color(0, 0, 1);
}

//=========================================================================
Color Color::yellow()
{
	return Color(1, 1, 0);
}

//=========================================================================
Color Color::white()
{
	return Color(1, 1, 1);
}

//=========================================================================
Color Color::pink()
{
	return Color(1, 0, 1);
}

//=========================================================================
Color Color::normal()
{
	return Color(128.0f / 255.0f, 128.0f / 255.0f, 1.0f);
}

//=========================================================================
Color::operator Color* ()
{
	return (Color*) this;
}

//=========================================================================
Color::operator const Color* () const
{
	return (const Color*) this;
}

//=========================================================================
vec3 Color::rgb() const
{
	return vec3(r, g, b);
}

//=========================================================================
vec4 Color::rgba() const
{
	return vec4(r, g, b, a);
}
