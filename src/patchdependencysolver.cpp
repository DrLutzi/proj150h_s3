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
        m_dependencyType = R2T;

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
        copyParent(rectanglePatch, upperT, lowerT);

        //append to the manager but don't reallocate until both are added
        m_manager.append(upperT, false);
        m_manager.append(lowerT);

        //don't forget to set the children
        m_child1Id = upperT->id();
        m_child2Id = lowerT->id();
    }
    //remake the scene since we might have added stuff to it
    m_manager.remakeScene();
    return true;
}

/**
 * @brief createH2TDependency creates a hexaedron -> 2 tetrahedrons dependency relation
 * @param id id of the hexaedron patch inside the manager
 */
bool PatchDependencySolver::createH2TDependency(unsigned int id, BezierPatch_Hexaedron::Face_t face)
{
    if(m_dependencyType != NO_DEPENDENCY)
        return false;   ///< a dependency was already created
    BezierPatch_Hexaedron *hexaedronPatch=dynamic_cast<BezierPatch_Hexaedron*>(m_manager.getPatch(id));
    if(hexaedronPatch==NULL)
    {
        WARNING("createH2TDependency: patch with id is not an hexaedron, discarding operation");
        return false;   ///< patch with id isn't a rectangle patch
    }
    std::pair<size_t, size_t> sizePatch = hexaedronPatch->sizeFace(face);
    if(sizePatch.first==0 || sizePatch.second==0)
    {
        WARNING("createH2TDependency: patch has an incompatible size, discarding operation");
        return false;  ///< the size of the patch is incompatible with the algorithm
    }
    else
    {
        m_dependencyType = H2T;

        m_sizeM = sizePatch.first;
        m_sizeN = sizePatch.second;

        //n1 and n2 are orders, and order is size-1 in both M and N for our rectangle patch
        size_t n1=m_sizeM-1;
        size_t n2=m_sizeN-1;

        //creating upper and lower triangles of order n1+n2 (size n1+n2+1)
        BezierPatch_Tetrahedron *upperT=new BezierPatch_Tetrahedron(n1+n2+1);
        BezierPatch_Tetrahedron *lowerT=new BezierPatch_Tetrahedron(n1+n2+1);

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
                    upperT->setPoint(i1, i2, 0, i3, upperT->getPoint(i1, i2, 0, i3) + hexaedronPatch->getPoint(j1, j2, face)        * (float)(*it).coef());

                    //update the point from t2, but with the opposite point for the lower triangle
                    lowerT->setPoint(i1, i2, 0, i3, lowerT->getPoint(i1, i2, 0, i3) + hexaedronPatch->getPoint(n1-j1, n2-j2, face)  * (float)(*it).coef());
                }
            }
        }
        upperT->raising();
        lowerT->raising();

        //now that the calculation of the CPs is out of the way, set our patches' display properties
        copyParent(hexaedronPatch, upperT, lowerT);

        //append to the manager but don't reallocate until both are added
        m_manager.append(upperT, false);
        m_manager.append(lowerT);

        //don't forget to set the children
        m_child1Id = upperT->id();
        m_child2Id = lowerT->id();

        //add the additional informations
        m_parentPatchInfos = face;
    }
    m_manager.remakeScene();
    return true;
}

/**
 * @brief updateDependency updates the child patches with the current dependency set
 */
void PatchDependencySolver::updateDependency()
{
    if(m_dependencyType!=NO_DEPENDENCY)
    {
        //reset the childs' patch
        BezierPatch *p_c1 = m_manager.getPatch(m_child1Id), *p_c2=m_manager.getPatch(m_child2Id);
        if(p_c1!=NULL)
            p_c1->clear();
        if(p_c2!=NULL)
            p_c2->clear();
        else if(p_c1==NULL)
        {
            WARNING("updateDependency: both child weren't found, no dependency updated");
            return;
        }

        union {BezierPatch_Rectangle *rParent; BezierPatch_Hexaedron *hParent;} parent;
        typedef union {BezierPatch_Triangle *tChild; BezierPatch_Tetrahedron *ttChild;} Child_u;
        Child_u child1, child2;

        if(m_dependencyType == R2T)
        {
            if( (parent.rParent=dynamic_cast<BezierPatch_Rectangle*>(m_manager.getPatch(m_parentId))) == NULL)
            {
                WARNING("updateDependency: rectangle parent couldn't be found");
                return;
            }
            child1.tChild = dynamic_cast<BezierPatch_Triangle*>(m_manager.getPatch(m_child1Id));
            child2.tChild = dynamic_cast<BezierPatch_Triangle*>(m_manager.getPatch(m_child2Id));
        }
        else //H2T
        {
            if( (parent.hParent=dynamic_cast<BezierPatch_Hexaedron*>(m_manager.getPatch(m_parentId))) == NULL)
            {
                WARNING("updateDependency: hexaedron parent couldn't be found");
                return;
            }
            child1.ttChild = dynamic_cast<BezierPatch_Tetrahedron*>(m_manager.getPatch(m_child1Id));
            child2.ttChild = dynamic_cast<BezierPatch_Tetrahedron*>(m_manager.getPatch(m_child2Id));
        }

        size_t n1=m_sizeM-1;
        size_t n2=m_sizeN-1;
        for(size_t j1=0; j1<m_sizeM; ++j1)
        {
            for(size_t j2=0; j2<m_sizeN; ++j2)
            {
                //get a reference to the proper polynomial sum, with a simple matrix-like access
                Sum_ProductPolynom3Var &polynomialSum = m_dependencyMatrix[indexOf(j1,j2)];

                //Set every point according to its polynom
                for(Sum_ProductPolynom3Var::iterator it=polynomialSum.begin(); it!=polynomialSum.end(); ++it)
                {
                    size_t i1=(*it)[ProductPolynom3Var::PP3V_A];
                    size_t i2=(*it)[ProductPolynom3Var::PP3V_B];
                    size_t i3=(*it)[ProductPolynom3Var::PP3V_C];

                    if(m_dependencyType==R2T)
                    {
                        //update the point from t1, where its index is given by the polynom's degrees, with the upper triangle point
                        child1.tChild->setPoint(i1, i2, i3, child1.tChild->getPoint(i1, i2, i3) + parent.rParent->getPoint(j1, j2)        *       (float)(*it).coef());

                        //update the point from t2, but with the opposite point for the lower triangle
                        child2.tChild->setPoint(i1, i2, i3, child2.tChild->getPoint(i1, i2, i3) + parent.rParent->getPoint(n1-j1, n2-j2)  *       (float)(*it).coef());
                    }
                    else //H2T
                    {
                        //update the point from t1, where its index is given by the polynom's degrees, with the upper triangle point
                        child1.ttChild->setPoint(i1, i2, 0, i3, child1.ttChild->getPoint(i1, i2, 0, i3) + parent.hParent->getPoint(j1, j2, m_parentPatchInfos)       * (float)(*it).coef());

                        //update the point from t2, but with the opposite point for the lower triangle
                        child2.ttChild->setPoint(i1, i2, 0, i3, child2.ttChild->getPoint(i1, i2, 0, i3) + parent.hParent->getPoint(n1-j1, n2-j2, m_parentPatchInfos) * (float)(*it).coef());
                    }
                }
            }
        }
        if(m_dependencyType==H2T)
        {
            child1.ttChild->raising();
            child2.ttChild->raising();
        }
    }
    m_manager.updateScene();
    return;
}

/**
 * @brief copies some of the content of parent into ch1 and ch2.
 */
void PatchDependencySolver::copyParent(BezierPatch *parent, BezierPatch *ch1, BezierPatch *ch2)
{
    ch1->setControlPointColor(parent->controlPointColor());
    ch2->setControlPointColor(parent->controlPointColor());

    ch1->setPatchColor(parent->patchColor());
    ch2->setPatchColor(parent->patchColor());

    ch1->setSurfaceColor(parent->surfaceColor());
    ch2->setSurfaceColor(parent->surfaceColor());

    ch1->setDrawCP(parent->isDrawCP());
    ch2->setDrawCP(parent->isDrawCP());

    ch1->setDrawPatch(parent->isDrawPatch());
    ch2->setDrawPatch(parent->isDrawPatch());

    ch1->setResolution(parent->resolution());
    ch2->setResolution(parent->resolution());

    ch1->setDrawSurface(parent->isDrawSurface());
    ch2->setDrawSurface(parent->isDrawSurface());

    return;
}
