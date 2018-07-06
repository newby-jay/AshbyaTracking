// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSurfaceTexture3D_H
#define DTSurfaceTexture3D_H

#include "DTSurfaceCoordinates3D.h"
#include "DTBitmap2D.h"

class DTSurfaceTexture3D {
public:
    DTSurfaceTexture3D() {}
    DTSurfaceTexture3D(const DTSurfaceCoordinates3D &,const DTBitmap2D &);
    
    DTSurfaceCoordinates3D Coordinates(void) const {return coordinates;}
    DTBitmap2D Image(void) const {return image;}
    
    void pinfo() const;
    
private:
    DTSurfaceCoordinates3D coordinates;
    DTBitmap2D image;
};

extern DTRegion3D BoundingBox(const DTSurfaceTexture3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTSurfaceTexture3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &input,const std::string &name,DTSurfaceTexture3D &toReturn,DTSurfaceCoordinates3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite,DTSurfaceCoordinates3D_SaveInfo &);

#endif
