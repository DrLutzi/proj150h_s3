#include "patchdependencysolver.h"
#include "errorsHandler.hpp"
#include "bezierpatch_manager.h"

PatchDependencySolver::PatchDependencySolver(BezierPatch_Manager &mgr):
    m_sizeM(0),
    m_sizeN(0),
    m_dependencyType(NO_DEPENDENCY),
    m_manager(mgr),
    m_parentId(0),
    m_child1Id(0),
    m_child2Id(0),
    m_dependencyMatrix()
{
}

PatchDependencySolver::~PatchDependencySolver()
{

}

/**
 * @brief createR2TDependency creates a rectangle -> 2 triangles dependency relation
 * @param id id of the rectangle patch inside the manager
 */
bool PatchDependencySolver::createR2TDependency(unsigned int id)
{
    if(m_dependencyType != NO_DEPENDENCY)
        return false;   ///< a dependency was already created
    BezierPatch_Rectangle *rectanglePatch=dynamic_cast<BezierPatch_Rectangle*>(m_manager.getPatch(id));
    if(rectanglePatch==NULL)
    {
        WARNING("createR2TDependency: patch with id is not a rectangle, discarding operation");
        return false;   ///< patch with id isn't a rectangle patch
    }
    else if(rectanglePatch->sizeM()==0 || rectanglePatch->sizeN()==0)
    {
        WARNING("createR2TDependency: patch has an incompatible size, discarding operation");
        return false;  ///< the size of the patch is incompatible with the algorithm
    }
    else
    {
        m_sizeM = rectanglePatch->sizeM();
        m_sizeN = rectanglePatch->sizeN();

        //n1 and n2 are orders, and order is size-1 in both M and N for our rectangle patch
        size_t n1=m_sizeM-1;
        size_t n2=m_sizeN-1;

        //creating upper and lower triangles of order n1+n2 (size n1+n2+1)
        BezierPatch_Triangle *upperT=new BezierPatch_Triangle(n1+n2+1);
        BezierPatch_Triangle *lowerT=new BezierPatch_Triangle(n1+n2+1);

        //allocating the vector of polynomial sums to update the dependency later
        m_dependencyMatrix.resize(m_sizeM * m_sizeN);

        for(size_t j1=0; j1<m_sizeM; ++j1)
        {
            for(size_t j2=0; j2<m_sizeN; ++j2)
            {
                //get a reference to the proper polynomial sum, with a simple matrix-like access
                Sum_ProductPolynom3Var &polynomialSum = m_dependencyMatrix[indexOf(j1,j2)];

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
                //you could also simplify the polynomial expression (and yes, this is a TODO)

                //Finally, iterate over the sum to know what triangle point we are setting
                //and deduce a new term of this point with the full coefficient and the rectangle control point
                for(Sum_ProductPolynom3Var::iterator it=polynomialSum.begin(); it!=polynomialSum.end(); ++it)
                {
                    size_t i1=(*it)[ProductPolynom3Var::PP3V_A];
                    size_t i2=(*it)[ProductPolynom3Var::PP3V_B];
                    size_t i3=(*it)[ProductPolynom3Var::PP3V_C];

                    //can be optimized in time because done several times, but this function isn't called too often anyway
                    double leftCoef=boost::math::factorial<double>(n1+n2)/(boost::math::factorial<double>(i1)*boost::math::factorial<double>(i2)*boost::math::factorial<double>(i3));

                    //now we just need to multiply by the right coefficient and divide by the left coefficient given by the equation, and there we have our polynomial.
                    ((*it)*=rightCoef)/=leftCoef;
                    //update the point from t1, where its index is given by the polynom's degrees, with the upper triangle point
                    upperT->setPoint(i1, i2, i3, upperT->getPoint(i1, i2, i3) + rectanglePatch->getPoint(j1, j2)        *       (float)(*it).coef());

                    //update the point from t2, but with the opposite point for the lower triangle
                    lowerT->setPoint(i1, i2, i3, lowerT->getPoint(i1, i2, i3) + rectanglePatch->getPoint(n1-j1, n2-j2)  *       (float)(*it).coef());
                }
            }
        }

        //now that the calculation of the CPs is out of the way, set our patches' display properties
        upperT->setControlPointColor(rectanglePatch->controlPointColor());
        lowerT->setControlPointColor(rectanglePatch->controlPointColor());

        upperT->setPatchColor(rectanglePatch->patchColor());
        lowerT->setPatchColor(rectanglePatch->patchColor());

        upperT->setSurfaceColor(rectanglePatch->surfaceColor());
        lowerT->setSurfaceColor(rectanglePatch->surfaceColor());

        upperT->setDrawCP(rectanglePatch->isDrawCP());
        lowerT->setDrawCP(rectanglePatch->isDrawCP());

        upperT->setDrawPatch(rectanglePatch->isDrawPatch());
        lowerT->setDrawPatch(rectanglePatch->isDrawPatch());

        upperT->setDrawSurface(rectanglePatch->isDrawSurface());
        lowerT->setDrawSurface(rectanglePatch->isDrawSurface());

        //append to the manager but don't reallocate until both are added
        m_manager.append(upperT, false);
        m_manager.append(lowerT);

        //don't forget to set the children
        m_child1Id = upperT->id();
        m_child2Id = lowerT->id();

        //we did it
        return true;
    }
}

/**
 * @brief createH2TDependency creates a hexaedron -> 2 tetrahedrons dependency relation
 * @param id id of the hexaedron patch inside the manager
 */
bool PatchDependencySolver::createH2TDependency(unsigned int id)
{

}

/**
 * @brief updateDependency updates the child patches with the current dependency set
 */
void PatchDependencySolver::updateDependency()
{

}
