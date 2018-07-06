// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPlane3D_H
#define DTPlane3D_H

#include "DTDataStorage.h"
#include "DTPoint3D.h"
#include "DTVector3D.h"

class DTPlane3D {
public:
    DTPlane3D() {}
    DTPlane3D(DTPoint3D c,DTVector3D n,DTVector3D u) : center(c), normal(n), up(u) {}

    DTPoint3D Center(void) const {return center;}
    DTVector3D Normal(void) const {return normal;}
    DTVector3D Up(void) const {return up;}

    void pinfo(void) const;
    
private:
    DTPoint3D center;
    DTVector3D normal;
    DTVector3D up;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPlane3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPlane3D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPlane3D &toWrite); // One time value, self documenting.

#endif
