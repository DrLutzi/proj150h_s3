#include "rpatch2tpatchsolver.h"

RPatch2TPatchSolver::RPatch2TPatchSolver()
{
}

RPatch2TPatchSolver::RPatch2TPatchSolver(size_t nOrder, size_t mOrder) :
    m_bezierTriangleSize(nOrder+mOrder+1),
    m_tCoefMatrix(m_bezierTriangleSize*m_bezierTriangleSize)
{
}

BezierPatch_Triangle RPatch2TPatchSolver::solveFrom(const BezierPatch_Rectangle& rectanglePatch, trianglePart_t triangle)
{
    BezierPatch_Triangle trianglePatch(m_bezierTriangleSize > 0 ? rectanglePatch.sizeM()+rectanglePatch.sizeN()-1 : 0);

    //declaring our resources to solve the equation
    ControlPoint_t cp;
    Sum_ProductPolynom3Var polynomialSum;

    //here develop the left side equation
    for(unsigned int i3=0; i3<m_bezierTriangleSize; ++i3)
    {
        for(unsigned int i2=0; i2<m_bezierTriangleSize-i3; ++i2)
        {
            unsigned int i1=m_bezierTriangleSize-i2-i3-1;

            //coefficient calculation : (n1+n2)!/i1!i2!i3!
            //note that order n1+n2 is triangleSize-1, because we chose to code based on the number of CPs rather than orders
            double leftCoef=boost::math::factorial<double>(m_bezierTriangleSize-1)/(boost::math::factorial<double>(i1)*boost::math::factorial<double>(i2)*boost::math::factorial<double>(i3));

            setCoef(triangle, i1, i2, i3, leftCoef);
        }
    }

    //n1 and n2 are orders, and order is size-1 in both M and N for our rectangle patch
    size_t n1=rectanglePatch.sizeM()-1;
    size_t n2=rectanglePatch.sizeN()-1;

    //here develop the right side equation, and complete the left side with points and coefficients
    for(size_t j1=0; j1<rectanglePatch.sizeM(); ++j1)
    {
        for(size_t j2=0; j2<rectanglePatch.sizeN(); ++j2)
        {
            if(triangle==Solver_UpperTriangle)
                cp=rectanglePatch.getPoint(j1, j2);
            else
                cp=rectanglePatch.getPoint(n1-j1, n2-j2);

            //coefficient calculation : binomial coefficient of n1, j1, multiplied by the one of n2, j2

            double rightCoef=boost::math::binomial_coefficient<double>(n1, j1) * boost::math::binomial_coefficient<double>(n2, j2);

            //The hardest part : we need to expand the right equation part which involves polynomials and remarkable identities
            //this is done through a very specific polynomial simulation written in productpolynom3var.cpp

            //(b+c)^j1
            polynomialSum=Sum_ProductPolynom3Var::remarkable_identity_plus(ProductPolynom3Var::PP3V_B, ProductPolynom3Var::PP3V_C, j1);
            //(b+c)^(j1)*a^(n1-j1)*b^(n2-j2)
            polynomialSum*=ProductPolynom3Var(1, n1-j1, n2-j2, 0);
            //(b+c)^(j1)*a^(n1-j1)*(a+c)^(j2)*b^(n2-j2)
            polynomialSum*=Sum_ProductPolynom3Var::remarkable_identity_plus(ProductPolynom3Var::PP3V_A, ProductPolynom3Var::PP3V_C, j2);

            //Finally, iterate over the sum to know what triangle point we are setting
            //and deduce a new term of this point with the full coefficient and the rectangle control point
            for(Sum_ProductPolynom3Var::iterator it=polynomialSum.begin(); it!=polynomialSum.end(); ++it)
            {
                size_t i1=(*it)[ProductPolynom3Var::PP3V_A];
                size_t i2=(*it)[ProductPolynom3Var::PP3V_B];
                size_t i3=(*it)[ProductPolynom3Var::PP3V_C];
                ((*it)*=rightCoef)/=coef(triangle, i1, i2, i3);
                trianglePatch.setPoint(i1, i2, i3, trianglePatch.getPoint(i1, i2, i3)+cp*(float)(*it).coef());
            }
        }
    }

    //simply return the patch
    return trianglePatch;
}

void RPatch2TPatchSolver::setCoef(trianglePart_t trianglePart, unsigned int i1, unsigned int i2, unsigned int i3, double coef)
{
    m_tCoefMatrix[trianglePart==Solver_UpperTriangle ? i1*m_bezierTriangleSize+i3 : (m_bezierTriangleSize*m_bezierTriangleSize)-(i3*m_bezierTriangleSize+i1)] = coef;
}

double RPatch2TPatchSolver::coef(trianglePart_t trianglePart, unsigned int i1, unsigned int i2, unsigned int i3)
{
    return m_tCoefMatrix[trianglePart==Solver_UpperTriangle ? i1*m_bezierTriangleSize+i3 : (m_bezierTriangleSize*m_bezierTriangleSize)-(i3*m_bezierTriangleSize+i1)];
}
