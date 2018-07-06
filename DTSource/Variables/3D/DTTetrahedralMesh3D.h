// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTetrahedralMesh3D_H
#define DTTetrahedralMesh3D_H

#include "DTTetrahedralGrid3D.h"
#include "DTDataStorage.h"

struct DTRegion1D;

class DTTetrahedralMesh3D {
public:
    DTTetrahedralMesh3D() : grid(), floatData(), doubleData() {}
    DTTetrahedralMesh3D(const DTTetrahedralGrid3D &,const DTFloatArray &); // values.m()==Grid.NumberOfVertices()
    DTTetrahedralMesh3D(const DTTetrahedralGrid3D &,const DTDoubleArray &); // values.m()==Grid.NumberOfVertices()
    
    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTTetrahedralGrid3D Grid(void) const {return grid;}

    bool FloatPrecision(void) const {return (floatData.NotEmpty());}
    bool DoublePrecision(void) const {return (doubleData.NotEmpty());}
    
    DTDoubleArray DoubleData(void) const;
    DTFloatArray FloatData(void) const;

    DTFloatArray Values(void) const; // Obsolete (9/21/14).

    void pinfo(void) const;

protected:
    DTTetrahedralGrid3D grid;
    DTFloatArray floatData;
    DTDoubleArray doubleData;
};

class DTMutableTetrahedralMesh3D : public DTTetrahedralMesh3D {
public:
    explicit DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &Grid);
    DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &,const DTMutableFloatArray &);
    DTMutableTetrahedralMesh3D(const DTTetrahedralGrid3D &,const DTMutableDoubleArray &);
    
    // Obsolete (9/21/14), will print an error message
    DTMutableFloatArray Values(void) const;
    
    DTMutableDoubleArray DoubleData(void) const;
    DTMutableFloatArray FloatData(void) const;
    
private:
    DTMutableFloatArray mutableFloatData;
    DTMutableDoubleArray mutableDoubleData;
};


extern DTRegion1D ValueRange(const DTTetrahedralMesh3D &);
extern DTRegion3D BoundingBox(const DTTetrahedralMesh3D &);

// Free if the type is the same
extern DTTetrahedralMesh3D ConvertToFloat(DTTetrahedralMesh3D);
extern DTTetrahedralMesh3D ConvertToDouble(DTTetrahedralMesh3D);

extern DTTetrahedralMesh3D ChangeGrid(const DTTetrahedralGrid3D &,const DTTetrahedralMesh3D &); // Need the same vertex count

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralMesh3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralMesh3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralMesh3D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTTetrahedralMesh3D &toReturn,DTTetrahedralGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTTetrahedralMesh3D &toWrite,DTTetrahedralGrid3D_SaveInfo &);

#endif
