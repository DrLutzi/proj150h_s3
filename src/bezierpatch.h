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
    BezierPatch(size_t sizePatch, size_t sizeVBOLines);
    ~BezierPatch();

    virtual void elevation(int degree);

    size_t getNumberOfPoints() const;

    void makeVBO(GLint vboId);
    void updateVBO_CP(GLint vboId);
    void updateVBO_Bezier(GLint vboId);

    void setResolution(size_t resolution);

    virtual void drawLines() const=0;
    virtual void drawBezier() const=0;
    virtual void drawControlPoints() const;

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
     * @brief computes the best patch lines VBO from the patch into m_VBOLines
     */
    virtual void makeVBOLines()=0;

    /**
     * @brief computes the best patch surface or curve VBO from the patch into m_VBOBezier using DeCasteljau
     * @param rate number of points generated to render polygons
     */
    virtual void makeVBOBezierDeCasteljau()=0;

    const glm::vec3* getVBOLinesData() const;
    const glm::vec3* getVBOBezierData() const;

    GLsizeiptr getSizeVBOLines_GPU() const;
    GLsizeiptr getSizeVBOBezier_GPU() const;

    std::vector<glm::vec3> m_points;
    std::vector<glm::vec3> m_tmpCasteljau;
    std::vector<glm::vec3> m_VBOLines;
    std::vector<glm::vec3> m_VBOBezier;

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
