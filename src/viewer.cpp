#include "viewer.h"
#include <glm/gtc/type_ptr.hpp>
#include <QKeyEvent>

void Viewer::tp_init()
{
    m_oldMousePos=QPoint(0,0);
    m_deltaPos=QPoint(0,0);
    m_selectedCP=false;
    m_drawSurfaces=false;
    m_surfacePolygonMode=GL_LINE;
    srand(time(NULL));

    setMouseTracking(true);
    // genere les donnees a afficher

    m_rectangularPatch = new BezierPatch_Rectangle(2,3);
    m_rectangularPatch->setResolution(10);

    m_rectangularPatch->setPoint(0,0, glm::vec3(-1,-1,-0.5));
    m_rectangularPatch->setPoint(0,1, glm::vec3(-1,1,-0.1));
    m_rectangularPatch->setPoint(0,2, glm::vec3(-1,1.8,0.2));

    m_rectangularPatch->setPoint(1,0, glm::vec3(1,-1,0.5));
    m_rectangularPatch->setPoint(1,1, glm::vec3(1,1,0));
    m_rectangularPatch->setPoint(1,2, glm::vec3(1,2,-0.2));

    m_rectangularPatch->setSurfaceColor(glm::vec4(0.2, 0.9, 0.2, 1.0));
    m_rectangularPatch->setPatchColor(glm::vec4(0.5, 0.7, 0.5, 1.0));
    m_rectangularPatch->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));

    m_rectangularPatch->hidePatch();


    m_examplePatch = new BezierPatch_Rectangle(4,4);
    m_examplePatch->setResolution(40);

    m_examplePatch->setPoint(0,0, glm::vec3(-1,0, 1.3));
    m_examplePatch->setPoint(0,1, glm::vec3(-1.2,1.2, 1));
    m_examplePatch->setPoint(0,2, glm::vec3(-0.5,1.5, 1.2));
    m_examplePatch->setPoint(0,3, glm::vec3(-0.1, 2, 1.5));

    m_examplePatch->setPoint(1,0, glm::vec3(-0.8,-0.1, 1));
    m_examplePatch->setPoint(1,1, glm::vec3(-0.6,1, 1));
    m_examplePatch->setPoint(1,2, glm::vec3(-0.2,1.4, 1.3));
    m_examplePatch->setPoint(1,3, glm::vec3(0.3, 2, 1.5));

    m_examplePatch->setPoint(2,0, glm::vec3(0.9,-0.3, 1));
    m_examplePatch->setPoint(2,1, glm::vec3(1,0.5, 1.2));
    m_examplePatch->setPoint(2,2, glm::vec3(1.2,1.2, 1.4));
    m_examplePatch->setPoint(2,3, glm::vec3(1.4, 1.7, 1.6));

    m_examplePatch->setPoint(3,0, glm::vec3(2.1,0.6, 0.9));
    m_examplePatch->setPoint(3,1, glm::vec3(2.1,1, 1.1));
    m_examplePatch->setPoint(3,2, glm::vec3(2.2,1.7, 1.3));
    m_examplePatch->setPoint(3,3, glm::vec3(2.3, 1.9, 1.5));

    m_examplePatch->setSurfaceColor(glm::vec4(0.9, 0.9, 0.3, 1.0));
    m_examplePatch->setPatchColor(glm::vec4(0.8, 0.8, 0.8, 1.0));
    m_examplePatch->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));

    m_examplePatch->hidePatch();


    //////////////////////////////////

    m_triangularPatch = new BezierPatch_Triangle(3);
    m_triangularPatch->setResolution(10);

    m_triangularPatch->setPoint(0,0,2, glm::vec3(3,1,-0.1));

    m_triangularPatch->setPoint(1,0,1, glm::vec3(2.5,0,0.1));
    m_triangularPatch->setPoint(0,1,1, glm::vec3(3.5,0,-0.2));

    m_triangularPatch->setPoint(2,0,0, glm::vec3(2,-1,0));
    m_triangularPatch->setPoint(1,1,0, glm::vec3(3,-1,0));
    m_triangularPatch->setPoint(0,2,0, glm::vec3(4,-1,0));

    m_triangularPatch->setSurfaceColor(glm::vec4(0.7, 0.2, 0.3, 1.0));
    m_triangularPatch->setPatchColor(glm::vec4(0.7, 0.5, 0.7, 1.0));
    m_triangularPatch->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));

    m_triangularPatch->hidePatch();

    //////////////////////////////////

    generateBezierThetraedron(7);
    m_tetrahedronPatch->showPatch();
    m_tetrahedronPatch->setResolution(40);

    m_tetrahedronPatch->setSurfaceColor(glm::vec4(0.9, 0.2, 0.4, 1.0));
    m_tetrahedronPatch->setPatchColor(glm::vec4(0.9, 0.5, 0.7, 1.0));
    m_tetrahedronPatch->setControlPointColor(glm::vec4(0.9, 0.2, 0.2, 1.0));


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
    m_manager->append(m_rectangularPatch, false);
    m_manager->append(m_triangularPatch, false);
    m_manager->append(m_examplePatch, false);
    m_manager->append(m_tetrahedronPatch);
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

void Viewer::rectangularPatch2UpperTrianglePatch()
{
    BezierPatch_Triangle *upperT = new BezierPatch_Triangle;
    BezierPatch_Triangle *lowerT = new BezierPatch_Triangle;

    RPatch2TPatchSolver solver;

    (*upperT)=solver.solveFrom((*m_examplePatch), RPatch2TPatchSolver::Solver_LowerTriangle);
    (*lowerT)=solver.solveFrom((*m_examplePatch), RPatch2TPatchSolver::Solver_UpperTriangle);

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

void Viewer::generateBezierThetraedron(size_t n, float xStep, float yStep, float zStep, float max_noise)
{
    float noise;
    m_tetrahedronPatch = new BezierPatch_Tetrahedron(n);
    glm::vec3 currentCP(0,0,0);
    size_t i,j,k,l;
    for(k=0; k<n; ++k)
    {
        noise=(float((rand()%2000)-1000)/1000)*max_noise;
        currentCP.z=k*zStep/2+noise;
        for(l=0; l<n-k; ++l)
        {
            noise=(float((rand()%2000)-1000)/1000)*max_noise;
            currentCP.x=(l+k)*xStep/2+noise;
            for(j=0; j<n-k-l; ++j)
            {
                i=n-k-l-1;
                m_tetrahedronPatch->setPoint(i,j,k,l, currentCP);
                noise=(float((rand()%2000)-1000)/1000)*max_noise;
                currentCP.x+=xStep+noise;
                currentCP.y+=noise;
                currentCP.z+=noise;
            }
            noise=(float((rand()%2000)-1000)/1000)*max_noise;
            currentCP.z+=zStep+noise;
        }
        noise=(float((rand()%2000)-1000)/1000)*max_noise;
        currentCP.y+=yStep+noise;
    }
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
	switch(e->key())
	{
        case Qt::Key_R: // touche 'r'
		break;

        case Qt::Key_B:

            m_drawSurfaces=!m_drawSurfaces;
            /*for(BezierPatch_Manager::iterator it=m_manager->begin(); it!=m_manager->end(); ++it)
            {
                BezierPatch *patch=(*it);
                patch->setDrawSurface(m_drawSurfaces);
            }
            m_rectangularPatch->hideSurface();
            m_examplePatch->hideSurface();*/
            m_tetrahedronPatch->setDrawSurface(m_drawSurfaces);
            m_manager->remakeScene();
            break;

        case Qt::Key_D:

            rectangularPatch2UpperTrianglePatch();
            break;

        case Qt::Key_H:

            m_examplePatch->toggleDrawPatch();
            m_examplePatch->hideSurface();

        default:
            break;
	}

	// retrace la fenetre
	updateGL();
	QGLViewer::keyPressEvent(e);
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    float r=0.1;
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
