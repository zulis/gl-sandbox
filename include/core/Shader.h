#pragma once

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
#include "core/ShaderConstants.h"

class Shader
{
public:
	enum class SourceType
	{
		File,
		String
	};

	Shader(const std::string &fileName, const SourceType& sourceType = SourceType::File);
	~Shader();

	void bind();
	void unbind();
	//void reload();

	//void setUniform(const std::string &name, const Texture* tex);
	//void setUniform(const std::string &name, const TextureCube* tex);
	void setUniform(const std::string &name, int i) const;
	void setUniform(const std::string &name, unsigned int i) const;
	void setUniform(const std::string &name, float f) const;
	void setUniform(const std::string &name, float f1, float f2) const;
	void setUniform(const std::string &name, const vec2& vec2) const;
	void setUniform(const std::string &name, float f1, float f2, float f3) const;
	void setUniform(const std::string &name, float f1, float f2, float f3, float f4) const;
	void setUniform(const std::string &name, const vec3& vec3) const;
	//void setUniform(const std::string &name, const vec4& vec4) const;
	void setUniform(const std::string &name, const mat3& mat3, bool transpose = false) const;
	void setUniform(const std::string &name, const mat4& mat4, bool transpose = false) const;
	void setUniform(const std::string &name, bool b) const;
	void setUniform(const std::string &name, const vec4* vec4, int count) const;
	void setUniform(const std::string &name, const vec4& vec4) const;
	void setUniform(const std::string &name, const Color& color) const;

	GLuint getAttribute(const std::string &name) const;
	GLuint getUniform(const std::string &name) const;

	bool hasAttribute(const std::string &name) const;
	bool hasUniform(const std::string &name) const;

private:
	enum ShaderType { Vertex, Geometry, Fragment };
	std::vector<std::tuple<ShaderType, std::string, GLuint>> mShaderVec;
	std::unordered_map<std::string, GLuint> mAttributeMap;
	std::unordered_map<std::string, GLuint> mUniformMap;
	GLuint mProgram;
	std::string mFileName;

private:
	std::string parseSource(const std::string &source);
	void showInfo();
	std::string readFile(const std::string &fileName);
	bool loadFromRaw(std::string &vertexShader, std::string &geometryShader, std::string &fragmentShader);
	bool loadFromString(const std::string &source, const ShaderType& shaderType);
	bool link();
};

//=========================================================================
Shader::Shader(const std::string &source, const SourceType& sourceType)
{
	std::string shaderText;
	std::string vsPattern = "[Vertex]";
	std::string gsPattern = "[Geometry]";
	std::string fsPattern = "[Fragment]";

	if (sourceType == SourceType::File)
	{
		mFileName = source;
		std::ifstream stream(source);

		if (stream.fail())
		{
			error("Failed to load shader: %s", source.c_str());
			stream.close();
			return;
		}

		stream.close();
		shaderText = readFile(source);
	}
	else
	{
		mFileName = "from string";
		const char *sourceArray[1] = { source.c_str() };
		shaderText = source;
	}

	size_t vsPos = shaderText.find(vsPattern);
	size_t gsPos = shaderText.find(gsPattern);
	size_t fsPos = shaderText.find(fsPattern);

	std::string vsText, gsText, fsText;

	if (vsPos != std::string::npos)
	{
		vsText = shaderText.substr(vsPos);
		vsText = vsText.substr(0, vsText.find(gsPattern));
		vsText = vsText.substr(0, vsText.find(fsPattern));
		vsText.erase(vsText.find(vsPattern), vsPattern.length());
	}

	if (gsPos != std::string::npos)
	{
		gsText = shaderText.substr(gsPos);
		gsText = gsText.substr(0, gsText.find(vsPattern));
		gsText = gsText.substr(0, gsText.find(fsPattern));
		gsText.erase(gsText.find(gsPattern), gsPattern.length());
	}

	if (fsPos != std::string::npos)
	{
		fsText = shaderText.substr(fsPos);
		fsText = fsText.substr(0, fsText.find(vsPattern));
		fsText = fsText.substr(0, fsText.find(gsPattern));
		fsText.erase(fsText.find(fsPattern), fsPattern.length());
	}

	if (loadFromRaw(vsText, gsText, fsText))
		link();
}

//=========================================================================
Shader::~Shader()
{
	unbind();

	for (const auto& s : mShaderVec)
		glDeleteShader(std::get<2>(s));

	if (mProgram)
		glDeleteProgram(mProgram);

	mShaderVec.clear();
	mAttributeMap.clear();
	mUniformMap.clear();

	note("Shader released.");
}

//=========================================================================
bool Shader::loadFromRaw(std::string &vertexShader, std::string &geometryShader, std::string &fragmentShader)
{
	bool vsResult = true;
	bool gsResult = true;
	bool fsResult = true;

	if (!vertexShader.empty())
	{
		vertexShader = parseSource(vertexShader);
		vsResult = loadFromString(vertexShader, ShaderType::Vertex);
	}

	if (!geometryShader.empty())
	{
		geometryShader = parseSource(geometryShader);
		gsResult = loadFromString(geometryShader, ShaderType::Geometry);
	}

	if (!fragmentShader.empty())
	{
		fragmentShader = parseSource(fragmentShader);
		fsResult = loadFromString(fragmentShader, ShaderType::Fragment);
	}

	return vsResult && gsResult && fsResult;
}

//=========================================================================
bool Shader::loadFromString(const std::string &source, const ShaderType& shaderType)
{
	GLenum type;
	char* typeStr;

	switch (shaderType)
	{
	case ShaderType::Vertex:
		type = GL_VERTEX_SHADER;
		typeStr = "vertex";
		break;

	case ShaderType::Geometry:
		type = GL_GEOMETRY_SHADER;
		typeStr = "geometry";
		break;

	case ShaderType::Fragment:
		type = GL_FRAGMENT_SHADER;
		typeStr = "fragment";
		break;
	}

	if (source != std::string())
	{
		GLuint shader = glCreateShader(type);
		const char *sourceArray[1] = { source.c_str() };
		glShaderSource(shader, 1, sourceArray, NULL);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			error("Failed to compile %s shader: %s", typeStr, mFileName.c_str());

			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* errorLog = new GLchar[infoLogLength + 1];
			GLsizei length;

			glGetShaderInfoLog(shader, infoLogLength, &length, errorLog);

			if (length > 0)
				error(errorLog);

			glDeleteShader(shader);

			return false;
		}

		mShaderVec.push_back(std::make_tuple(shaderType, mFileName, shader));

		note("Shader loaded (%s): %s", typeStr, mFileName.c_str());
		return true;
	}
	else
		return false;
}

//=========================================================================
bool Shader::link()
{
	mProgram = glCreateProgram();

	for (const auto& s : mShaderVec)
		glAttachShader(mProgram, std::get<2>(s));

	glLinkProgram(mProgram);

	int isOK;
	glGetProgramiv(mProgram, GL_LINK_STATUS, &isOK);

	if (!isOK)
	{
		note("Failed to link shader.");

		GLint infoLogLength;
		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* errorLog = new GLchar[infoLogLength + 1];
		GLsizei length;

		glGetProgramInfoLog(mProgram, infoLogLength, &length, errorLog);

		if (length > 0)
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

	for (int i = 0; i < maxUniforms; i++)
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

	for (int i = 0; i < maxAttributes; i++)
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
std::string Shader::parseSource(const std::string &source)
{
	std::string line;
	std::string search = "#include";
	std::stringstream  strStream;
	std::istringstream sourceStream(source);

	unsigned int curLine = 0;

	while (std::getline(sourceStream, line))
	{
		curLine++;

		if (StringUtils::startsWith(line, search))
		{
			// Extract file name
			line = StringUtils::extract(line, '"');

			// Extract path name
			std::string path = StringUtils::cutTail(mFileName.c_str(), "\\/");

			// Include file content
			if (line == "core")
				strStream << ShaderConstants::CoreShaderSource;
			else
				strStream << readFile(path + line);
		}
		else
			strStream << line << std::endl;
	}

	return strStream.str();
}

/// <summary>
/// Dumps various shader information to the console.
/// </summary>
void Shader::showInfo()
{
	// Create orderred std::map using uniform/attribute ID as a key (for prety print)
	std::map<GLuint, std::string> uniformMap;
	for (auto i = mUniformMap.begin(); i != mUniformMap.end(); ++i)
		uniformMap[i->second] = i->first;

	std::map<GLuint, std::string> attributeMap;
	for (auto i = mAttributeMap.begin(); i != mAttributeMap.end(); ++i)
		attributeMap[i->second] = i->first;


	note("------------------------------------------------");
	//     log("Shader: %s\n", fileName.c_str());
	//     log("Program ID: %s\n", std::to_string(mProgram).c_str());
	//     log("VertexShader ID: %s\n", std::to_string(mVertexShaderID).c_str());
	//     log("FragmentShader ID: %s\n", std::to_string(mFragmentShaderID).c_str());
	//     log("\n");
	note("Uniforms:");

	for (auto i = uniformMap.begin(); i != uniformMap.end(); ++i)
	{
		note("  %s -> %d", i->second.c_str(), i->first);
	}

	//log("\n");
	note("Attributes:");

	for (auto i = attributeMap.begin(); i != attributeMap.end(); ++i)
	{
		note("  %s -> %d", i->second.c_str(), i->first);
	}

	note("------------------------------------------------");
}

//=========================================================================
std::string Shader::readFile(const std::string &fileName)
{
	std::string result;
	std::ifstream file(fileName, std::ios::in | std::ios::binary);

	if (file)
	{
		file.seekg(0, std::ios::end);
		result.resize(static_cast<unsigned int>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();
	}

	return result;
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

/*
//=========================================================================
void Shader::reload()
{
	for (const auto& s : mShaderVec)
	{
		glDetachShader(mProgram, std::get<2>(s));
		glDeleteShader(std::get<2>(s));
	}

	auto shaderVec = mShaderVec;

	mShaderVec.clear();
	mAttributeMap.clear();
	mUniformMap.clear();

	for (const auto& s : shaderVec)
		loadFromString(std::get<1>(s), std::get<0>(s));

	link();

	note("Shader reloaded.");
}
*/

//=========================================================================
//void Shader::setUniform(const std::string &name, const Texture* tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
//void Shader::setUniform(const std::string &name, const TextureCube *tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//=========================================================================
void Shader::setUniform(const std::string &name, int i) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string &name, unsigned int i) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform1i(uniform, i);
}

//=========================================================================
void Shader::setUniform(const std::string &name, float f) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform1f(uniform, f);
}

//=========================================================================
void Shader::setUniform(const std::string &name, float f1, float f2) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform2f(uniform, f1, f2);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const vec2& vec2) const
{
	setUniform(name, vec2.x, vec2.y);
}

//=========================================================================
void Shader::setUniform(const std::string &name, float f1, float f2, float f3) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform3f(uniform, f1, f2, f3);
}

//=========================================================================
void Shader::setUniform(const std::string &name, float f1, float f2, float f3, float f4) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform4f(uniform, f1, f2, f3, f4);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const vec3& vec3) const
{
	setUniform(name, vec3.x, vec3.y, vec3.z);
}

//=========================================================================
//void Shader::setUniform(const std::string &name, const vec4& vec4) const
//{
//	setUniform(name, vec4.x, vec4.y, vec4.z, vec4.w);
//}

//=========================================================================
void Shader::setUniform(const std::string &name, const vec4* vec4, int count) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform4fv(uniform, count, (GLfloat*)vec4);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const vec4& vec4) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform4fv(uniform, 1, (GLfloat*)&vec4);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const mat3& mat3, bool transpose) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniformMatrix3fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat3[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const mat4& mat4, bool transpose) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniformMatrix4fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat4[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string &name, bool b) const
{
	auto uniform = getUniform(name);

	if (uniform != -1)
		glUniform1i(uniform, b ? 1 : 0);
}

//=========================================================================
void Shader::setUniform(const std::string &name, const Color& color) const
{
	setUniform(name, color.r, color.g, color.b, color.a);
}

//=========================================================================
GLuint Shader::getAttribute(const std::string &name) const
{
	auto it = mAttributeMap.find(name);

	if (it != mAttributeMap.end())
		return it->second;
	else
	{
		//error("Attribute '%s' not found\n", name.c_str());
		return -1;
	}
}

//=========================================================================
GLuint Shader::getUniform(const std::string &name) const
{
	auto it = mUniformMap.find(name);

	if (it != mUniformMap.end())
		return it->second;
	else
	{
#ifdef _DEBUG
		error("Uniform '%s' not found\n", name.c_str());
#endif
		return -1;
	}
}

//=========================================================================
bool Shader::hasAttribute(const std::string &name) const
{
	auto it = mAttributeMap.find(name);

	if (it == mAttributeMap.end())
		return false;
	else
		return true;
}

//=========================================================================
bool Shader::hasUniform(const std::string &name) const
{
	auto it = mUniformMap.find(name);

	if (it == mUniformMap.end())
		return false;
	else
		return true;
}
