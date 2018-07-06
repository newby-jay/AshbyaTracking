// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTetrahedralVectorField3D_H
#define DTTetrahedralVectorField3D_H

#include "DTTetrahedralGrid3D.h"
#include "DTDataStorage.h"

class DTTetrahedralVectorField3D {
public:
    DTTetrahedralVectorField3D() : grid(), vectors() {}
    DTTetrahedralVectorField3D(const DTTetrahedralGrid3D &Grid,const DTFloatArray &Vectors);

    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTTetrahedralGrid3D Grid(void) const {return grid;}
    DTFloatArray Vectors(void) const {return vectors;}

    void pinfo(void) const;

private:
    DTTetrahedralGrid3D grid;
    DTFloatArray vectors;
};

extern DTRegion3D BoundingBox(const DTTetrahedralVectorField3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralVectorField3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralVectorField3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralVectorField3D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTTetrahedralVectorField3D &toReturn,DTTetrahedralGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTTetrahedralVectorField3D &toWrite,DTTetrahedralGrid3D_SaveInfo &);

#endif
