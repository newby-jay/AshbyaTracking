// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTetrahedralGrid3D_H
#define DTTetrahedralGrid3D_H

#include "DTIntArray.h"
#include "DTDataStorage.h"
#include "DTPointCollection3D.h"

struct DTRegion3D;

// The grid class for DTTetrahedralMesh3D and DTTetrahedralVectorField3D

class DTTetrahedralGrid3D {
public:
    DTTetrahedralGrid3D() {}
    DTTetrahedralGrid3D(const DTIntArray &Offsets,const DTPointCollection3D &Points);
    DTTetrahedralGrid3D(const DTIntArray &Offsets,const DTPointCollection3D &Points,const DTIntArray &NextTetra);
    DTTetrahedralGrid3D(const DTIntArray &Offsets,const DTPointCollection3D &Points,
                        const DTIntArray &NextTetra,const DTIntArray &PointNumbers); // See comments below.

    bool IsEmpty(void) const {return points.IsEmpty();}
    bool CheckIfNextTetraArrayIsValid() const;
    
    ssize_t NumberOfVertices(void) const {return points.NumberOfPoints();}
    
    DTPointCollection3D Points(void) const {return points;}
    DTIntArray Offsets(void) const {return offsets;}

    // Optional information
    bool NextTetrahedraExists(void) const {return nextTetra.NotEmpty();}
    DTIntArray NeighboringTetrahedra(void) const {return nextTetra;} 
    
    DTIntArray PointNumbers(void) const {return pointNumbers;}

    void pinfo(void) const;
    
private:
        
    DTPointCollection3D points; // 3xN array
    DTIntArray offsets; // 4xM array

    // Optional
    DTIntArray nextTetra;
    // This array is a 4xM Array that specifies the adjoining tetrahedra.  The coding is done as follows:
    // Let A,B,C,D be the points on the tetrahedra (the offsets stored in the offsets array)
    // Number the sides as 0=ABC, 1=ADB, 2=ACD, 3=BDC (oriented consistent with an outward pointing normal)
    // The nextTetra entries are then
    // nextTetra(side,tetra) = (next tetra number)*4 + (number of the adjoining side of the other adjoining tetra).
    // Example:
    // If side S1 on tetra N1 touches side S2 on tetra N2, then
    // nextTetra(S1,N1) = 4*N2 + S2    and nextTetra(S2,N2) = 4*N1 + S1;
    
    DTIntArray pointNumbers; // N numbers
};

extern DTRegion3D BoundingBox(const DTTetrahedralGrid3D &);

extern bool operator==(const DTTetrahedralGrid3D &,const DTTetrahedralGrid3D &);
extern bool operator!=(const DTTetrahedralGrid3D &,const DTTetrahedralGrid3D &);

// Free if the type is the same
extern DTTetrahedralGrid3D ConvertToFloat(const DTTetrahedralGrid3D &);
extern DTTetrahedralGrid3D ConvertToDouble(const DTTetrahedralGrid3D &);

// Modifying grids
extern DTTetrahedralGrid3D SetNextTetrahedra(const DTTetrahedralGrid3D &,const DTIntArray &);
extern DTTetrahedralGrid3D SetPointNumbers(const DTTetrahedralGrid3D &,const DTIntArray &);
extern DTTetrahedralGrid3D AddNextTetrahedra(const DTTetrahedralGrid3D &); // Computes it if it doesn't exist already.

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralGrid3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite);

// A utility class to enable grid sharing.
struct DTTetrahedralGrid3D_SaveInfo {
    DTTetrahedralGrid3D grid;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralGrid3D &toReturn,DTTetrahedralGrid3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite,DTTetrahedralGrid3D_SaveInfo &);

#endif
