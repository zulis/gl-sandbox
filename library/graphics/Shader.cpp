#include "Shader.h"
#include "GL.h"
#include "system/Log.h"
#include "system/StringUtils.h"
#include "system/Resource.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <tuple>
#include <fstream>

namespace library
{

class ShaderHandler{
public:
    enum class SourceType
    {
        File,
        String
    };

    ShaderHandler(const std::string &source, const SourceType& sourceType);
    ~ShaderHandler();

    void bind();
    GLuint getUniform(const std::string &name) const;

private:
    enum ShaderType { Vertex, Geometry, Fragment };
    std::vector<std::tuple<ShaderType, std::string, GLuint>> mShaderVec;
    std::unordered_map<std::string, GLuint> mAttributeMap;
    std::unordered_map<std::string, GLuint> mUniformMap;
    GLuint mProgram;
    std::string mSource;
    std::string readFile(const std::string &fileName);
    bool compileAll(std::string &vertexShader, std::string &geometryShader, std::string &fragmentShader);
    bool compile(const std::string &source, const ShaderType &shaderType);
    std::string parseSource(const std::string &source);
    bool link();
    void showInfo();
};

ShaderHandler::ShaderHandler(const std::string &source, const SourceType& sourceType)
{
    std::string shaderText;
    std::string vsPattern = "[Vertex]";
    std::string gsPattern = "[Geometry]";
    std::string fsPattern = "[Fragment]";

    if (sourceType == SourceType::File)
    {
        mSource = source;
        std::ifstream stream(source);

        if (stream.fail())
        {
            error("Failed to load shader: {}", source.c_str());
            stream.close();
            return;
        }

        stream.close();
        shaderText = readFile(source);
    }
    else
    {
        mSource = "from string";
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

    if (compileAll(vsText, gsText, fsText))
        link();
}

ShaderHandler::~ShaderHandler()
{
    for (const auto& s : mShaderVec)
        glDeleteShader(std::get<2>(s));

    if (mProgram)
        glDeleteProgram(mProgram);

    mShaderVec.clear();
    mAttributeMap.clear();
    mUniformMap.clear();

    note("Shader released.");
}

std::string ShaderHandler::readFile(const std::string &fileName)
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

bool ShaderHandler::compileAll(std::string &vertexShader, std::string &geometryShader, std::string &fragmentShader)
{
    bool vsResult = true;
    bool gsResult = true;
    bool fsResult = true;

    if (!vertexShader.empty())
    {
        vertexShader = parseSource(vertexShader);
        vsResult = compile(vertexShader, ShaderType::Vertex);
    }

    if (!geometryShader.empty())
    {
        geometryShader = parseSource(geometryShader);
        gsResult = compile(geometryShader, ShaderType::Geometry);
    }

    if (!fragmentShader.empty())
    {
        fragmentShader = parseSource(fragmentShader);
        fsResult = compile(fragmentShader, ShaderType::Fragment);
    }

    return vsResult && gsResult && fsResult;
}

bool ShaderHandler::compile(const std::string &source, const ShaderType &shaderType)
{
    GLenum type;
    std::string typeStr;

    switch (shaderType)
    {
        case ShaderType::Vertex:
            type = GL_VERTEX_SHADER;
            typeStr = "Vertex";
            break;

        case ShaderType::Geometry:
            type = GL_GEOMETRY_SHADER;
            typeStr = "Geometry";
            break;

        case ShaderType::Fragment:
            type = GL_FRAGMENT_SHADER;
            typeStr = "Fragment";
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
            error("Failed to compile %s shader: {}", typeStr, mSource.c_str());

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

        mShaderVec.push_back(std::make_tuple(shaderType, mSource, shader));

        note("{} shader ({}) compiled", typeStr, mSource.c_str());
        return true;
    }
    else
        return false;
}

std::string ShaderHandler::parseSource(const std::string &source)
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
            std::string path = StringUtils::cutTail(mSource.c_str(), "\\/");

            // Include file content
            if (line == "core")
                //strStream << ShaderConstants::CoreShaderSource;
                int tmp = 0;
            else
                strStream << readFile(path + line);
        }
        else
            strStream << line << std::endl;
    }

    return strStream.str();
}

bool ShaderHandler::link()
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

void ShaderHandler::showInfo()
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
        note("  {} -> {}", i->second.c_str(), i->first);
    }

    //log("\n");
    note("Attributes:");

    for (auto i = attributeMap.begin(); i != attributeMap.end(); ++i)
    {
        note("  {} -> {}", i->second.c_str(), i->first);
    }

    note("------------------------------------------------");
}

void ShaderHandler::bind()
{
    glUseProgram(mProgram);
}

GLuint ShaderHandler::getUniform(const std::string &name) const
{
    auto it = mUniformMap.find(name);

    if (it != mUniformMap.end())
        return it->second;
    else
    {
#ifdef _DEBUG
        error("Uniform '{}' not found", name);
#endif
        return -1;
    }
}

//=========================================================================

class Shader::Impl
{
public:
    std::shared_ptr<ShaderHandler> shaderRef;
    std::string fileName;
    GLuint getUniform(const std::string &name) const;
};

GLuint Shader::Impl::getUniform(const std::string &name) const
{
    return shaderRef->getUniform(name);
}

//=========================================================================

Shader::Shader()
    : impl{std::make_unique<Impl>()}
{
}

Shader::~Shader()
{
}

void Shader::fromFile(const std::string &fileName)
{
    impl->fileName = fileName;
    impl->shaderRef = Resource::get<ShaderHandler>(fileName, ShaderHandler::SourceType::File);
}

void Shader::fromString(const std::string &source)
{
    impl->shaderRef = Resource::get<ShaderHandler>(source, ShaderHandler::SourceType::String);
}

void Shader::bind()
{
    impl->shaderRef->bind();
}

void Shader::reload()
{
    if(impl->fileName != std::string())
    {
        note("Reloading shader");
        impl->shaderRef.reset();
        impl->shaderRef = std::make_shared<ShaderHandler>(impl->fileName, ShaderHandler::SourceType::File);
    }
    else{
        note("Shaders from string can not be reloaded!");
    }
}

//void Shader::setUniform(const std::string &name, const Texture* tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

//void Shader::setUniform(const std::string &name, const TextureCube *tex)
//{
//	glUniform1i(getUniform(name), tex->index);
//}

void Shader::setUniform(const std::string &name, int i) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform1i(uniform, i);
}

void Shader::setUniform(const std::string &name, unsigned int i) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform1i(uniform, i);
}

void Shader::setUniform(const std::string &name, float f) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform1f(uniform, f);
}

void Shader::setUniform(const std::string &name, float f1, float f2) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform2f(uniform, f1, f2);
}

void Shader::setUniform(const std::string &name, const vec2& vec2) const
{
    setUniform(name, vec2.x, vec2.y);
}

void Shader::setUniform(const std::string &name, float f1, float f2, float f3) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform3f(uniform, f1, f2, f3);
}

void Shader::setUniform(const std::string &name, float f1, float f2, float f3, float f4) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform4f(uniform, f1, f2, f3, f4);
}

void Shader::setUniform(const std::string &name, const vec3& vec3) const
{
    setUniform(name, vec3.x, vec3.y, vec3.z);
}

//void Shader::setUniform(const std::string &name, const vec4& vec4) const
//{
//	setUniform(name, vec4.x, vec4.y, vec4.z, vec4.w);
//}

void Shader::setUniform(const std::string &name, const vec4* vec4, int count) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform4fv(uniform, count, (GLfloat*)vec4);
}

void Shader::setUniform(const std::string &name, const vec4& vec4) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform4fv(uniform, 1, (GLfloat*)&vec4);
}

void Shader::setUniform(const std::string &name, const mat3& mat3, bool transpose) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniformMatrix3fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat3[0][0]);
}

void Shader::setUniform(const std::string &name, const mat4& mat4, bool transpose) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniformMatrix4fv(uniform, 1, transpose ? GL_TRUE : GL_FALSE, &mat4[0][0]);
}

void Shader::setUniform(const std::string &name, bool b) const
{
    auto uniform = impl->getUniform(name);

    if (uniform != -1)
        glUniform1i(uniform, b ? 1 : 0);
}

/*void Shader::setUniform(const std::string &name, const Color& color) const
{
    setUniform(name, color.r, color.g, color.b, color.a);
}*/

}
