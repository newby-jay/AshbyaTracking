// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTriangularMesh2D_H
#define DTTriangularMesh2D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTTriangularGrid2D.h"

struct DTRegion1D;

class DTTriangularMesh2D {
public:
    DTTriangularMesh2D() : grid(), data() {};
    DTTriangularMesh2D(const DTTriangularGrid2D &grid,const DTDoubleArray &input); // input.m()==grid.NumberOfPoints()
    
    DTDoubleArray Values(void) const {return data;}
    DTTriangularGrid2D Grid(void) const {return grid;}
    
    int IsEmpty() const {return grid.IsEmpty();}
    int NotEmpty() const {return grid.NotEmpty();}
    
    void pinfo(void) const;

protected:
    DTTriangularGrid2D grid;
    DTDoubleArray data;
};

bool operator==(const DTTriangularMesh2D &,const DTTriangularMesh2D &);
bool operator!=(const DTTriangularMesh2D &,const DTTriangularMesh2D &);

extern DTRegion2D BoundingBox(const DTTriangularMesh2D &);
extern DTRegion1D ValueRange(const DTTriangularMesh2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTriangularMesh2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &theMesh);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &input,const std::string &name,DTTriangularMesh2D &toReturn,DTTriangularGrid2D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTTriangularMesh2D &toWrite,DTTriangularGrid2D_SaveInfo &);


class DTMutableTriangularMesh2D : public DTTriangularMesh2D {
public:
    explicit DTMutableTriangularMesh2D(const DTTriangularGrid2D &Grid);
    DTMutableTriangularMesh2D(const DTTriangularGrid2D &Grid,const DTMutableDoubleArray &V);
    
    DTMutableDoubleArray Values(void) {return mutableData;}
    DTDoubleArray Values(void) const {return data;}
    
    void operator=(double v) {mutableData=v;}
    
private:
    DTMutableDoubleArray mutableData;
};

#endif
