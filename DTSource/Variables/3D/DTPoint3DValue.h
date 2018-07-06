// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPoint3DValue_H
#define DTPoint3DValue_H

#include "DTDataStorage.h"
#include "DTPoint3D.h"

struct DTPoint3DValue {
    DTPoint3DValue() {}
    DTPoint3DValue(DTPoint3D pv,double vv) : point(pv), value(vv) {}

    void pinfo(void) const;
    
    DTPoint3D point;
    double value;
};

inline DTPoint3DValue operator+(const DTPoint3DValue &A,const DTPoint3DValue &B) {return DTPoint3DValue(A.point+B.point,A.value+B.value);}
inline DTPoint3DValue operator-(const DTPoint3DValue &A,const DTPoint3DValue &B) {return DTPoint3DValue(A.point-B.point,A.value-B.value);}
inline DTPoint3DValue operator*(const DTPoint3DValue &A,double v) {return DTPoint3DValue(A.point*v,A.value*v);}
inline DTPoint3DValue operator*(double v,const DTPoint3DValue &A) {return DTPoint3DValue(A.point*v,A.value*v);}
inline DTPoint3DValue operator/(const DTPoint3DValue &A,double v) {return DTPoint3DValue(A.point/v,A.value/v);}
inline DTPoint3DValue operator-(const DTPoint3DValue &A) {return DTPoint3DValue(-A.point,-A.value);}

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPoint3DValue &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPoint3DValue &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPoint3DValue &toWrite); // One time value, self documenting.

#endif
