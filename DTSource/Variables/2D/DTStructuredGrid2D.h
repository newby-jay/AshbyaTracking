// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredGrid2D_H
#define DTStructuredGrid2D_H

// Each coordinate is stored separately.  For each one it can be stored either
// as a full array or a single vector of values.


#include "DTFloatArray.h"
#include "DTMask.h"
#include "DTDataStorage.h"

struct DTRegion2D;

class DTStructuredGrid2D {
public:
    DTStructuredGrid2D() : Xa(), Ya(), maskDefined(false), mask() {}
    DTStructuredGrid2D(const DTFloatArray &X,const DTFloatArray &Y);
    DTStructuredGrid2D(const DTFloatArray &X,const DTFloatArray &Y,const DTMask &);

    int IsEmpty() const {return (m()==0);}
    int NotEmpty() const {return (m()!=0);}

    DTFloatArray X(void) const {return Xa;}
    DTFloatArray Y(void) const {return Ya;}

    // Slow access if X, Y or Z are vectors, and not full arrays.  Will create the array if necessary.
    DTFloatArray FullXArray(void) const;
    DTFloatArray FullYArray(void) const;

    ssize_t m(void) const {return Xa.m();}
    ssize_t n(void) const {return Ya.n();}

    bool MaskDefined(void) const {return maskDefined;}
    DTMask Mask(void) const {return mask;}

    void pinfo(void) const;

private:
    DTFloatArray Xa,Ya;
    bool maskDefined;
    DTMask mask;
};

bool operator==(const DTStructuredGrid2D &,const DTStructuredGrid2D &);
bool operator!=(const DTStructuredGrid2D &,const DTStructuredGrid2D &);

extern DTRegion2D BoundingBox(const DTStructuredGrid2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTStructuredGrid2D_SaveInfo {
    DTStructuredGrid2D grid;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid2D &toReturn,DTStructuredGrid2D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite,DTStructuredGrid2D_SaveInfo &);

#endif
