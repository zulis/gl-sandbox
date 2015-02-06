#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "core/GL.h"
#include "core/ShaderConstants.h"
#include "core/Math.h"
#include "core/Log.h"
#include "core/Color.h"
#include "core/StringUtils.h"

typedef std::shared_ptr<class Shader> ShaderRef;

class ShaderCollection
{
public:
	typedef std::unordered_map<std::string, ShaderRef> ResourceMap;

	static bool find(const std::string& fileName);
	static void add(const std::string& fileName, ShaderRef shader);
	static ShaderRef get(const std::string& fileName);
private:
	static ResourceMap sShaderList;
};

ShaderCollection::ResourceMap ShaderCollection::sShaderList;

//=========================================================================
bool ShaderCollection::find(const std::string& fileName)
{
	auto it = sShaderList.find(fileName);

	if (it == sShaderList.end())
		return false;
	else
		return true;
}

//=========================================================================
void ShaderCollection::add(const std::string& fileName, ShaderRef shader)
{
	if (!find(fileName))
		sShaderList[fileName] = shader;
}

//=========================================================================
ShaderRef ShaderCollection::get(const std::string& fileName)
{
	auto it = sShaderList.find(fileName);

	if (it != sShaderList.end())
		return it->second;
	else
		return NULL;
}

#define ERROR_BUFSIZE 1024

class Shader
{
public:
	static ShaderRef create(const std::string& fileName);

	Shader(const std::string& fileName);
	~Shader();

	bool isLoaded() const;
	const std::string& errors() const;
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
	std::unordered_map<std::string, GLuint> mAttributeMap;
	std::unordered_map<std::string, GLuint> mUniformMap;
	GLuint mVertexShaderID;
	GLuint mFragmentShaderID;
	GLuint mProgramID;
	bool mIsLoaded;
	std::string mErrors;

	void loadFromFile(const std::string& fileName);
	std::string readSource(std::string& path);
	void dumpShaderInfo(const std::string& fileName);
};

//=========================================================================
ShaderRef Shader::create(const std::string& fileName)
{
	//return ShaderRef(new Shader(fileName));

	if (ShaderCollection::find(fileName))
		return ShaderCollection::get(fileName);
	else
	{
		auto shader = std::make_shared<Shader>(fileName);
		ShaderCollection::add(fileName, shader);
		return shader;
	}
}

//=========================================================================
Shader::Shader(const std::string& fileName)
{
	loadFromFile(fileName);
}

//=========================================================================
Shader::~Shader()
{
	unbind();

	if(mVertexShaderID > 0)
		glDeleteShader(mVertexShaderID);

	if(mFragmentShaderID > 0)
		glDeleteShader(mFragmentShaderID);

	if(mProgramID > 0)
		glDeleteProgram(mProgramID);

	mAttributeMap.clear();
	mUniformMap.clear();
}

//=========================================================================
void Shader::loadFromFile(const std::string& fileName)
{
	// Load the fragment shader and compile
	std::string fragmentSource = readSource(fileName + ".frag");
	const char* source = fragmentSource.c_str();
	mFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mFragmentShaderID, 1, &source, NULL);
	glCompileShader(mFragmentShaderID);

	// Load the vertex shader and compile
	std::string vertexSource = readSource(fileName + ".vert");
	source = vertexSource.c_str();
	mVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mVertexShaderID, 1, &source, NULL);
	glCompileShader(mVertexShaderID);

	// Create the vertex program
	mProgramID = glCreateProgram();
	glAttachShader(mProgramID, mFragmentShaderID);
	glAttachShader(mProgramID, mVertexShaderID);
	glLinkProgram(mProgramID);

	// Error checking
	glGetProgramiv(mProgramID, GL_LINK_STATUS, (GLint*)&mIsLoaded);

	if(!mIsLoaded)
	{
		GLchar tempErrorLog[ERROR_BUFSIZE];
		GLsizei length;

		glGetShaderInfoLog(mFragmentShaderID, ERROR_BUFSIZE, &length, tempErrorLog);

		if(length > 0)
		{
			mErrors += "Fragment shader errors:\n";
			mErrors += std::string(tempErrorLog, length) + "\n";
		}

		glGetShaderInfoLog(mVertexShaderID, ERROR_BUFSIZE, &length, tempErrorLog);

		if(length > 0)
		{
			mErrors += "Vertex shader errors:\n";
			mErrors += std::string(tempErrorLog, length) + "\n";
		}

		glGetProgramInfoLog(mProgramID, ERROR_BUFSIZE, &length, tempErrorLog);

		if(length > 0)
		{
			mErrors += "Linker errors:\n";
			mErrors += std::string(tempErrorLog, length) + "\n";
		}

		logError("Could not load shader %s\n", fileName.c_str());
		logError("-------------------------------------------------------------------------\n");
		logError("%s", mErrors.c_str());
		logError("-------------------------------------------------------------------------\n");
	}
	else
	{
		int maxUniforms;
		glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORMS, &maxUniforms);
		int maxUlen;
		glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUlen);

		GLchar* ubuf = (char*)malloc(maxUlen + 1);

		for(int i = 0; i < maxUniforms; i++)
		{
			int size;
			GLenum type;
			glGetActiveUniform(mProgramID, i, maxUlen, NULL, &size, &type, ubuf);

			mUniformMap[ubuf] = glGetUniformLocation(mProgramID, ubuf);
		}

		free(ubuf);

		int maxAttributes;
		glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES, &maxAttributes);
		int maxAlen;
		glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAlen);

		GLchar* abuf = (char*)malloc(maxAlen + 1);

		for(int i = 0; i < maxAttributes; i++)
		{
			int size;
			GLenum type;
			glGetActiveAttrib(mProgramID, i, maxAlen, NULL, &size, &type, abuf);

			mAttributeMap[abuf] = glGetAttribLocation(mProgramID, abuf);
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
	}
}

//=========================================================================
std::string Shader::readSource(std::string& path)
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

	return ss.str();
}

/// <summary>
/// Dumps various shader information to the console.
/// </summary>
void Shader::dumpShaderInfo(const std::string& fileName)
{
	logNote("------------------------------------------------\n");
	logNote("Shader: %s\n", fileName.c_str());
	logNote("Program ID: %s\n", std::to_string(mProgramID).c_str());
	logNote("VertexShader ID: %s\n", std::to_string(mVertexShaderID).c_str());
	logNote("FragmentShader ID: %s\n", std::to_string(mFragmentShaderID).c_str());
	logNote("\n");
	logNote("Uniforms:\n");

	for(auto i = mUniformMap.begin(); i != mUniformMap.end(); ++i)
	{
		logNote("  %s -> %d\n", i->first.c_str(), i->second);
	}

	logNote("\n");
	logNote("Attributes:\n");

	for(auto i = mAttributeMap.begin(); i != mAttributeMap.end(); ++i)
	{
		logNote("  %s -> %d\n", i->first.c_str(), i->second);
	}

	logNote("------------------------------------------------\n");
}

//=========================================================================
bool Shader::isLoaded() const
{
	return mIsLoaded;
}

//=========================================================================
const std::string& Shader::errors() const
{
	return mErrors;
}

//=========================================================================
void Shader::bind()
{
	glUseProgram(mProgramID);
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
	if (uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string& name, unsigned int i) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
		glUniform1f(uniform, f);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
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
	if (uniform != -1)
		glUniform3f(uniform, f1, f2, f3);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2, float f3, float f4) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
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
	if (uniform != -1)
		glUniform4fv(uniform, count, (GLfloat*)vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const vec4& vec4) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
		glUniform4fv(uniform, 1, (GLfloat*)&vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const mat3& mat3, bool transpose) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
		glUniformMatrix3fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat3[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const mat4& mat4, bool transpose) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
		glUniformMatrix4fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat4[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, bool b) const
{
	auto uniform = getUniform(name);
	if (uniform != -1)
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

	if (it != mAttributeMap.end())
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

	if (it != mUniformMap.end())
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