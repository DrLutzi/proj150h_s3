#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <ShaderProgramBezier.h>
#include "bezierpatch_manager.h"
#include <QTimer>

/**
 * @brief The Viewer class
 *
 *
 */
class Viewer : public QGLViewer
{
public:
    Viewer(QWidget *parent=NULL);
    ~Viewer();

    BezierPatch_Manager * manager();

    inline void setSelectionSize(float selectionSize) {m_selectionSize=selectionSize;}

protected:
    /// OpenGL intialisation called automatically
    void init();

    /// Program-specific render
    void render_init();

    ///refresh rate for CP selection
    void setRefreshRate(unsigned int refreshRate);

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

private slots:

    /// slot fired when timer out
    void update();

private:

    void updateSelectedCP();

    ///VAO Id (vbo+ebo)
    GLuint m_vaoId;
    ///VBO Id (positions)
    GLuint m_vboId;
    ///EBO Id (triangles, etc)
    GLuint m_eboId;
    /// shader prg
    ShaderProgramBezier* m_ShaderProgram;

    /// patch uniforme
    BezierPatch_Manager *m_manager;

    glm::vec3 m_origin, m_direction;
    bool m_selectedCP;
    /// old mouse position, for computing differences
    QPoint m_oldMousePos, m_deltaPos;
    /// distance between camera and selected point
    float m_distanceSelection;
    ///
    /// \brief m_selectionSize size of the control points hitboxes for selection
    ///
    float m_selectionSize;

    GLenum m_surfacePolygonMode;

    ///A timer to limit the number of times a scene can be updated
    QTimer                          m_refreshTimer;

    ///refresh rate of the manager
    unsigned int                    m_refreshRate;

    ///indicates if an update function has been recently called or not
    bool                            m_waitingUpdate;
};

#endif
