#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <string>

#include <GL/glew.h>
#include "Shader.h"



class ShaderProgram
{
public:
    ShaderProgram();

    ~ShaderProgram();

    GLuint programId() const				{ return m_programId; }
    Shader* vertShader() const				{ return m_vertShader; }
    Shader* fragShader() const				{ return m_fragShader; }

    inline void startUseProgram()					{ glUseProgram(m_programId); }
    inline void stopUseProgram()					{ glUseProgram(0);	}

protected:

    GLuint m_programId;
    Shader* m_vertShader;
    Shader* m_fragShader;
    Shader* m_geomShader;


    /// link information error if necessary
    bool printInfoLinkProgram();

    /**
     * @brief load & compile shaders
     * @param vert_name vertex shader file name
     * @param frag_name fragment shader file name
     * @param geom_name fragment shader file name
     */
    void load(const std::string& vert_name, const std::string& frag_name, const std::string& geom_name);
    void load(const std::string& vert_name, const std::string& frag_name);

};

#endif
