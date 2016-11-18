#include "viewer.h"
#include <glm/gtc/type_ptr.hpp>
#include <QKeyEvent>

void Viewer::tp_init()
{
    m_oldMousePos=QPoint(0,0);
    m_deltaPos=QPoint(0,0);
    m_selectedCP=NULL;

    setMouseTracking(true);
    // genere les donnees a afficher

    /*m_patch = new BezierPatch_Rectangle(2,3);
    m_patch->setResolution(30);
    m_rectangularPatch = static_cast<BezierPatch_Rectangle*>(m_patch);

    m_rectangularPatch->setPoint(0,0, glm::vec3(-1,-1,-0.5));
    m_rectangularPatch->setPoint(0,1, glm::vec3(-1,1,-0.1));
    m_rectangularPatch->setPoint(0,2, glm::vec3(-1,1.8,0.2));


    m_rectangularPatch->setPoint(1,0, glm::vec3(1,-1,0.5));
    m_rectangularPatch->setPoint(1,1, glm::vec3(1,1,0));
    m_rectangularPatch->setPoint(1,2, glm::vec3(1,2,-0.2));
    */
    m_patch = new BezierPatch_Triangle(3);
    m_patch->setResolution(50);
    m_triangularPatch = static_cast<BezierPatch_Triangle*>(m_patch);

    m_triangularPatch->setPoint(0,0,2, glm::vec3(0,1,-0.1));

    m_triangularPatch->setPoint(1,0,1, glm::vec3(-0.5,0,0.1));
    m_triangularPatch->setPoint(0,1,1, glm::vec3(0.5,0,-0.2));

    m_triangularPatch->setPoint(2,0,0, glm::vec3(-1,-1,0));
    m_triangularPatch->setPoint(1,1,0, glm::vec3(0,-1,0));
    m_triangularPatch->setPoint(0,2,0, glm::vec3(1,-1,0));


    // SHADER
    m_ShaderProgram = new ShaderProgramBezier();

    //VBO
    glGenBuffers(1, &m_vbo_id);

    //EBO
    glGenBuffers(1, &m_ebo_id);

    // genere 1 VAO
    glGenVertexArrays(1, &m_Vao);

    // on travaille sur celui-ci
    glBindVertexArray(m_Vao);

    m_patch->makeVBO(m_vbo_id, m_ebo_id);
    m_patch->updateVBO_CP(m_vbo_id, m_ebo_id);

    //associe l'ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id);

    // associe le VBO de position
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    // avec l'attribut position du shader
    glEnableVertexAttribArray(m_ShaderProgram->idOfVertexAttribute);
    // en donne les bon parametre (idAttrib, 3 x float / sommets, pas de normalisation,
    // et 0, 0 : on ne saute rien et on commence au debut)
    glVertexAttribPointer(m_ShaderProgram->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //vao fini
    glBindVertexArray(0);
}



void Viewer::init()
{
    makeCurrent();
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if( glewErr != GLEW_OK )
    {
        qDebug("Error %s", glewGetErrorString(glewErr) ) ;
    }
    std::cout << "GL VERSION = " << glGetString(GL_VERSION) << std::endl;

	// la couleur de fond
	glClearColor(0.1,0.1,0.2,0.0);

	// on initialize le rendu que l'on veut faire
    tp_init();

	// LIB QGLViewer scene initialization
	setSceneRadius(3.0);
    setSceneCenter(qglviewer::Vec(0.0,0.0,0.0));
    camera()->showEntireScene();

    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void Viewer::updatePatch()
{
    m_patch->updateVBO_CP(m_vbo_id, m_ebo_id);
    m_patch->updateVBO_Bezier(m_vbo_id, m_ebo_id);
    //m_drawBezier=false; //bezier CPs have changed, thus the bezier surface isn't valide anymore

    updateGL();
}

void Viewer::setDrawingColor(const glm::vec4& color)
{
    glUniform4f(m_ShaderProgram->idOfColor, color.x, color.y, color.z, color.a);
}

void Viewer::drawPatchLines()
{
    setDrawingColor(glm::vec4(0.7, 0.7, 0.7, 0.0));

    m_patch->drawLines();
}

void Viewer::drawPatchControlPoints()
{
    setDrawingColor(glm::vec4(1.0, 0.1, 0.1, 0.0));

    m_patch->drawControlPoints();
}

void Viewer::drawPatchBezier()
{
    setDrawingColor(glm::vec4(0.3, 0.9, 0.3, 0.0));

    m_patch->drawBezier();
}

void Viewer::draw()
{
    // recupere les matrices depuis l'interface
    glm::mat4 viewMatrix = getCurrentModelViewMatrix();
    glm::mat4 projectionMatrix = getCurrentProjectionMatrix();

    if(m_ShaderProgram != NULL)
    {
        m_ShaderProgram->startUseProgram();

        // envoit les matrices au shader
        glUniformMatrix4fv(m_ShaderProgram->idOfViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(m_ShaderProgram->idOfProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        // travaille avec le vao defini (et donc les buffers associés)
        glBindVertexArray(m_Vao);

        glPointSize(4.0); // clair !!
        glLineWidth(2.0);

        drawPatchLines();

                drawPatchControlPoints();

        if(m_drawBezier)
            drawPatchBezier();

        glBindVertexArray(0);

        m_ShaderProgram->stopUseProgram();
    }
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
        case Qt::Key_R: // touche 'r'
		break;

        case Qt::Key_B:

            if(m_drawBezier=!m_drawBezier)
                m_patch->updateVBO_Bezier(m_vbo_id, m_ebo_id);
            break;

	default:
		break;
	}

	// retrace la fenetre
	updateGL();
	QGLViewer::keyPressEvent(e);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    if((m_selectedCP=m_patch->rayIntersectsCP(m_origin, m_direction, 0.1, m_distanceSelection))==NULL)
        QGLViewer::mousePressEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    m_selectedCP=NULL;

    QGLViewer::mouseReleaseEvent(e);
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    qglviewer::Vec origin, direction;
    camera()->convertClickToLine(e->pos(), origin, direction);
    m_origin=vecToGlmVec3(origin);
    m_direction=vecToGlmVec3(direction);

    m_deltaPos=e->pos()-m_oldMousePos;
    m_oldMousePos=e->pos();

    if(m_selectedCP!=NULL)
    {
 //       if(m_deltaPos.x()!=0 || m_deltaPos.y()!=0)
        {
            *m_selectedCP=m_origin+m_direction*m_distanceSelection;

            updatePatch();
        }
    }
    else
    {
        QGLViewer::mouseMoveEvent(e);
    }
}

glm::vec3 Viewer::vecToGlmVec3(const qglviewer::Vec& v)
{
    return glm::vec3(v.x, v.y, v.z);
}

qglviewer::Vec Viewer::glmVec3ToVec(const glm::vec3 &v)
{
    return qglviewer::Vec(v.x, v.y, v.z);
}

glm::mat4 Viewer::getCurrentModelViewMatrix() const
{
	GLdouble gl_mvm[16];
	camera()->getModelViewMatrix(gl_mvm);
	glm::mat4 mvm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvm[i][j] = (float)gl_mvm[i*4+j];
	}
	return mvm;
}


glm::mat4 Viewer::getCurrentProjectionMatrix() const
{
	GLdouble gl_pm[16];
	camera()->getProjectionMatrix(gl_pm);
	glm::mat4 pm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			pm[i][j] = (float)gl_pm[i*4+j];
	}
	return pm;
}
