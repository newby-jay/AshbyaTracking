// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPrismGrid3D_H
#define DTPrismGrid3D_H

#include "DTTriangularGrid2D.h"
#include "DTFloatArray.h"

struct DTRegion3D;

class DTPrismGrid3D {
public:
    DTPrismGrid3D() : xyGrid(), zValues() {}
    DTPrismGrid3D(const DTTriangularGrid2D &xy,const DTFloatArray &z);
    
    bool IsEmpty(void) const {return xyGrid.IsEmpty();}
    DTTriangularGrid2D XYGrid(void) const {return xyGrid;}
    DTFloatArray ZValues(void) const {return zValues;}; // MxN, where M is the number of vertices, N number of layers.
    
    ssize_t NumberOfPoints(void) const {return xyGrid.NumberOfPoints();}
    ssize_t NumberOfTriangles(void) const {return xyGrid.NumberOfTriangles();}
    int NumberOfZSlices(void) const {return (int)zValues.n();}
    
    void pinfo(void) const;
    
private:
    DTTriangularGrid2D xyGrid;
    DTFloatArray zValues;    
};

extern bool operator==(const DTPrismGrid3D &,const DTPrismGrid3D &);
extern bool operator!=(const DTPrismGrid3D &,const DTPrismGrid3D &);

extern DTRegion3D BoundingBox(const DTPrismGrid3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPrismGrid3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPrismGrid3D &toWrite);

// A utility class to enable grid sharing.
struct DTPrismGrid3D_SaveInfo {
    DTPrismGrid3D grid;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTPrismGrid3D &toReturn,DTPrismGrid3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTPrismGrid3D &toWrite,DTPrismGrid3D_SaveInfo &);

#endif
