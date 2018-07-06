// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTShiftScale3D_H
#define DTShiftScale3D_H

#include "DTDataStorage.h"
#include "DTPoint3D.h"

class DTShiftScale3D {
public:
    DTShiftScale3D() : shift(), scaleX(1.0), scaleY(1.0), scaleZ(1.0) {}
    DTShiftScale3D(DTPoint3D P,double sX,double sY,double sZ) : shift(P), scaleX(sX), scaleY(sY), scaleZ(sZ) {}

    double ScaleX(void) const {return scaleX;}
    double ScaleY(void) const {return scaleY;}
    double ScaleZ(void) const {return scaleZ;}
    DTPoint3D Shift(void) const {return shift;}
    
    void pinfo(void) const;
    
    double TransformX(double x) const {return shift.x+x*scaleX;}
    double TransformY(double y) const {return shift.y+y*scaleY;}
    double TransformZ(double z) const {return shift.z+z*scaleZ;}
    DTPoint3D Transform(const DTPoint3D &P) const {return DTPoint3D(shift.x+P.x*scaleX,shift.y+P.y*scaleY,shift.z+P.z*scaleZ);}

private:
    DTPoint3D shift;
    double scaleX;
    double scaleY;
    double scaleZ;
};

// Creating some standard transformations
extern DTShiftScale3D ShiftBy(DTPoint3D);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTShiftScale3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTShiftScale3D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTShiftScale3D &toWrite); // One time value, self documenting.

#endif
