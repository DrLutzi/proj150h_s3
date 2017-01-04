#include "viewer.h"
#include <glm/gtc/type_ptr.hpp>
#include <QKeyEvent>

Viewer::Viewer(QWidget *parent) :
    QGLViewer(parent),
    m_selectedCP(false),
    m_oldMousePos(0,0),
    m_deltaPos(0,0),
    m_selectionSize(0.5f),
    m_surfacePolygonMode(GL_FILL),
    m_refreshTimer(this),
    m_refreshRate(60),
    m_waitingUpdate(false)
{}

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

    // on initialise le rendu que l'on veut faire
    render_init();

	// LIB QGLViewer scene initialization
    setSceneRadius(20.0);
    setSceneCenter(qglviewer::Vec(0.0,0.0,0.0));
    camera()->showEntireScene();
}

void Viewer::render_init()
{
    srand(time(NULL));

    m_refreshTimer.setInterval((int)(float(1000)/m_refreshRate));
    connect(&m_refreshTimer, &QTimer::timeout, this, &Viewer::update);

    setMouseTracking(true);

    // SHADER
    m_ShaderProgram = new ShaderProgramBezier();

    //VBO
    glGenBuffers(1, &m_vboId);

    //EBO
    glGenBuffers(1, &m_eboId);

    // genere 1 VAO
    glGenVertexArrays(1, &m_vaoId);

    // on travaille sur celui-ci
    glBindVertexArray(m_vaoId);

    //associe l'ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);

    // associe le VBO de position
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
    // avec l'attribut position du shader
    glEnableVertexAttribArray(m_ShaderProgram->idOfVertexAttribute);
    // en donne les bon parametre (idAttrib, 3 x float / sommets, pas de normalisation,
    // et 0, 0 : on ne saute rien et on commence au debut)
    glVertexAttribPointer(m_ShaderProgram->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //vao fini
    glBindVertexArray(0);

    //display filled polygons as surface
    glPolygonMode(GL_FRONT_AND_BACK, m_surfacePolygonMode);

    m_manager = new BezierPatch_Manager(m_vaoId, m_vboId, m_eboId, m_ShaderProgram->idOfColor);
}

void Viewer::setRefreshRate(unsigned int refreshRate)
{
    refreshRate = refreshRate>0 ? refreshRate : 1;
    m_refreshTimer.setInterval((int)(float(1000)/m_refreshRate));
}

BezierPatch_Manager *Viewer::manager()
{
    return m_manager;
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
        glBindVertexArray(m_vaoId);

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
    if(!(m_selectedCP=m_manager->rayIntersectsCP(m_origin, m_direction, m_selectionSize, m_distanceSelection)))
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

    if(e->buttons() & Qt::LeftButton)
    {
        m_deltaPos=e->pos()-m_oldMousePos;
        m_oldMousePos=e->pos();

        if(m_selectedCP)
        {
            if(m_refreshTimer.isActive())
                m_waitingUpdate=true;
            else
            {
                updateSelectedCP();
                m_waitingUpdate=false;
                m_refreshTimer.start();
            }
        }
        else
            QGLViewer::mouseMoveEvent(e);
    }
    else
        QGLViewer::mouseMoveEvent(e);
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

///PRIVATE SLOTS

void Viewer::update()
{
    m_refreshTimer.stop();
    if(m_waitingUpdate && m_selectedCP)
    {
        updateSelectedCP();
    }
}

///PRIVATE

void Viewer::updateSelectedCP()
{
    m_manager->updateSelectedCP(m_origin+m_direction*m_distanceSelection);
    updateGL();
}
