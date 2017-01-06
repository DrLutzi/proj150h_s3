#ifndef BEZIERPATCH_RECTANGLE_H
#define BEZIERPATCH_RECTANGLE_H

#include "bezierpatch.h"

class BezierPatch_Rectangle : public BezierPatch
{
public:
    BezierPatch_Rectangle();
    BezierPatch_Rectangle(size_t m, size_t n);
    ~BezierPatch_Rectangle();

    //get

    size_t sizeM() const;
    size_t sizeN() const;

    const glm::vec3 &getPoint(size_t i, size_t j) const;

    //set

    void setPoint(size_t i, size_t j, const glm::vec3& cp, bool sendToVBO=false);

    void makePatchEBO();
    void makeSurfaceVBO();
    void makeSurfaceEBO();

    //operators

    BezierPatch_Rectangle& operator=(const BezierPatch_Rectangle &other);

    //static random generation

    static BezierPatch_Rectangle* generate(size_t m, size_t n, float xStep=1.0f, float yStep=1.0f, float max_noise=0.5f);

    void rayIntersectsCP(const glm::vec3& origin, const glm::vec3& direction, RayHit& hitProperties);

protected:

    //others

    void drawPatch() const;
    void drawSurface() const;

    const glm::vec3 &casteljau(float u, float v);

    glm::vec3 &getTmpCasteljau(size_t i, size_t j);

private:

    size_t indexOf(size_t i, size_t j) const;

    size_t m_sizeM;
    size_t m_sizeN;

    std::vector<glm::vec3> m_tmpCasteljau;

    /*

    The patch is constructed following this nice ascii blueprint:

    0,0 --------> j = N-1
      |
      |
      |
      v
    i = M-1

    */
};

#endif // BEZIERPATCH_RECTANGLE_H
