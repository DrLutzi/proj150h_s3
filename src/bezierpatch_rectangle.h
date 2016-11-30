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

    size_t sizeM() const;
    size_t sizeN() const;

    const glm::vec3 &getPoint(size_t i, size_t j) const;

    //set

    void setPoint(size_t i, size_t j, const glm::vec3& cp);

    void makePatch();
    void makeSurfaceDeCasteljau();

    //operators

    BezierPatch_Rectangle& operator=(const BezierPatch_Rectangle &other);

protected:

    //others

    void drawPatch() const;
    void drawSurface() const;

    const glm::vec3 &casteljau(float u, float v);

    glm::vec3 &getTmpCasteljau(size_t i, size_t j);

private:

    size_t m_sizeM;
    size_t m_sizeN;

    std::vector<glm::vec3> m_tmpCasteljau;
};

#endif // BEZIERPATCH_RECTANGLE_H
