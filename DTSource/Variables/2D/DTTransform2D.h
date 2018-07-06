// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTransform2D_H
#define DTTransform2D_H

// [x]   [a b sx][x]
// [y] = [c d sy][y]
// [1]   [0 0  1][1]

#include "DTDataStorage.h"
#include "DTPoint2D.h"
#include "DTVector2D.h"

class DTDoubleArray;
class DTFloatArray;
class DTMutableDoubleArray;
class DTMutableFloatArray;

class DTTransform2D {
public:
    DTTransform2D();
    DTTransform2D(double t11,double t12,double s1,
                  double t21,double t22,double s2);
    DTTransform2D(double t11,double t12,double s1,
                  double t21,double t22,double s2,
                  double px,double py);
    
    void pinfo(void) const;
    
    DTPoint2D operator*(const DTPoint2D &P) const;
    DTVector2D operator*(const DTVector2D &P) const;
    DTTransform2D operator*(const DTTransform2D &) const;
    
    bool IsOrthogonal(void) const;
    bool IsPerspective(void) const {return (_Px!=0 || _Py!=0);}
    DTTransform2D Inverse(void) const;
    DTPoint2D ShiftVector(void) const {return DTPoint2D(_S1,_S2);}
    DTTransform2D RotationOnly(void) const {return DTTransform2D(_T11,_T12,0.0,_T21,_T22,0.0);}
    
    // Access the data eitehr as a 3x3 matrix
    double operator()(int i,int j) const;
    
    // Or individual entries of the matrix
    double Txx(void) const {return _T11;}
    double Txy(void) const {return _T12;}
    double Tyx(void) const {return _T21;}
    double Tyy(void) const {return _T22;}
    double Sx(void) const {return _S1;}
    double Sy(void) const {return _S2;}
    double Px(void) const {return _Px;}
    double Py(void) const {return _Py;}
    
    static DTTransform2D Shift(const DTPoint2D &);
    static DTTransform2D Scale(double,double);
    
private:
    double _T11,_T12,_S1;
    double _T21,_T22,_S2;
    double _Px,_Py; // Perspective transformation
};

extern DTTransform2D TransformFromCoordinateToGrid(const DTPoint2D &origin,double dx,double dy);
extern DTTransform2D TransformFromGridToCoordinate(const DTPoint2D &origin,double dx,double dy);
extern DTTransform2D Inverse(const DTTransform2D &);

extern DTMutableDoubleArray TransformPoints(const DTTransform2D &,const DTDoubleArray &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTransform2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTransform2D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTransform2D &toWrite); // One time value, self documenting.

#endif
