// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPoint2DValue_H
#define DTPoint2DValue_H

#include "DTDataStorage.h"
#include "DTPoint2D.h"

struct DTPoint2DValue {
    DTPoint2DValue() {};
    DTPoint2DValue(DTPoint2D pv,double vv) : point(pv), value(vv) {}

    void pinfo(void) const;
    
    DTPoint2D point;
    double value;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPoint2DValue &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPoint2DValue &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPoint2DValue &toWrite); // One time value, self documenting.

#endif
