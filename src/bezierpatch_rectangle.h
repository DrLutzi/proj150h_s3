#ifndef BEZIERPATCH_RECTANGLE_H
#define BEZIERPATCH_RECTANGLE_H

#include "bezierpatch.h"

class BezierPatch_Rectangle : public BezierPatch
{
public:
    BezierPatch_Rectangle();
    BezierPatch_Rectangle(size_t m, size_t n);
    virtual ~BezierPatch_Rectangle();

    //get

    size_t getSizeM() const;
    size_t getSizeN() const;

    const glm::vec3 &getPoint(size_t i, size_t j) const;

    //set

    void setNbPointsSide(size_t size);
    void setPoint(size_t i, size_t j, const glm::vec3& cp);

    //others

    void drawLines(GLint first, GLint baseVertex=0) const;
    void drawBezier(GLint first, GLint baseVertex=0) const;

protected:

    const glm::vec3 &casteljau(float u, float v);

    void makeVBOLines();
    void makeVBOBezierDeCasteljau();

    glm::vec3 &getTmpCasteljau(size_t i, size_t j);

private:

    size_t m_sizeM;
    size_t m_sizeN;

    std::vector<glm::vec3> m_tmpCasteljau;
};

class BezierPatch_Square : public BezierPatch_Rectangle
{
    BezierPatch_Square();
    BezierPatch_Square(size_t size);
    virtual ~BezierPatch_Square();
};

#endif // BEZIERPATCH_RECTANGLE_H
