#ifndef RPATCH2TPATCHSOLVER_H
#define RPATCH2TPATCHSOLVER_H

#include "bezierpatch_rectangle.h"
#include "bezierpatch_triangle.h"
#include "productpolynom3var.h"
#include "queue"

class RPatch2TPatchSolver
{

public:
    typedef enum {Solver_UpperTriangle=0, Solver_LowerTriangle=1} trianglePart_t;

    RPatch2TPatchSolver();
    RPatch2TPatchSolver(size_t nOrder, size_t mOrder);

    BezierPatch_Triangle solveFrom(const BezierPatch_Rectangle& rectanglePatch, trianglePart_t triangle);

    /**
     * @brief sets (gets) a coefficient in the coefficient matrix representing the left side of the equation.
     * It uses degree i1, i2 and i3 of polynomial a^i1*b^i2*c^i3 to find an appropriate index in the table,
     * as well as wether we are computing the upper part or the lower part of the triangle.
     * @param trianglePart the part of the triangle we are setting a coefficient
     * @param i1 degree of a
     * @param i2 degree of b
     * @param i3 degree of c
     * @param coef
     * @precondition i1, i2 and i3 are valid values in any context according to the equation. This function doesn't perform boundaries checking.
     */
    void setCoef(trianglePart_t trianglePart, unsigned int i1, unsigned int i2, unsigned int i3, double coef);
    double coef(trianglePart_t trianglePart, unsigned int i1, unsigned int i2, unsigned int i3);

private:

    typedef glm::vec3 ControlPoint_t;

    size_t m_bezierTriangleSize;
    std::vector<double> m_tCoefMatrix;
};

#endif // RPATCH2TPATCHSOLVER_H
