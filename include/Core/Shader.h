#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "Core/ShaderConstants.h"
#include "Core/Math.h"
#include "core/GL.h"

typedef std::shared_ptr<class Shader> ShaderRef;

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
		void setUniform(const std::string& name, float f) const;
		void setUniform(const std::string& name, float f1, float f2) const;
		void setUniform(const std::string& name, const glm::vec2 vec2) const;
		void setUniform(const std::string& name, float f1, float f2, float f3) const;
		void setUniform(const std::string& name, float f1, float f2, float f3, float f4) const;
		void setUniform(const std::string& name, const glm::vec3& vec3) const;
		//void setUniform(const std::string& name, const vec4& vec4) const;
		void setUniform(const std::string& name, const glm::mat3& mat3, bool transpose = false) const;
		void setUniform(const std::string& name, const glm::mat4& mat4, bool transpose = false) const;
		void setUniform(const std::string& name, bool b) const;
		void setUniform(const std::string& name, const glm::vec4* vec4, int count) const;
		void setUniform(const std::string& name, const glm::vec4 vec4) const;

		GLuint getAttribute(const std::string& name) const;
		GLuint getUniform(const std::string& name) const;

		bool hasAttribute(const std::string& name) const;
		bool hasUniform(const std::string& name) const;

	private:
		std::map<const std::string, GLuint> mAttributeMap;
		std::map<const std::string, GLuint> mUniformMap;
		GLuint mVertexShaderID;
		GLuint mFragmentShaderID;
		GLuint mProgramID;
		bool mIsLoaded;
		std::string mErrors;

		void loadFromFile(const std::string& fileName);
		std::vector<char> readSource(const std::string& path);
};

//=========================================================================
ShaderRef Shader::create(const std::string& fileName)
{
	return ShaderRef(new Shader(fileName));
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
	const GLchar* source[1];
	int length = 0;

	// Load the fragment shader and compile
	std::vector<char> fragmentSource = readSource(fileName + ".frag");
	source[0] = &fragmentSource.front();
	length = fragmentSource.size()-1;
	mFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mFragmentShaderID, 1, source, &length);
	glCompileShader(mFragmentShaderID);

	// Load the vertex shader and compile
	std::vector<char> vertexSource = readSource(fileName + ".vert");
	source[0] = &vertexSource.front();
	length = vertexSource.size()-1;
	mVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mVertexShaderID, 1, source, &length);
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

		printf("Could not load shader %s\n", fileName.c_str());
		printf("-------------------------------------------------------------------------\n");
		printf(mErrors.c_str());
		printf("-------------------------------------------------------------------------\n");
	}
	else
	{
		int maxUniforms;
		glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORMS,&maxUniforms);
		int maxUlen;
		glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxUlen);

		GLchar* ubuf=(char*)malloc(maxUlen+1);

		for(int i=0 ; i < maxUniforms; i++)
		{
			int size;
			GLenum type;
			glGetActiveUniform(mProgramID,i,maxUlen,NULL,&size,&type,ubuf);

			mUniformMap[ubuf] = glGetUniformLocation(mProgramID, ubuf);
		}

		free(ubuf);

		int maxAttributes;
		glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES,&maxAttributes);
		int maxAlen;
		glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAlen);

		GLchar* abuf=(char*)malloc(maxAlen+1);

		for(int i=0 ; i < maxAttributes; i++)
		{
			int size;
			GLenum type;
			glGetActiveAttrib(mProgramID, i, maxAlen, NULL, &size, &type, abuf);

			mAttributeMap[abuf] = glGetAttribLocation(mProgramID, abuf);
		}

		free(abuf);
	}
}

//=========================================================================
std::vector<char> Shader::readSource(const std::string& path)
{
	// Open the file
	std::vector<char> source;
	std::ifstream in(path.c_str());

	if(in.fail())
	{
		printf("Shader file doesn't exist %s\n", path.c_str());
		source.push_back(0);
		return source;
	}

	// Seek to the end of the file to get the size
	in.seekg(0, std::ios::end);
	source.reserve((unsigned)(1 + in.tellg()));
	source.resize((unsigned)in.tellg());
	in.seekg(0, std::ios::beg);

	if(source.empty())
	{
		source.push_back(0);
		return source;
	}

	// Now read the whole buffer in one call, and don't
	// forget to null-terminate the vector with a zero
	in.read(&source.front(), source.size());
	source.push_back(0);

	return source;
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
	glUniform1i(getUniform(name), i);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f) const
{
	glUniform1f(getUniform(name), f);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2) const
{
	glUniform2f(getUniform(name), f1, f2);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::vec2 vec2) const
{
	setUniform(name, vec2.x, vec2.y);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2, float f3) const
{
	glUniform3f(getUniform(name), f1, f2, f3);
}

//=========================================================================
void Shader::setUniform(const std::string& name, float f1, float f2, float f3, float f4) const
{
	glUniform4f(getUniform(name), f1, f2, f3, f4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::vec3& vec3) const
{
	setUniform(name, vec3.x, vec3.y, vec3.z);
}

//=========================================================================
//void Shader::setUniform(const std::string& name, const vec4& vec4) const
//{
//	setUniform(name, vec4.x, vec4.y, vec4.z, vec4.w);
//}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::vec4* vec4, int count) const
{
	glUniform4fv(getUniform(name), count, (GLfloat*)vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::vec4 vec4) const
{
	glUniform4fv(getUniform(name), 1, (GLfloat*)&vec4);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::mat3& mat3, bool transpose) const
{
	glUniformMatrix3fv(getUniform(name), 1, transpose ? GL_TRUE : GL_FALSE, &mat3[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, const glm::mat4& mat4, bool transpose) const
{
	glUniformMatrix4fv(getUniform(name), 1, transpose ? GL_TRUE : GL_FALSE, &mat4[0][0]);
}

//=========================================================================
void Shader::setUniform(const std::string& name, bool b) const
{
	glUniform1i(getUniform(name), b ? 1 : 0);
}

//=========================================================================
GLuint Shader::getAttribute(const std::string& name) const
{
	if(hasAttribute(name))
		return mAttributeMap.find(name)->second;
	else
	{
		printf("Attribute '%s' not found\n", name.c_str());
		return NULL;
	}
}

//=========================================================================
GLuint Shader::getUniform(const std::string& name) const
{
	if(hasUniform(name))
		return mUniformMap.find(name)->second;
	else
	{
		printf("Uniform '%s' not found\n", name.c_str());
		return NULL;
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
