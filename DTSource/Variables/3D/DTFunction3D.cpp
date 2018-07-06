// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTFunction3D.h"

#include "DTFunction1D.h"
#include "DTFunction2D.h"
#include "DTDataStorage.h"
#include "DTError.h"

DTFunction3D::DTFunction3D(const DTFunction &f)
{
    // In order for this to valid, it is required to have only an x argument.
    DTList<std::string> C = f.ConstantNames();
    if (C.Length()>3) {
        DTErrorMessage("DTFunction3D(DTFunction)","Can only have three unknown arguments");
        _fcn = DTFunction();
    }
    else if ((C.Length()>0 && C(0)!="x" && C(0)!="y" && C(0)!="z") ||
             (C.Length()>1 && C(1)!="x" && C(1)!="y" && C(1)!="z") ||
             (C.Length()>2 && C(2)!="x" && C(2)!="y" && C(2)!="z")) {
        // Can be a function of, x, y or z.
        DTErrorMessage("DTFunction3D(DTFunction)","Can only depend on \"x\", \"y\" or \"z\"");
        _fcn = DTFunction();
    }
    else {
        DTMutableList<std::string> order(3);
        order(0) = "x";
        order(1) = "y";
        order(2) = "z";
        _fcn = f.ChangeConstantOrder(order);
    }
}

void DTFunction3D::pinfo(void) const
{
    _fcn.pinfo();
}

void Read(const DTDataStorage &input,const std::string &name,DTFunction3D &toReturn)
{
    DTFunction _fcn;
    Read(input,name,_fcn);
    toReturn = DTFunction3D(_fcn);
}

void Write(DTDataStorage &output,const std::string &name,const DTFunction3D &theVar)
{
    Write(output,name,theVar.Function());
}

DTFunction3D operator+(const DTFunction3D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()+B.Function());
}

DTFunction3D operator-(const DTFunction3D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()-B.Function());
}

DTFunction3D operator*(const DTFunction3D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()*B.Function());
}

DTFunction3D operator/(const DTFunction3D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()/B.Function());
}

DTFunction3D operator+(const DTFunction2D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()+B.Function());
}

DTFunction3D operator-(const DTFunction2D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()-B.Function());
}

DTFunction3D operator*(const DTFunction2D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()*B.Function());
}

DTFunction3D operator/(const DTFunction2D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()/B.Function());
}

DTFunction3D operator+(const DTFunction1D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()+B.Function());
}

DTFunction3D operator-(const DTFunction1D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()-B.Function());
}

DTFunction3D operator*(const DTFunction1D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()*B.Function());
}

DTFunction3D operator/(const DTFunction1D &A,const DTFunction3D &B)
{
    return DTFunction3D(A.Function()/B.Function());
}

DTFunction3D operator+(const DTFunction3D &A,const DTFunction1D &B)
{
    return DTFunction3D(A.Function()+B.Function());
}

DTFunction3D operator-(const DTFunction3D &A,const DTFunction1D &B)
{
    return DTFunction3D(A.Function()-B.Function());
}

DTFunction3D operator*(const DTFunction3D &A,const DTFunction1D &B)
{
    return DTFunction3D(A.Function()*B.Function());
}

DTFunction3D operator/(const DTFunction3D &A,const DTFunction1D &B)
{
    return DTFunction3D(A.Function()/B.Function());
}

DTFunction3D operator+(const DTFunction3D &A,const DTFunction2D &B)
{
    return DTFunction3D(A.Function()+B.Function());
}

DTFunction3D operator-(const DTFunction3D &A,const DTFunction2D &B)
{
    return DTFunction3D(A.Function()-B.Function());
}

DTFunction3D operator*(const DTFunction3D &A,const DTFunction2D &B)
{
    return DTFunction3D(A.Function()*B.Function());
}

DTFunction3D operator/(const DTFunction3D &A,const DTFunction2D &B)
{
    return DTFunction3D(A.Function()/B.Function());
}

DTFunction3D operator-(const DTFunction3D &A)
{
    return DTFunction3D(-A.Function());
}

DTFunction3D operator*(const DTFunction3D &A,double b)
{
    return DTFunction3D(A.Function()*b);
}

DTFunction3D operator*(double a,const DTFunction3D &B)
{
    return DTFunction3D(a*B.Function());
}

DTFunction3D operator/(const DTFunction3D &A,double b)
{
    return DTFunction3D(A.Function()/b);
}

DTFunction3D operator/(double a,const DTFunction3D &B)
{
    return DTFunction3D(a/B.Function());
}

DTFunction3D operator+(const DTFunction3D &A,double b)
{
    return DTFunction3D(A.Function()+b);
}

DTFunction3D operator+(double a,const DTFunction3D &B)
{
    return DTFunction3D(a+B.Function());
}

DTFunction3D operator-(const DTFunction3D &A,double b)
{
    return DTFunction3D(A.Function()-b);
}

DTFunction3D operator-(double a,const DTFunction3D &B)
{
    return DTFunction3D(a-B.Function());
}

DTFunction3D abs(const DTFunction3D &F)
{
    return DTFunction3D(abs(F.Function()));
}

DTFunction3D sin(const DTFunction3D &F)
{
    return DTFunction3D(sin(F.Function()));
}

DTFunction3D cos(const DTFunction3D &F)
{
    return DTFunction3D(cos(F.Function()));
}

DTFunction3D asin(const DTFunction3D &F)
{
    return DTFunction3D(asin(F.Function()));
}

DTFunction3D acos(const DTFunction3D &F)
{
    return DTFunction3D(acos(F.Function()));
}

DTFunction3D tan(const DTFunction3D &F)
{
    return DTFunction3D(tan(F.Function()));
}

DTFunction3D atan(const DTFunction3D &F)
{
    return DTFunction3D(atan(F.Function()));
}

DTFunction3D sinh(const DTFunction3D &F)
{
    return DTFunction3D(sinh(F.Function()));
}

DTFunction3D cosh(const DTFunction3D &F)
{
    return DTFunction3D(cosh(F.Function()));
}

DTFunction3D tanh(const DTFunction3D &F)
{
    return DTFunction3D(tanh(F.Function()));
}

DTFunction3D sqrt(const DTFunction3D &F)
{
    return DTFunction3D(sqrt(F.Function()));
}

DTFunction3D gamma(const DTFunction3D &F)
{
    return DTFunction3D(gamma(F.Function()));
}

DTFunction3D loggamma(const DTFunction3D &F)
{
    return DTFunction3D(loggamma(F.Function()));
}

DTFunction3D exp(const DTFunction3D &F)
{
    return DTFunction3D(exp(F.Function()));
}

DTFunction3D log(const DTFunction3D &F)
{
    return DTFunction3D(log(F.Function()));
}

DTFunction3D log10(const DTFunction3D &F)
{
    return DTFunction3D(log10(F.Function()));
}

DTFunction3D erfc(const DTFunction3D &F)
{
    return DTFunction3D(erfc(F.Function()));
}

DTFunction3D floor(const DTFunction3D &F)
{
    return DTFunction3D(floor(F.Function()));
}

DTFunction3D ceil(const DTFunction3D &F)
{
    return DTFunction3D(ceil(F.Function()));
}

DTFunction3D round(const DTFunction3D &F)
{
    return DTFunction3D(round(F.Function()));
}

DTFunction3D fact(const DTFunction3D &F)
{
    return DTFunction3D(fact(F.Function()));
}

