#ifndef SHADERPROGRAM_BEZIER_H
#define SHADERPROGRAM_BEZIER_H

#include <stdio.h>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include "ShaderProgram.h"



class ShaderProgramBezier: public ShaderProgram
{
public:
    /// uniform id pour matrice de projection
    GLint idOfProjectionMatrix;

    /// uniform id pour matrice de model-view
    GLint idOfViewMatrix;

    /// uniform id pour couleur
    GLint idOfColor;

    /// attribute id
    GLuint idOfVertexAttribute;

    ShaderProgramBezier();

};

#endif
