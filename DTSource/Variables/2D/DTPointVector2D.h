// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPointVector2D_H
#define DTPointVector2D_H

#include "DTDataStorage.h"
#include "DTPoint2D.h"
#include "DTVector2D.h"

struct DTPointVector2D {
    DTPointVector2D() : isSet(false) {}
    DTPointVector2D(DTPoint2D p,DTVector2D v) : isSet(true), point(p), vector(v) {}
    
    void pinfo(void) const;
    
    bool isSet;
    DTPoint2D point;
    DTVector2D vector;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTPointVector2D &var);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointVector2D &var);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPointVector2D &toWrite); // One time value, self documenting.

#endif
