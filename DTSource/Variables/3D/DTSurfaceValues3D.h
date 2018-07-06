// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSurfaceValues3D_H
#define DTSurfaceValues3D_H

#include "DTSurface3D.h"

// A surface, where each vertex has a value assigned.
// Need to first create the surface, and save the values in the same order as the vertices are saved
// in the surface.

struct DTRegion1D;

class DTSurfaceValues3D {
public:
    DTSurfaceValues3D() {}
    DTSurfaceValues3D(const DTSurface3D &surf,const DTFloatArray &val);
    DTSurfaceValues3D(const DTSurface3D &surf,const DTDoubleArray &val);
    
    bool IsEmpty(void) const {return (surface.IsEmpty());}

    bool FloatPrecision(void) const {return (_floatData.NotEmpty());}
    bool DoublePrecision(void) const {return (_doubleData.NotEmpty());}
    
    const DTDoubleArray &DoubleData(void) const;
    const DTFloatArray &FloatData(void) const;

    DTSurface3D Surface(void) const {return surface;}
    
    void pinfo(void) const;

protected:
    DTSurface3D surface;
    DTFloatArray _floatData;
    DTDoubleArray _doubleData;
};

extern bool operator==(const DTSurfaceValues3D &,const DTSurfaceValues3D &);
extern bool operator!=(const DTSurfaceValues3D &,const DTSurfaceValues3D &);

extern DTRegion1D ValueRange(const DTSurfaceValues3D &);
extern DTRegion3D BoundingBox(const DTSurfaceValues3D &);
extern DTSurfaceValues3D Combine(const DTList<DTSurfaceValues3D> &);

extern DTSurfaceValues3D operator-(const DTSurfaceValues3D &,const DTSurfaceValues3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTSurfaceValues3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTSurfaceValues3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceValues3D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTSurfaceValues3D &toReturn,DTSurface3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTSurfaceValues3D &toWrite,DTSurface3D_SaveInfo &);

// ************************************************************************************************************

class DTMutableSurfaceValues3D : public DTSurfaceValues3D {
public:
    explicit DTMutableSurfaceValues3D(const DTSurface3D &);
    DTMutableSurfaceValues3D(const DTSurface3D &,DTMutableFloatArray &);
	
	void operator=(float v) {mutableValues=v;}
	void operator*=(float v) {mutableValues*=v;}
	
private:
    DTMutableFloatArray mutableValues;
};

#endif
