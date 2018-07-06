// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTCoordinatePlane3D_H
#define DTCoordinatePlane3D_H

#include "DTDataStorage.h"

class DTVector3D;

class DTCoordinatePlane3D {
public:
    enum PlaneType {Empty, XPlane, YPlane, ZPlane};

    DTCoordinatePlane3D() : type(DTCoordinatePlane3D::Empty), value(0.0) {}
    DTCoordinatePlane3D(PlaneType t,double v) : type(t), value(v) {}

    DTVector3D Normal() const;
    PlaneType Type(void) const {return type;}
    double Value(void) const {return value;}
    
    void pinfo(void) const;
    
private:
    PlaneType type;
    double value;
};

extern DTCoordinatePlane3D XYPlane(double z);
extern DTCoordinatePlane3D XZPlane(double y);
extern DTCoordinatePlane3D YZPlane(double x);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTCoordinatePlane3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTCoordinatePlane3D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTCoordinatePlane3D &toWrite); // One time value, self documenting.

#endif
