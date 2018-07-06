// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSurface3DCoordinates_H
#define DTSurface3DCoordinates_H

#include "DTSurface3D.h"

class DTSurfaceCoordinates3D {
public:
    DTSurfaceCoordinates3D() {}
    DTSurfaceCoordinates3D(const DTSurface3D &,const DTFloatArray &,const DTFloatArray &);
    
    DTSurface3D Surface(void) const {return surface;}
    DTFloatArray U(void) const {return u;}
    DTFloatArray V(void) const {return v;}
    
    void pinfo() const;

private:
    DTSurface3D surface;
    DTFloatArray u,v;
};

extern DTRegion3D BoundingBox(const DTSurfaceCoordinates3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTSurfaceCoordinates3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTSurfaceCoordinates3D_SaveInfo {
    DTSurface3D_SaveInfo surface;
    DTFloatArray u,v;
    std::string uname,vname;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTSurfaceCoordinates3D &toReturn,DTSurfaceCoordinates3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite,DTSurfaceCoordinates3D_SaveInfo &);

#endif
