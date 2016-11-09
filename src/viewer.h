#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <glm/glm.hpp>
#include <ShaderProgramBezier.h>
#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"


/**
 * @brief The Viewer class
 *
 *
 */
class Viewer : public QGLViewer
{
protected:
	/// OpenGL intialisation appelee par la QGLViewer
    void init();

    /// update patch
    void updatePatch();

    ///dessine les lignes d'un patch
    void drawPatchLines();

    ///dessine les points de controle du patch
    void drawPatchControlPoints();

    ///dessine la surface/courbe de Bézier définie par le patch
    void drawPatchBezier();

	/// draw callback de la QGLViewer
    void draw();

	/// callback when key pressed
    void keyPressEvent(QKeyEvent *e);

    /// callback when mouse pressed
    void mousePressEvent(QMouseEvent *e);

    /// callback when mouse released
    void mouseReleaseEvent(QMouseEvent *e);

    /// callback when mouse moved
    void mouseMoveEvent(QMouseEvent *e);

    static glm::vec3 vecToGlmVec3(const qglviewer::Vec &v);
    static qglviewer::Vec glmVec3ToVec(const glm::vec3 &v);

	/// recupere la matrice de modelview de la QGLViewer
	glm::mat4 getCurrentModelViewMatrix() const;

    /// recupere la matrice de projection de la QGLViewer
	glm::mat4 getCurrentProjectionMatrix() const;

    /// init specific au TP
    void tp_init();

    /// Donnees du TP
    /// id de VAO
    GLuint m_Vao;
    /// id de VBO pour les positions et les lignes des CP
    GLuint m_vbo_id;
    /// id de IBO pour desiner les triangles/lignes
    GLuint m_ebo_id;
    /// patch de bezier dans sa version rectangulaire
    BezierPatch_Rectangle *m_rectangularPatch;
    /// patch de bezier dans sa version triangulaire
    BezierPatch_Triangle *m_triangularPatch;
    /// patch uniforme
    BezierPatch *m_patch;
    /// shader prg
    ShaderProgramBezier* m_ShaderProgram;

private:

    glm::vec3 m_origin, m_direction;
    /// Cp sélectionné (NULL/Cp)
    glm::vec3 *m_selectedCP;
    /// old mouse position, for computing differences
    QPoint m_oldMousePos, m_deltaPos;
    /// distance between camera and selected point
    float m_distanceSelection;
    /// compute bezier surface for next frame
    bool m_drawBezier;
};

#endif
