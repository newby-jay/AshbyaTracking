// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPrismGrid3D.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"
#include "DTRegion3D.h"
#include "DTError.h"

DTPrismGrid3D::DTPrismGrid3D(const DTTriangularGrid2D &xy,const DTFloatArray &z)
{
    if (z.o()>1) {
        DTErrorMessage("DTPrismGrid3D(xy,z)","z is a 3D array");
        return;
    }
    if (xy.NumberOfPoints()!=z.m()) {
        DTErrorMessage("DTPrismGrid3D(xy,z)","z.n()!=xy.NumberOfPoints()");
        return;
    }
    
    xyGrid = xy;
    zValues = z;
}

void DTPrismGrid3D::pinfo(void) const
{
    std::cerr << NumberOfTriangles() << " triangles, " << NumberOfPoints() << " points";
    std::cerr << ", " << NumberOfZSlices() << " slices.\n";
    
    if (xyGrid.BoundaryDefined())
        std::cerr << ", boundary defined";
    
    if (xyGrid.NextTrianglesDefined())
        std::cerr << ", connected triangles";
    
    if (xyGrid.PointNumbersSpecified())
        std::cerr << ", point numbers";
    std::cerr << flush;
}

bool operator==(const DTPrismGrid3D &A,const DTPrismGrid3D &B)
{
    return (A.XYGrid()==B.XYGrid() && A.ZValues()==B.ZValues());
}

bool operator!=(const DTPrismGrid3D &A,const DTPrismGrid3D &B)
{
    return (A.XYGrid()!=B.XYGrid() || A.ZValues()!=B.ZValues());
}

DTRegion3D BoundingBox(const DTPrismGrid3D &grid)
{
    DTRegion2D bbox2D = BoundingBox(grid.XYGrid());
    DTRegion1D range = ValueRange(grid.ZValues());
    if (!bbox2D.isSet || !range.isSet) return DTRegion3D();

    DTRegion3D toReturn;
    
    toReturn.xmin = bbox2D.xmin;
    toReturn.xmax = bbox2D.xmax;
    toReturn.ymin = bbox2D.ymin;
    toReturn.ymax = bbox2D.ymax;
    toReturn.zmin = range.minV;
    toReturn.zmax = range.maxV;
    
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismGrid3D &toReturn)
{
    // Check if there is a redirect.
    std::string theName = input.ResolveName(name);
    
    DTTriangularGrid2D grid;
    Read(input,name,grid);
    
    DTFloatArray zValues;
    Read(input,name+"_z",zValues);
    
    toReturn = DTPrismGrid3D(grid,zValues);
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismGrid3D &toWrite)
{
    Write(output,name+"_bbox3D",BoundingBox(toWrite));
    Write(output,name+"_z",toWrite.ZValues());
    Write(output,name,toWrite.XYGrid());
}

void Read(const DTDataStorage &input,const std::string &name,DTPrismGrid3D &toReturn,DTPrismGrid3D_SaveInfo &SaveInfo)
{
    std::string theName = input.ResolveName(name);
    if (SaveInfo.name==theName) {
        toReturn = SaveInfo.grid; // Already read this grid in.
        return;
    }
    
    Read(input,theName,toReturn);
    
    SaveInfo.grid = toReturn;
    SaveInfo.name = theName;
}

void Write(DTDataStorage &output,const std::string &name,const DTPrismGrid3D &toWrite,DTPrismGrid3D_SaveInfo &SaveInfo)
{
    if (SaveInfo.name.length() && SaveInfo.grid==toWrite) {
        // Just save the reference.
        Write(output,name,SaveInfo.name);
    }
    else {
        Write(output,name,toWrite);
        SaveInfo.grid = toWrite;
        SaveInfo.name = name;
    }
}

