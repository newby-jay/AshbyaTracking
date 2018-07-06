// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredGrid3D_H
#define DTStructuredGrid3D_H

// Each coordinate is stored separately.  For each one it can be stored either
// as a full array or a single vector of values.


#include "DTFloatArray.h"
#include "DTMask.h"
#include "DTDataStorage.h"

struct DTRegion3D;

class DTStructuredGrid3D {
public:
    DTStructuredGrid3D() : Xa(), Ya(), Za(), maskDefined(false), mask() {}
    DTStructuredGrid3D(const DTFloatArray &X,const DTFloatArray &Y,const DTFloatArray &Z);
    DTStructuredGrid3D(const DTFloatArray &X,const DTFloatArray &Y,const DTFloatArray &Z,const DTMask &);
    
    bool IsEmpty(void) const {return Xa.IsEmpty();}
    
    DTFloatArray X(void) const {return Xa;}
    DTFloatArray Y(void) const {return Ya;}
    DTFloatArray Z(void) const {return Za;}

    // Slow access if X, Y or Z are vectors, and not full arrays.  Will create the array if necessary.
    DTFloatArray FullXArray(void) const;
    DTFloatArray FullYArray(void) const;
    DTFloatArray FullZArray(void) const;
    
    int m(void) const {return (int)Xa.m();}
    int n(void) const {return (int)Ya.n();}
    int o(void) const {return (int)Za.o();}

    bool MaskDefined(void) const {return maskDefined;}
    DTMask Mask(void) const {return mask;}
    
    void pinfo(void) const;

protected:
    DTFloatArray Xa,Ya,Za;
    bool maskDefined;
    DTMask mask;
};

class DTMutableStructuredGrid3D : DTStructuredGrid3D
{
public:
    DTMutableStructuredGrid3D() : DTStructuredGrid3D() {}
    DTMutableStructuredGrid3D(const DTMutableFloatArray &X,const DTMutableFloatArray &Y,const DTMutableFloatArray &Z);
    
    DTMutableFloatArray X(void) const {return Xm;}
    DTMutableFloatArray Y(void) const {return Ym;}
    DTMutableFloatArray Z(void) const {return Zm;}
    
private:
    DTMutableFloatArray Xm,Ym,Zm;
};

extern bool operator==(const DTStructuredGrid3D &,const DTStructuredGrid3D &);
extern bool operator!=(const DTStructuredGrid3D &,const DTStructuredGrid3D &);

extern DTRegion3D BoundingBox(const DTStructuredGrid3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTStructuredGrid3D_SaveInfo {
    DTStructuredGrid3D grid;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid3D &toReturn,DTStructuredGrid3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite,DTStructuredGrid3D_SaveInfo &);

#endif
