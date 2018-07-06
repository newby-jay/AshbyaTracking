// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredMesh3D_H
#define DTStructuredMesh3D_H

#include "DTFloatArray.h"
#include "DTDataStorage.h"
#include "DTStructuredGrid3D.h"

struct DTRegion1D;

// Location in space is stored in the DTStructuredGrid3D structure.
// The grid structure could possibly contain a mask.

class DTStructuredMesh3D {
public:
    DTStructuredMesh3D() : grid(), data() {};
    DTStructuredMesh3D(const DTStructuredGrid3D &grid,const DTFloatArray &input);
    
    DTFloatArray Data(void) const {return data;}
    DTStructuredGrid3D Grid(void) const {return grid;}
    
    int IsEmpty() const {return (m()==0);}
    int NotEmpty() const {return (m()!=0);}
    
    int m(void) const {return grid.m();}
    int n(void) const {return grid.n();}
    int o(void) const {return grid.o();}
    
    void pinfo(void) const;
    
private:
    DTStructuredGrid3D grid;
    DTFloatArray data;
};

bool operator==(const DTStructuredMesh3D &,const DTStructuredMesh3D &);
bool operator!=(const DTStructuredMesh3D &,const DTStructuredMesh3D &);

extern DTRegion3D BoundingBox(const DTStructuredMesh3D &);
extern DTRegion1D ValueRange(const DTStructuredMesh3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredMesh3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredMesh3D &theMesh);

extern void Read(const DTDataStorage &,const std::string &name,DTStructuredMesh3D &toReturn,DTStructuredGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTStructuredMesh3D &toWrite,DTStructuredGrid3D_SaveInfo &);

#endif
