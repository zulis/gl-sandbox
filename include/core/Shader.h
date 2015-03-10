#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include "core/GL.h"
#include "core/Math.h"
#include "core/Log.h"
#include "core/Color.h"
#include "core/StringUtils.h"
#include "core/Resource.h"

#define ERROR_BUFSIZE 1024

typedef std::unique_ptr<class Shader> ShaderPtr;

enum class ShaderType
{
	Vertex,
	Fragment
};

class Shader
{
public:
    static ShaderPtr create();
	Shader();
	virtual ~Shader();

	bool loadFromFile(const std::string& fileName, const ShaderType& shaderType);
	bool link();
	void bind();
	void unbind();
	void reload();

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
	std::vector<std::tuple<ShaderType, std::string, GLuint>> mShaderVec;
	std::unordered_map<std::string, GLuint> mAttributeMap;
	std::unordered_map<std::string, GLuint> mUniformMap;
	GLuint mProgram;

	std::string readSource(std::string fileName);
	void showInfo(/*const std::string& fileName*/);
};

//=========================================================================
ShaderPtr Shader::create()
{
    return std::make_unique<Shader>();
}

//=========================================================================
Shader::Shader()
{
	mProgram = glCreateProgram();
}

//=========================================================================
Shader::~Shader()
{
	unbind();

	for(const auto& s : mShaderVec)
		glDeleteShader(std::get<2>(s));

	if(mProgram)
		glDeleteProgram(mProgram);

	mShaderVec.clear();
	mAttributeMap.clear();
	mUniformMap.clear();

	note("Shader released.");
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

	auto source = readSource(fileName);

	if(source != std::string())
	{
		//auto sourceChar = source.c_str();
		auto shader = glCreateShader(type);
		auto sourceChar = (const GLchar*)source.c_str();
		glShaderSource(shader, 1, &sourceChar, NULL);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if(isCompiled == GL_FALSE)
		{
			error("Failed to compile shader: %s", fileName.c_str());

			GLchar errorLog[ERROR_BUFSIZE];
			GLsizei length;

			glGetShaderInfoLog(shader, ERROR_BUFSIZE, &length, errorLog);

            if (length > 0)
			    error(errorLog);

			glDeleteShader(shader);

			return false;
		}

		mShaderVec.push_back(std::make_tuple(shaderType, fileName, shader));

		note("Shader loaded: %s", fileName.c_str());
		return true;
	}
	else
		return false;
}

//=========================================================================
bool Shader::link()
{
	for(const auto& s : mShaderVec)
		glAttachShader(mProgram, std::get<2>(s));

	glLinkProgram(mProgram);

	int isOK;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &isOK);

	if(!isOK)
	{
		note("Failed to link shader.");

		GLchar errorLog[ERROR_BUFSIZE];
		GLsizei length;

		glGetProgramInfoLog(mProgram, ERROR_BUFSIZE, &length, errorLog);

		if(length > 0)
		{
			note(errorLog);
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

	showInfo(/*fileName*/);

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

	note("Shader linked.");
	return true;
}

//=========================================================================
std::string Shader::readSource(std::string fileName)
{
	std::ifstream source(fileName);
	std::string line;
	std::string search = "#include";
	std::stringstream  ss;

	if(!source.fail())
	{
		unsigned int curLine = 0;

		while(std::getline(source, line))
		{
			curLine++;

			if(StringUtils::startsWith(line, search))
			{
				// Extract file name
				line = StringUtils::extract(line, '"');

				// Extract path name
				fileName = StringUtils::cutTail(fileName, "\\/");

				// Include file content
				ss << readSource(fileName + line);
			}
			else
				ss << line << std::endl;
		}

		return ss.str();
	}
	else
	{
		error("Failed to load shader: %s", fileName.c_str());
		return std::string();
	}
}

/// <summary>
/// Dumps various shader information to the console.
/// </summary>
void Shader::showInfo(/*const std::string& fileName*/)
{
     note("------------------------------------------------");
//     log("Shader: %s\n", fileName.c_str());
//     log("Program ID: %s\n", std::to_string(mProgram).c_str());
//     log("VertexShader ID: %s\n", std::to_string(mVertexShaderID).c_str());
//     log("FragmentShader ID: %s\n", std::to_string(mFragmentShaderID).c_str());
//     log("\n");
	note("Uniforms:");

	for(auto i = mUniformMap.begin(); i != mUniformMap.end(); ++i)
	{
		note("  %s -> %d", i->first.c_str(), i->second);
	}

	//log("\n");
	note("Attributes:");

	for(auto i = mAttributeMap.begin(); i != mAttributeMap.end(); ++i)
	{
		note("  %s -> %d", i->first.c_str(), i->second);
	}

	note("------------------------------------------------");
}

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
void Shader::reload()
{
	for(const auto& s : mShaderVec)
	{
		glDetachShader(mProgram, std::get<2>(s));
        glDeleteShader(std::get<2>(s));
	}

    auto shaderVec = mShaderVec;

	mShaderVec.clear();
	mAttributeMap.clear();
	mUniformMap.clear();

    for (const auto& s : shaderVec)
        loadFromFile(std::get<1>(s), std::get<0>(s));

	link();

	note("Shader reloaded.");
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
		//error("Attribute '%s' not found\n", name.c_str());
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
		//error("Uniform '%s' not found\n", name.c_str());
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