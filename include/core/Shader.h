#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "core/GL.h"
#include "core/Math.h"
#include "core/Log.h"
#include "core/Color.h"
#include "core/StringUtils.h"
#include "core/Resource.h"

#define ERROR_BUFSIZE 1024

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
public:
	Shader();
	virtual ~Shader();

	bool loadFromFile(const std::string& fileName, const ShaderType& shaderType);
	bool link();
	//bool isLoadedAndCompiled() const;
	//const std::string& errors() const;
	void bind();
	void unbind();

	//void setUniform(const std::string& name, const Texture* tex);
	//void setUniform(const std::string& name, const TextureCube* tex);
	void setUniform(const std::string& name, int i) const;
	void setUniform(const std::string& name, unsigned int i) const;
	void setUniform(const std::string& name, float f) const;
	void setUniform(const std::string& name, float f1, float f2) const;
	void setUniform(const std::string& name, const vec2& vec2) const;
	void setUniform(const std::string& name, float f1, float f2, float f3) const;
	void setUniform(const std::string& name, float f1, float f2, float f3, float f4) const;
	void setUniform(const std::string& name, const vec3& vec3) const;
	//void setUniform(const std::string& name, const vec4& vec4) const;
	void setUniform(const std::string& name, const mat3& mat3, bool transpose = false) const;
	void setUniform(const std::string& name, const mat4& mat4, bool transpose = false) const;
	void setUniform(const std::string& name, bool b) const;
	void setUniform(const std::string& name, const vec4* vec4, int count) const;
	void setUniform(const std::string& name, const vec4& vec4) const;
	void setUniform(const std::string& name, const Color& color) const;

	GLuint getAttribute(const std::string& name) const;
	GLuint getUniform(const std::string& name) const;

	bool hasAttribute(const std::string& name) const;
	bool hasUniform(const std::string& name) const;

private:
	std::unordered_map<ShaderType, GLuint> mShaderMap;
	std::unordered_map<std::string, GLuint> mAttributeMap;
	std::unordered_map<std::string, GLuint> mUniformMap;
	//GLuint mVertexShaderID;
	//GLuint mFragmentShaderID;
	GLuint mProgram;
	//bool mIsLoadedAndCompiled;
	//std::string mErrors;

	const char* readSource(std::string path);
	//void dumpShaderInfo(const std::string& fileName);
};

//=========================================================================
Shader::Shader()
{
	mProgram = glCreateProgram();
}

//=========================================================================
Shader::~Shader()
{
	unbind();

	for(const auto& s : mShaderMap)
		glDeleteShader(s.second);

	if(mProgram)
		glDeleteProgram(mProgram);

	mShaderMap.clear();
	mAttributeMap.clear();
	mUniformMap.clear();
}

//=========================================================================
bool Shader::loadFromFile(const std::string& fileName, const ShaderType& shaderType)
{
	GLenum type;

	switch(shaderType)
	{
		case ShaderType::Vertex:
			type = GL_VERTEX_SHADER;
			break;

		case ShaderType::Fragment:
			type = GL_FRAGMENT_SHADER;
			break;
	}

	int length;
	auto source = readSource(fileName);
	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);

	delete[] source;

	int isOK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isOK);

	if(!isOK)
	{
		logNote("Failed to compile shader: %s\n", fileName.c_str());

		GLchar tempErrorLog[ERROR_BUFSIZE];
		GLsizei length;

		glGetShaderInfoLog(shader, ERROR_BUFSIZE, &length, tempErrorLog);

		if(length > 0)
		{
			logNote(tempErrorLog);
			glDeleteShader(shader);
		}

		return false;
	}

	mShaderMap[shaderType] = shader;

	return true;
}

//=========================================================================
bool Shader::link()
{
	for(const auto& s : mShaderMap)
		glAttachShader(mProgram, s.second);

	glLinkProgram(mProgram);

	int isOK;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &isOK);

	if(!isOK)
	{
		logNote("Failed to link shader:\n");

		GLchar errorLog[ERROR_BUFSIZE];
		GLsizei length;

		glGetProgramInfoLog(mProgram, ERROR_BUFSIZE, &length, errorLog);

		if(length > 0)
		{
			logNote(errorLog);
			glDeleteProgram(mProgram);
		}

		return false;
	}


	int maxUniforms;
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, &maxUniforms);
	int maxUlen;
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUlen);

	GLchar* ubuf = (char*)malloc(maxUlen + 1);

	for(int i = 0; i < maxUniforms; i++)
	{
		int size;
		GLenum type;
		glGetActiveUniform(mProgram, i, maxUlen, NULL, &size, &type, ubuf);

		mUniformMap[ubuf] = glGetUniformLocation(mProgram, ubuf);
	}

	free(ubuf);

	int maxAttributes;
	glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTES, &maxAttributes);
	int maxAlen;
	glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAlen);

	GLchar* abuf = (char*)malloc(maxAlen + 1);

	for(int i = 0; i < maxAttributes; i++)
	{
		int size;
		GLenum type;
		glGetActiveAttrib(mProgram, i, maxAlen, NULL, &size, &type, abuf);

		mAttributeMap[abuf] = glGetAttribLocation(mProgram, abuf);
	}

	free(abuf);

	//dumpShaderInfo(fileName);

	/*
	// New way
	GLint numActiveAttribs = 0;
	GLint numActiveUniforms = 0;
	glGetProgramInterfaceiv(mProgramID, GL_PROGRAM_INPUT​, GL_ACTIVE_RESOURCES​, &numActiveAttribs);
	glGetProgramInterfaceiv(mProgramID, GL_UNIFORM​, GL_ACTIVE_RESOURCES​, &numActiveAttribs);

	std::vector<GLchar> nameData(256);
	std::vector<GLenum> properties;
	properties.push_back(GL_NAME_LENGTH​);
	properties.push_back(GL_TYPE​);
	properties.push_back(GL_ARRAY_SIZE​);
	std::vector<GLint> values(properties.size());
	for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
	{
	glGetProgramResourceiv(mProgramID, GL_PROGRAM_INPUT, attrib, properties.size(),
	&properties[0], values.size(), NULL, &values[0]);

	nameData.resize(properties[0]); //The length of the name.
	glGetProgramResourceName(mProgramID, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
	std::string name((char*)&nameData[0], nameData.size() - 1);
	}
	// the same with uniforms
	*/

	return true;
}

//=========================================================================
const char* Shader::readSource(std::string path)
{
	std::ifstream source(path);
	std::string line;
	std::string search = "#include";
	std::stringstream  ss;

	unsigned int curLine = 0;

	while(std::getline(source, line))
	{
		curLine++;

		if(StringUtils::startsWith(line, search))
		{
			// Extract file name
			line = StringUtils::extract(line, '"');

			// Extract path name
			path = StringUtils::cutTail(path, "\\/");

			// Include file content
			ss << readSource(path + line);
		}
		else
			ss << line << std::endl;
	}

	return ss.str().c_str();
}

/// <summary>
/// Dumps various shader information to the console.
/// </summary>
//void Shader::dumpShaderInfo(const std::string& fileName)
//{
//	logNote("------------------------------------------------\n");
//	logNote("Shader: %s\n", fileName.c_str());
//	logNote("Program ID: %s\n", std::to_string(mProgram).c_str());
//	logNote("VertexShader ID: %s\n", std::to_string(mVertexShaderID).c_str());
//	logNote("FragmentShader ID: %s\n", std::to_string(mFragmentShaderID).c_str());
//	logNote("\n");
//	logNote("Uniforms:\n");
//
//	for (auto i = mUniformMap.begin(); i != mUniformMap.end(); ++i)
//	{
//		logNote("  %s -> %d\n", i->first.c_str(), i->second);
//	}
//
//	logNote("\n");
//	logNote("Attributes:\n");
//
//	for (auto i = mAttributeMap.begin(); i != mAttributeMap.end(); ++i)
//	{
//		logNote("  %s -> %d\n", i->first.c_str(), i->second);
//	}
//
//	logNote("------------------------------------------------\n");
//}

//=========================================================================
//bool Shader::isLoadedAndCompiled() const
//{
//	return mIsLoadedAndCompiled;
//}

//=========================================================================
//const std::string& Shader::errors() const
//{
//	return mErrors;
//}

//=========================================================================
void Shader::bind()
{
	glUseProgram(mProgram);
}

//=========================================================================
void Shader::unbind()
{
	glUseProgram(0);
}

//=========================================================================
//void Shader::setUniform(const std::string& name, const Texture* tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
//void Shader::setUniform(const std::string& name, const TextureCube *tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
void Shader::setUniform(const std::string& name, int i) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string& name, unsigned int i) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform1f(uniform, f);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform2f(uniform, f1, f2);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const vec2& vec2) const
{
	setUniform(name, vec2.x, vec2.y);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2, float f3) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform3f(uniform, f1, f2, f3);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2, float f3, float f4) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform4f(uniform, f1, f2, f3, f4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const vec3& vec3) const
{
	setUniform(name, vec3.x, vec3.y, vec3.z);
}

//=========================================================================
//void Shader::setUniform(const std::string& name, const vec4& vec4) const
//{
//	setUniform(name, vec4.x, vec4.y, vec4.z, vec4.w);
//}

//=========================================================================
void Shader::setUniform(const std::string& name, const vec4* vec4, int count) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform4fv(uniform, count, (GLfloat*)vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const vec4& vec4) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform4fv(uniform, 1, (GLfloat*)&vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const mat3& mat3, bool transpose) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniformMatrix3fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat3[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const mat4& mat4, bool transpose) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniformMatrix4fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat4[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, bool b) const
{
	auto uniform = getUniform(name);

	if(uniform != -1)
		glUniform1i(uniform, b ? 1 : 0);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const Color& color) const
{
	setUniform(name, color.r, color.g, color.b, color.a);
}

//=========================================================================
GLuint Shader::getAttribute(const std::string& name) const
{
	auto it = mAttributeMap.find(name);

	if(it != mAttributeMap.end())
		return it->second;
	else
	{
		//logError("Attribute '%s' not found\n", name.c_str());
		return -1;
	}
}

//=========================================================================
GLuint Shader::getUniform(const std::string& name) const
{
	auto it = mUniformMap.find(name);

	if(it != mUniformMap.end())
		return it->second;
	else
	{
		//logError("Uniform '%s' not found\n", name.c_str());
		return -1;
	}
}

//=========================================================================
bool Shader::hasAttribute(const std::string& name) const
{
	auto it = mAttributeMap.find(name);

	if(it == mAttributeMap.end())
		return false;
	else
		return true;
}

//=========================================================================
bool Shader::hasUniform(const std::string& name) const
{
	auto it = mUniformMap.find(name);

	if(it == mUniformMap.end())
		return false;
	else
		return true;
}