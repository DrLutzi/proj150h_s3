#include "ShaderProgramBezier.h"


ShaderProgramBezier::ShaderProgramBezier()
{
    // load & compile & link shaders
    load("shaderBezier.vert","shaderBezier.frag");

    // get id of uniforms
    idOfProjectionMatrix = glGetUniformLocation(m_programId, "projectionMatrix");
    idOfViewMatrix = glGetUniformLocation(m_programId, "viewMatrix");

    idOfColor = glGetUniformLocation(m_programId, "fsu_color");

    // set id of attribute and bind
    idOfVertexAttribute = 0;
    glBindAttribLocation(m_programId, idOfVertexAttribute, "vertex_in");

}
