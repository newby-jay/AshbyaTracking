// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPointVector3D_H
#define DTPointVector3D_H

#include "DTDataStorage.h"
#include "DTPoint3D.h"
#include "DTVector3D.h"

struct DTPointVector3D {
    DTPointVector3D() : isSet(false) {}
    DTPointVector3D(DTPoint3D p,DTVector3D v) : isSet(true), point(p), vector(v) {}
    
    void pinfo(void) const;
    
    bool isSet;
    DTPoint3D point;
    DTVector3D vector;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTPointVector3D &var);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointVector3D &var);

#endif
