#include "viewer.h"
#include <glm/gtc/type_ptr.hpp>
#include <QKeyEvent>

Viewer::Viewer(QWidget *parent) :
    QGLViewer(parent)
{}

void Viewer::tp_init()
{
    m_oldMousePos=QPoint(0,0);
    m_deltaPos=QPoint(0,0);
    m_selectedCP=false;
    m_drawSurfaces=false;
    m_surfacePolygonMode=GL_LINE;
    srand(time(NULL));

    setMouseTracking(true);

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

    m_manager = new BezierPatch_Manager(m_Vao, m_vbo_id, m_ebo_id, m_ShaderProgram->idOfColor);

    m_manager->setRefreshRate(60);
}

Viewer::~Viewer()
{
    delete m_manager;
    delete m_ShaderProgram;
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
    setSceneRadius(20.0);
    setSceneCenter(qglviewer::Vec(0.0,0.0,0.0));
    camera()->showEntireScene();

    glPolygonMode(GL_FRONT_AND_BACK,m_surfacePolygonMode);
}

BezierPatch_Manager *Viewer::manager()
{
    return m_manager;
}

void Viewer::rectangularPatch2UpperTrianglePatch()
{
    BezierPatch_Triangle *upperT = new BezierPatch_Triangle;
    BezierPatch_Triangle *lowerT = new BezierPatch_Triangle;

    RPatch2TPatchSolver solver;

/*    (*upperT)=solver.solveFrom((*m_examplePatch), RPatch2TPatchSolver::Solver_LowerTriangle);
    (*lowerT)=solver.solveFrom((*m_examplePatch), RPatch2TPatchSolver::Solver_UpperTriangle); */

    upperT->setResolution(40);
    upperT->showPatch();
    upperT->setPatchColor(glm::vec4(0.7, 0.5, 0.5, 1.0));
    upperT->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));
    upperT->setSurfaceColor(glm::vec4(0.7, 0.2, 0.2, 1.0));

    lowerT->setResolution(40);
    lowerT->showPatch();
    lowerT->setPatchColor(glm::vec4(0.5, 0.7, 0.5, 1.0));
    lowerT->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));
    lowerT->setSurfaceColor(glm::vec4(0.2, 0.7, 0.2, 1.0));

    m_manager->append(upperT, false);
    m_manager->append(lowerT, true);
    return;
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

        m_manager->drawScene();

        glBindVertexArray(0);

        m_ShaderProgram->stopUseProgram();
    }
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
	updateGL();
	QGLViewer::keyPressEvent(e);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    float r=0.5;

    if(!(m_selectedCP=m_manager->rayIntersectsCP(m_origin, m_direction, r, m_distanceSelection)))
        QGLViewer::mousePressEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    m_selectedCP=false;
    m_manager->releaseSelectedCP();

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

    if(m_selectedCP)
    {
        m_manager->updateSelectedCP(m_origin+m_direction*m_distanceSelection);
        m_manager->updateScene();
        updateGL();
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
