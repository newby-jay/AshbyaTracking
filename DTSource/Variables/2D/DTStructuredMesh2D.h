// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredMesh2D_H
#define DTStructuredMesh2D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTStructuredGrid2D.h"

struct DTRegion1D;

// Location in space is stored in the DTStructuredGrid2D structure.
// The grid structure could possibly contain a mask.

class DTStructuredMesh2D {
public:
    DTStructuredMesh2D() : grid(), data() {};
    DTStructuredMesh2D(const DTStructuredGrid2D &grid,const DTDoubleArray &input);

    DTDoubleArray Data(void) const {return data;}
    DTStructuredGrid2D Grid(void) const {return grid;}

    int IsEmpty() const {return (m()==0);}
    int NotEmpty() const {return (m()!=0);}

    size_t m(void) const {return grid.m();}
    size_t n(void) const {return grid.n();}

    void pinfo(void) const;

private:
    DTStructuredGrid2D grid;
    DTDoubleArray data;
};

bool operator==(const DTStructuredMesh2D &,const DTStructuredMesh2D &);
bool operator!=(const DTStructuredMesh2D &,const DTStructuredMesh2D &);

extern DTRegion2D BoundingBox(const DTStructuredMesh2D &);
extern DTRegion1D ValueRange(const DTStructuredMesh2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh2D &theMesh);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredMesh2D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTStructuredMesh2D &toReturn,DTStructuredGrid2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTStructuredMesh2D &toWrite,DTStructuredGrid2D_SaveInfo &);

#endif
