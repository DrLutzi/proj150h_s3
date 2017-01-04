#ifndef BEZIERPATCH_HEXAEDRON_H
#define BEZIERPATCH_HEXAEDRON_H

#include "bezierpatch.h"

class BezierPatch_Hexaedron : public BezierPatch
{
public:

    typedef enum {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM} Face_t;

    BezierPatch_Hexaedron();

    BezierPatch_Hexaedron(size_t m, size_t n, size_t p);
    ~BezierPatch_Hexaedron();

    //get

    inline size_t sizeM() const{return m_sizeM;}
    inline size_t sizeN() const{return m_sizeN;}
    inline size_t sizeP() const{return m_sizeP;}

    std::pair<size_t, size_t> sizeFace(Face_t face) const;

    const glm::vec3 &getPoint(size_t i, size_t j, size_t k) const;
    const glm::vec3 &getPoint(size_t i, size_t j, Face_t face) const;

    //set

    void setPoint(size_t i, size_t j, size_t k, const glm::vec3& cp, bool sendToVBO=false);
    void setPoint(size_t i, size_t j, Face_t face, const glm::vec3& cp, bool sendToVBO=false);

    void makePatchEBO();
    void makeSurfaceVBO();
    void makeSurfaceEBO();

    //operators

    BezierPatch_Hexaedron& operator=(const BezierPatch_Hexaedron &other);

    //static random generation

    static BezierPatch_Hexaedron* generate(size_t m, size_t n, size_t p,
                                           float xStep=1.0f, float yStep=1.0f, float zStep=-1.0f, float max_noise=0.5f);


protected:

    //others

    void drawPatch() const;
    void drawSurface() const;

    const glm::vec3 &casteljau(float u, float v, float w);

    glm::vec3 &getTmpCasteljau(size_t i, size_t j, size_t k);

private:

    size_t indexOf(size_t i, size_t j, size_t k) const;
    size_t indexOf(size_t i, size_t j, Face_t face) const;

    size_t m_sizeM;
    size_t m_sizeN;
    size_t m_sizeP;

    std::vector<glm::vec3> m_tmpCasteljau;

    /*

    The patch is constructed following this nice 3D ascii blueprint:

    0,0 --------> j = N-1
      | \
      |  \
      |   \ k=P-1
      v    x
 i = M-1

    */
};

#endif // BEZIERPATCH_HEXAEDRON_H
