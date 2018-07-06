// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTFunction3D_H
#define DTFunction3D_H

#include "DTFunction.h"

class DTDataStorage;
class DTFunction1D;
class DTFunction2D;

class DTFunction3D {
public:
    DTFunction3D() : _fcn(DTFunction::Value(0)) {}
    DTFunction3D(const DTFunction &);
    
    double operator()(double xv,double yv,double zv) const {double v[3]; v[0] = xv; v[1] = yv; v[2] = zv; return _fcn.Evaluate(v);}
    
    void pinfo(void) const;
    std::string StringVersion(void) const {return _fcn.StringVersion();}
    
    static DTFunction3D x(void) {return DTFunction3D(DTFunction::Constant("x"));}
    static DTFunction3D y(void) {return DTFunction3D(DTFunction::Constant("y"));}
    static DTFunction3D z(void) {return DTFunction3D(DTFunction::Constant("z"));}
    static DTFunction3D one(void) {return DTFunction3D(DTFunction::Value(1.0));}
    static DTFunction3D Value(double v) {return DTFunction3D(DTFunction::Value(v));}
    
    DTFunction Function(void) const {return _fcn;}

private:
    DTFunction _fcn;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTFunction3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTFunction3D &theVar);

extern std::ostream &operator<<(std::ostream &,const DTFunction3D &);


// Operators 
extern DTFunction3D operator+(const DTFunction3D &,const DTFunction3D &);
extern DTFunction3D operator-(const DTFunction3D &,const DTFunction3D &);
extern DTFunction3D operator*(const DTFunction3D &,const DTFunction3D &);
extern DTFunction3D operator/(const DTFunction3D &,const DTFunction3D &);
extern DTFunction3D operator-(const DTFunction3D &);

extern DTFunction3D operator+(const DTFunction2D &,const DTFunction3D &);
extern DTFunction3D operator-(const DTFunction2D &,const DTFunction3D &);
extern DTFunction3D operator*(const DTFunction2D &,const DTFunction3D &);
extern DTFunction3D operator/(const DTFunction2D &,const DTFunction3D &);

extern DTFunction3D operator+(const DTFunction1D &,const DTFunction3D &);
extern DTFunction3D operator-(const DTFunction1D &,const DTFunction3D &);
extern DTFunction3D operator*(const DTFunction1D &,const DTFunction3D &);
extern DTFunction3D operator/(const DTFunction1D &,const DTFunction3D &);

extern DTFunction3D operator+(const DTFunction3D &,const DTFunction2D &);
extern DTFunction3D operator-(const DTFunction3D &,const DTFunction2D &);
extern DTFunction3D operator*(const DTFunction3D &,const DTFunction2D &);
extern DTFunction3D operator/(const DTFunction3D &,const DTFunction2D &);

extern DTFunction3D operator+(const DTFunction3D &,const DTFunction1D &);
extern DTFunction3D operator-(const DTFunction3D &,const DTFunction1D &);
extern DTFunction3D operator*(const DTFunction3D &,const DTFunction1D &);
extern DTFunction3D operator/(const DTFunction3D &,const DTFunction1D &);

extern DTFunction3D operator*(const DTFunction3D &,double);
extern DTFunction3D operator*(double,const DTFunction3D &);
extern DTFunction3D operator/(const DTFunction3D &,double);
extern DTFunction3D operator/(double,const DTFunction3D &);
extern DTFunction3D operator+(const DTFunction3D &,double);
extern DTFunction3D operator+(double,const DTFunction3D &);
extern DTFunction3D operator-(const DTFunction3D &,double);
extern DTFunction3D operator-(double,const DTFunction3D &);

extern DTFunction3D abs(const DTFunction3D &);
extern DTFunction3D sin(const DTFunction3D &);
extern DTFunction3D cos(const DTFunction3D &);
extern DTFunction3D asin(const DTFunction3D &);
extern DTFunction3D acos(const DTFunction3D &);
extern DTFunction3D tan(const DTFunction3D &);
extern DTFunction3D atan(const DTFunction3D &);
extern DTFunction3D sinh(const DTFunction3D &);
extern DTFunction3D cosh(const DTFunction3D &);
extern DTFunction3D tanh(const DTFunction3D &);
extern DTFunction3D sqrt(const DTFunction3D &);
extern DTFunction3D gamma(const DTFunction3D &);
extern DTFunction3D loggamma(const DTFunction3D &);
extern DTFunction3D exp(const DTFunction3D &);
extern DTFunction3D log(const DTFunction3D &);
extern DTFunction3D log10(const DTFunction3D &);
extern DTFunction3D erfc(const DTFunction3D &);
extern DTFunction3D floor(const DTFunction3D &);
extern DTFunction3D ceil(const DTFunction3D &);
extern DTFunction3D round(const DTFunction3D &);
extern DTFunction3D fact(const DTFunction3D &);

#endif
