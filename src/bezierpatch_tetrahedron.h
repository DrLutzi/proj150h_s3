#ifndef BEZIERPATCH_THETRAEDRON_H
#define BEZIERPATCH_THETRAEDRON_H

#include "bezierpatch.h"

class BezierPatch_Tetrahedron : public BezierPatch
{
public:
    BezierPatch_Tetrahedron();

    BezierPatch_Tetrahedron(size_t n);
    ~BezierPatch_Tetrahedron();

    //get

    size_t size() const;

    const glm::vec3 &getPoint(size_t i, size_t j, size_t k, size_t l) const;

    //set

    void setPoint(size_t i, size_t j, size_t k, size_t l, const glm::vec3& cp);

    void makePatchEBO();
    void makeSurfaceVBO();
    void makeSurfaceEBO();

    //operators

    BezierPatch_Tetrahedron& operator=(const BezierPatch_Tetrahedron& other);

    //static random generation

    static BezierPatch_Tetrahedron* generate(size_t n, float xStep=1.0f, float yStep=1.0f, float zStep=-1.0f, float max_noise=0.5f);

protected:

    //others

    void drawPatch() const;
    void drawSurface() const;

    const glm::vec3 &casteljau(float u, float v, float w, float t);

    glm::vec3 &getTmpCasteljau(size_t i, size_t j, size_t k, size_t l);

private:

    /**
     * @brief calculates \sum_{j=0}^{l}(n-j)(n+1-j)/2 + \sum_{i=0}^{k}((n-l)-i) + j for any (j,k,l)>(-1,0,0). n is m_size.
     * This is used to access the vector m_points.
     * a similar function is used for BezierPatch_Triangle because
     * we decided not to fully expand the expression used to access the set of control points.
     */
    inline size_t indexOf(size_t j, size_t k, size_t l) const;

    size_t m_size;

    std::vector<glm::vec3> m_tmpCasteljau;

    ///This is used as a mean to reduce the number of calculations
    size_t mc_constValue;

    /*

    The patch is constructed following this nice 3D ascii blueprint:

      (0,0,n-1,0)
          /|\
         / | \
        /  |  \
       /   |*  \      * (0,0,0,n-1) (back)
      /  _/ \_  \
     / _/     \_ \
     _/_________\_ (0,n-1,0,0)
(n-1,0,0,0)

    */
};

#endif // BEZIERPATCH_THETRAEDRON_H
