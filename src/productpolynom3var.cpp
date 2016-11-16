#include "productpolynom3var.h"

ProductPolynom3Var::ProductPolynom3Var()
{
}

ProductPolynom3Var::ProductPolynom3Var(double coef, unsigned int dA, unsigned int dB, unsigned int dC) : m_coef(coef)
{
    m_tab[PP3V_A]=dA;
    m_tab[PP3V_B]=dB;
    m_tab[PP3V_C]=dC;
}

double ProductPolynom3Var::coef() const
{
    return m_coef;
}

unsigned int ProductPolynom3Var::degree(var_t var) const
{
    return m_tab[var];
}

unsigned int ProductPolynom3Var::operator[](var_t var) const
{
    return m_tab[var];
}

void ProductPolynom3Var::setDegree(var_t var, unsigned int degree)
{
    m_tab[var]=degree;
}

ProductPolynom3Var ProductPolynom3Var::operator *(const ProductPolynom3Var& other) const
{
    return ProductPolynom3Var(m_coef*other.coef(), degree(PP3V_A)+other.degree(PP3V_A), degree(PP3V_B)+other.degree(PP3V_B), degree(PP3V_C)+other.degree(PP3V_C));
}


////////////////////////////////////////////////////////////////////////////////////////////////////

Sum_ProductPolynom3Var::Sum_ProductPolynom3Var(size_t polynomSize)
{
    m_expression.reserve(polynomSize);
}

Sum_ProductPolynom3Var::iterator Sum_ProductPolynom3Var::begin()
{
    return m_expression.begin();
}

Sum_ProductPolynom3Var::const_iterator Sum_ProductPolynom3Var::begin() const
{
    return m_expression.begin();
}

Sum_ProductPolynom3Var::iterator Sum_ProductPolynom3Var::end()
{
    return m_expression.end();
}

Sum_ProductPolynom3Var::const_iterator Sum_ProductPolynom3Var::end() const
{
    return m_expression.end();
}

void Sum_ProductPolynom3Var::append(const ProductPolynom3Var& product)
{
    m_expression.push_back(product);
}

Sum_ProductPolynom3Var Sum_ProductPolynom3Var::operator*(const Sum_ProductPolynom3Var& other) const
{
    Sum_ProductPolynom3Var result(size()*other.size());
    for(const_iterator it=begin(); it!=end(); ++it)
    {
        for(const_iterator itOther=other.begin(); itOther!=other.end(); ++itOther)
        {
            result.append((*it)*(*itOther));
        }
    }
    return result;
}


Sum_ProductPolynom3Var Sum_ProductPolynom3Var::operator*(const ProductPolynom3Var& other) const
{
    Sum_ProductPolynom3Var result(size());
    for(const_iterator it=begin(); it!=end(); ++it)
    {
        result.append((*it)*other);
    }
    return result;
}

Sum_ProductPolynom3Var& Sum_ProductPolynom3Var::operator*=(const ProductPolynom3Var& other)
{
    for(iterator it=begin(); it!=end(); ++it)
    {
        (*it)=(*it)*other;
    }
    return (*this);
}

Sum_ProductPolynom3Var Sum_ProductPolynom3Var::remarkable_identity_plus(ProductPolynom3Var::var_t first, ProductPolynom3Var::var_t second, unsigned int n)
{
    Sum_ProductPolynom3Var result(n+1);
    for(unsigned int k=0; k<result.size(); ++k)
    {
        double binomialCoef=boost::math::binomial_coefficient<double>(n, k);
        ProductPolynom3Var nextTerm(binomialCoef, 0, 0, 0);
        nextTerm.setDegree(first, n-k);
        nextTerm.setDegree(second, k);
        result.append(nextTerm);
    }
    return result;
}

size_t Sum_ProductPolynom3Var::size() const
{
    return m_expression.size();
}
