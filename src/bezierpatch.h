#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <vector>
#include <deque>
#include <string>
#include <QString>

class BezierPatch
{
public:
    BezierPatch();
    BezierPatch(size_t sizePatch, const QString &name);
    virtual ~BezierPatch();

    //inline
    //show and hide patch or surface

    void setDrawCP(bool draw);
    void setDrawPatch(bool draw);
    void setDrawSurface(bool draw);

    void toggleDrawCP();
    void toggleDrawPatch();
    void toggleDrawSurface();

    void hideCP();
    void hidePatch();
    void hideSurface();

    void showCP();
    void showPatch();
    void showSurface();

    inline void notifyPatchChanged(){m_patchChanged=true; m_surfaceChanged=true;} //patch changed => surface changed
    inline void notifySurfaceChanged(){m_surfaceChanged=true;}

    inline bool isDrawCP() const {return m_drawCP;}
    inline bool isDrawPatch() const {return m_drawPatch;}
    inline bool isDrawSurface() const {return m_drawSurface;}

    //OpenGL indexes

    inline void setFirstVBO(GLintptr first){m_firstVBO=first;}
    inline void setFirstEBO(GLintptr first){m_firstEBO=first;}
    inline void setBaseVertexEBO(GLintptr baseVertex){m_baseVertexEBO=baseVertex;}


    inline GLintptr firstVBO() const {return m_firstVBO;}
    inline GLintptr firstEBO() const {return m_firstEBO;}
    inline GLintptr baseVertexEBO() const {return m_baseVertexEBO;}

    //Colors

    inline void setControlPointColor(const glm::vec4& color) {m_controlPointColor=color;}
    inline void setPatchColor(const glm::vec4& color) {m_patchColor=color;}
    inline void setSurfaceColor(const glm::vec4& color) {m_surfaceColor=color;}

    inline const glm::vec4& controlPointColor() const {return m_controlPointColor;}
    inline const glm::vec4& patchColor() const {return m_patchColor;}
    inline const glm::vec4& surfaceColor() const {return m_surfaceColor;}

    //Id/name
    inline unsigned int id() const {return m_id;}
    inline const QString& name() const {return m_name;}
    inline void setName(const QString& name) {m_name=name;}

    /**
     * @brief computes the size of the VBO generated for the GPU, in bytes.
     */
    inline virtual GLsizeiptr getSizeVBOPosition() const {return 3 * m_points.size() * sizeof(GLfloat);}
    inline virtual GLsizeiptr getSizeVBOSurface() const {return 3 * m_surface.size() * sizeof(GLfloat);}

    /**
     * @brief computes the size of the EBO generated for the GPU, in bytes.
     */
    inline virtual GLsizeiptr getSizeEBOPosition() const {return m_EBOPoints.size() * sizeof(unsigned int);}
    inline virtual GLsizeiptr getSizeEBOSurface() const {return m_EBOSurface.size() * sizeof(unsigned int);}

    /**
     * @brief computes the number of points generated for the GPU, as an unsigned int.
     */
    inline virtual size_t getNumberOfPointsPatch() const {return m_points.size();}
    inline virtual size_t getNumberOfPointsSurface() const {return m_surface.size();}

    //operations

    BezierPatch& operator=(const BezierPatch& other);

    /**
     * @brief setResolution sets the surface's resolution. How it is handled varies between patches.
     */
    void setResolution(size_t resolution);
    size_t resolution() const;
    void clear();

    void draw(GLint uColorLocation);

    /**
     * @brief rayIntersectsCP computes intersection between a control point and a ray
     * @param origin origin of the ray
     * @param direction direction of the ray
     * @param r maximum distance of the control point sphere hitbox
     * @param iIndex index in i
     * @param jIndex index in j
     * @param distance distance between origin and point found
     * @return a pointer to the control point intersected if one was found, otherwise a null pointer
     */
    glm::vec3* rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float &r, float &distance) const;

    /**
     * @brief makePatchEBO computes the lines of the patch.
     * Override this at the same time as drawLines and fill m_EBOPoints.
     */
    virtual void makePatchEBO()=0;

    /**
     * @brief makeSurfaceDeCasteljau computes the surface polygons (w/ the CPU) using DeCasteljau's algorithm.
     * Override this at the same time as drawBezier and fill m_bezier.
     */
    virtual void makeSurfaceVBO()=0;

    /**
     * @brief makeSurfaceDeCasteljau computes the index of the surface polygons (w/ the CPU) using DeCasteljau's algorithm.
     * Override this at the same time as drawBezier and makeSurfaceVBO, and fill m_bezier.
     */
    virtual void makeSurfaceEBO()=0;

    void updateVBOPatch(GLint vboId, GLint eboId);
    void updateVBOSurface(GLint vboId, GLint eboId);

protected:

    void setDrawingColor(const glm::vec4& color, GLint uColorLocation);

    /**
     * @brief drawPatch draws the lines of the patch. This function must be overriden.
     * Override this at the same time as makeVBOLines and use m_points and m_EBOPoints.
     * @param first index of where the GPU should start reading indexes in the VBO
     * @param baseVertex value added to every indexes, in case we draw several objects
     */
    virtual void drawPatch() const=0;

    /**
     * @brief drawSurface draws the bezier surface described by the patch.
     * If this function is not overriden, the surface will be drawn as simple points.
     * Override this at the same time as makeVBOBezierDeCasteljau and use m_bezier and m_EBOBezier.
     * @param first index of where the GPU should start reading indexes in the VBO
     * @param baseVertex value added to every indexes, in case we draw several objects
     */
    virtual void drawSurface() const;

    /**
     * @brief drawControlPoints draws the control points. You probably don't need to override this.
     * If this function is not overriden, every control point will be drawn.
     * @param first index of where the GPU should start reading indexes in the VBO
     * @param baseVertex value added to every indexes, in case we draw several objects
     */
    virtual void drawControlPoints() const;

    //iterators
    typedef std::vector<glm::vec3>::iterator iterator;
    typedef std::vector<glm::vec3>::const_iterator const_iterator;

    /**
     * @brief describes the operations for iterating over m_points.
     */
    inline iterator begin() {return m_points.begin();}
    inline const_iterator begin() const {return m_points.begin();}

    inline iterator end() {return m_points.end();}
    inline const_iterator end() const {return m_points.end();}

    unsigned int                m_id;
    QString                     m_name;

    std::vector<glm::vec3>      m_points;
    std::vector<unsigned int>   m_EBOPoints;

    std::vector<glm::vec3>      m_surface;
    std::vector<unsigned int>   m_EBOSurface;

    size_t                      m_resolution;

    bool                        m_drawCP;
    bool                        m_drawPatch;
    bool                        m_drawSurface;

    GLintptr                    m_firstVBO;            //first index of the VBO datas (shared through multiple patches)
    GLintptr                    m_firstEBO;            //first index of the EBO datas (shared through multiple patches)
    GLintptr                    m_baseVertexEBO;       //number that should be added to every index in the EBO
                                    //(maximum index of the previously filled EBO, which is also the total number of points prior to this object)

    glm::vec4                   m_controlPointColor;
    glm::vec4                   m_patchColor;
    glm::vec4                   m_surfaceColor;

    bool                        m_patchChanged;
    bool                        m_surfaceChanged;


    bool                        m_patchEBOCalculationNeeded;
    bool                        m_surfaceEBOCalculationNeeded;

    //static

    static unsigned int         ms_currentId;
};

/*

Le patch est construit selon les indices suivants :

0,0 --------> j = N-1
  |
  |
  |
  v
i = M-1

VBOData permet au GPU de construire rapidement le patch, colonnes par colonnes puis lignes par lignes.

*/

#endif // BEZIERPATCH_H
