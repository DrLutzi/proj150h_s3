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
    m_numberOfVertices = 4;
    m_rectangularPatch = new BezierPatch_Rectangle(2,3);

    m_rectangularPatch->setPoint(0,0, glm::vec3(-1,-1,-0.5));
    m_rectangularPatch->setPoint(0,1, glm::vec3(-1,1,-0.1));
    m_rectangularPatch->setPoint(0,2, glm::vec3(-1,1.8,0.2));


    m_rectangularPatch->setPoint(1,0, glm::vec3(1,-1,0.5));
    m_rectangularPatch->setPoint(1,1, glm::vec3(1,1,0));
    m_rectangularPatch->setPoint(1,2, glm::vec3(1,2,-0.2));



    // SHADER
    m_ShaderProgram = new ShaderProgramBezier();

    //VBO
    glGenBuffers(1, &m_vbo_id);
    // buffer courant a manipuler
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    // definit la taille du buffer et le remplit
    glBufferData(GL_ARRAY_BUFFER, 3 * m_rectangularPatch->getVBOSize() * sizeof(GLfloat), m_rectangularPatch->getVBOData(), GL_DYNAMIC_DRAW);
    // buffer courant : rien
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // genere 1 VAO
    glGenVertexArrays(1, &m_Vao);
    // on travaille sur celui-ci
    glBindVertexArray(m_Vao);
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

}

void Viewer::drawPatch_Rectangle()
{
    glUniform4f(m_ShaderProgram->idOfColor, 0.7, 0.7, 0.7, 0.0);

    for(int i=0; i<m_rectangularPatch->getNumberOfPoints(); i+=m_rectangularPatch->getSizeN())
        glDrawArrays(GL_LINE_STRIP, i, m_rectangularPatch->getSizeN());

    for(int j=0; j<m_rectangularPatch->getNumberOfPoints(); j+=m_rectangularPatch->getSizeM())
        glDrawArrays(GL_LINE_STRIP, m_rectangularPatch->getNumberOfPoints()+j, m_rectangularPatch->getSizeM());
}

void Viewer::drawPatch_Triangle()
{
    glUniform4f(m_ShaderProgram->idOfColor, 0.9, 0.9, 0.9, 0.0);

    int jump=0;
    for(int i=0; i<3; ++i) //dessin des trois passes
    {
        for(int passe=m_triangularPatch->getSize(); passe>1; --passe)
        {
            glDrawArrays(GL_LINE_STRIP, jump, passe);
            jump+=passe;
        }
    }
}

void Viewer::drawPatchControlPoints()
{
    glUniform4f(m_ShaderProgram->idOfColor, 1.0, 0.1, 0.1, 0.0);

    glDrawArrays(GL_POINTS, 0, m_rectangularPatch->getNumberOfPoints());
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

        drawPatch_Rectangle();

        drawPatchControlPoints();

        glBindVertexArray(0);

        m_ShaderProgram->stopUseProgram();
    }
}

void Viewer::updatePatch()
{
    // buffer courant a manipuler
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    // definit la taille du buffer et le remplit
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat) * m_rectangularPatch->getVBOSize(), m_rectangularPatch->getVBOData(), GL_DYNAMIC_DRAW);
    // buffer courant : rien
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void Viewer::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
        case Qt::Key_R: // touche 'r'
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
    if((m_selectedCP=m_rectangularPatch->rayIntersectsCP(m_origin, m_direction, 0.6, m_distanceSelection))==NULL)
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

    if(m_selectedCP!=NULL && m_deltaPos.x()!=0 && m_deltaPos.y()!=0)
    {
        *m_selectedCP=m_origin+m_direction*m_distanceSelection;

        updatePatch();
    }
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