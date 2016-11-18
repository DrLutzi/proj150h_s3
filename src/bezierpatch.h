#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <vector>

#include <QDebug>

class BezierPatch
{
public:
    BezierPatch();
    BezierPatch(size_t sizePatch, size_t sizeEBOPoints);
    ~BezierPatch();

    size_t getNumberOfPoints() const;

    void makeVBO(GLint vboId, GLint eboId);
    void updateVBO_CP(GLint vboId, GLint eboId);
    void updateVBO_Bezier(GLint vboId, GLint eboId);

    virtual void setResolution(size_t resolution);

    virtual void drawLines(GLint first=0, GLint baseVertex=0) const=0;
    virtual void drawBezier(GLint first=0, GLint baseVertex=0) const;
    virtual void drawControlPoints(GLint first=0, GLint baseVertex=0) const;

    /**
     * @brief compute intersection between a control point and a ray
     * @param origin origin of the ray
     * @param direction direction of the ray
     * @param r maximum distance of the control point sphere hitbox
     * @param iIndex index in i
     * @param jIndex index in j
     * @param distance distance between origin and point found
     * @return true if intersection was computed, otherwise false.
     */
    glm::vec3* rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, float r, float &distance) const;

protected:

    /**
     * @brief computes the best patch lines VBO from the patch using an EBO as well.
     */
    virtual void makeVBOLines()=0;

    /**
     * @brief computes the best patch surface or curve VBO from the patch using an EBO as well.
     * @param rate number of points generated to render polygons
     */
    virtual void makeVBOBezierDeCasteljau()=0;

    GLsizeiptr getSizeVBOPoints_GPU() const;
    GLsizeiptr getSizeVBOBezier_GPU() const;

    GLsizeiptr getSizeEBOPoints_GPU() const;
    GLsizeiptr getSizeEBOBezier_GPU() const;

    std::vector<glm::vec3> m_points;
    std::vector<unsigned int> m_EBOPoints;

    std::vector<glm::vec3> m_bezier;
    std::vector<unsigned int> m_EBOBezier;

    size_t m_resolution;
    bool m_resolutionChanged;
};

/*

Le patch est construit selon les indices suivants :

0,0 --------> N-1, j
  |
  |
  |
  v
M-1, i

VBOData permet au GPU de construire rapidement le patch, colonnes par colonnes puis lignes par lignes.

*/

#endif // BEZIERPATCH_H
