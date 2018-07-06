// Part of DTSource. Copyright 2005-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTMesh2DBanded.h"
#include "DTMesh2D.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"
#include "DTError.h"
#include "DTDoubleArrayOperators.h"
#include <cstring>

DTMesh2DBanded::DTMesh2DBanded(const DTMesh2DGrid &gr,const DTDoubleArray &input)
{
    if (input.IsEmpty()) return;
    
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }
    
    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.Mask().NumberOfPoints()) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }

    grid = gr;
    data = input;
}

void DTMesh2DBanded::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty.\n";
        return;
    }
    
    if (grid.dx()==grid.dy()) {
        std::cerr << m() << "x" << n() << " banded mesh, origin = (" << grid.Origin().x << "," << grid.Origin().y << "), h = " << grid.dx();
    }
    else {
        std::cerr << m() << "x" << n() << " banded mesh, origin = (" << grid.Origin().x << "," << grid.Origin().y << "), dx = " << grid.dx() << ", dy = " << grid.dy();
    }
    
    std::cerr << ". Mask = " << grid.Mask().NumberOfPoints() << " points.";
    std::cerr << std::flush;
}

DTMutableMesh2DBanded::DTMutableMesh2DBanded(const DTMesh2DGrid &gr)
{
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh2DBanded(grid)","Mask needs to be defined for the grid.");
        return;
    }
        
    grid = gr;
    mutableData = DTMutableDoubleArray(gr.Mask().NumberOfPoints());;
    data = mutableData;
}

DTMutableMesh2DBanded::DTMutableMesh2DBanded(const DTMesh2DGrid &gr,const DTMutableDoubleArray &input)
{
    if (input.IsEmpty()) return;
    
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }
    
    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.Mask().NumberOfPoints()) {
        DTErrorMessage("DTMesh2DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }
    
    grid = gr;
    data = input;
    mutableData = input;
}

void DTMutableMesh2DBanded::operator=(double v)
{
    mutableData = v;
}

void DTMutableMesh2DBanded::operator*=(double v)
{
    mutableData *= v;
}

void DTMutableMesh2DBanded::operator-=(const DTMesh2DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh2DBanded-=DTMesh2DBanded","Incompatiable grids");
    }
    else {
        mutableData -= M.Data();
    }
}

void DTMutableMesh2DBanded::operator+=(const DTMesh2DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh2DBanded+=DTMesh2DBanded","Incompatiable grids");
    }
    else {
        mutableData += M.Data();
    }
}

void DTMutableMesh2DBanded::operator*=(const DTMesh2DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh2DBanded*=DTMesh2DBanded","Incompatiable grids");
    }
    else {
        mutableData *= M.Data();
    }
}

void operator-=(DTMutableMesh2D &Full,const DTMesh2DBanded &addThis)
{
    if (Full.Grid()!=addThis.Grid()) {
        DTErrorMessage("DTMutableMesh2D-=DTMesh2DBand","Incompatible grids");
        return;
    }
    
    DTMesh2DGrid grid = Full.Grid();
    
    // Just change this portion of the mesh
    DTIntArray intervals = grid.Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTMutableDoubleArray values = Full.DoubleData();
    double *valuesD = values.Pointer();
    DTDoubleArray arr = addThis.Data();
    const double *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[ij] -= arrD[pos++];
        }
    }
}

void operator+=(DTMutableMesh2D &Full,const DTMesh2DBanded &addThis)
{
    if (Full.Grid()!=addThis.Grid()) {
        DTErrorMessage("DTMutableMesh2D+=DTMesh2DBand","Incompatible grids");
        return;
    }
    
    DTMesh2DGrid grid = Full.Grid();
    
    // Just change this portion of the mesh
    DTIntArray intervals = grid.Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTMutableDoubleArray values = Full.DoubleData();
    double *valuesD = values.Pointer();
    DTDoubleArray arr = addThis.Data();
    const double *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[ij] += arrD[pos++];
        }
    }
}

void operator*=(DTMutableMesh2DBanded &banded,const DTMesh2D &Full)
{
    if (SameExceptForMask(Full.Grid(),banded.Grid())==false) {
        DTErrorMessage("DTMutableMesh2DBanded*=DTMesh2D","Incompatible grids");
        return;
    }
    
    DTMesh2DGrid grid = banded.Grid();
    
    // Just change this portion of the mesh
    DTIntArray intervals = grid.Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTDoubleArray values = Full.DoubleData();
    const double *valuesD = values.Pointer();
    DTMutableDoubleArray arr = banded.Data();
    double *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            arrD[pos++] *= valuesD[ij];
        }
    }
}

DTMutableMesh2D ConvertToFull(const DTMesh2DBanded &A)
{
	DTMesh2DGrid grid = A.Grid();
	ssize_t m = grid.m();
	ssize_t n = grid.n();
	DTMutableDoubleArray toReturn(m,n);
	toReturn = 0;
	DTMask theMask = grid.Mask();
	DTIntArray intervals = theMask.Intervals();
	ssize_t howManyIntervals = intervals.n();
	int start,end,which;
	DTDoubleArray theValues = A.Data();
	const double *fromD = theValues.Pointer();
	double *toD = toReturn.Pointer();
	
	int pos = 0;
	for (which=0;which<howManyIntervals;which++) {
		start = intervals(0,which);
		end = intervals(1,which);
		std::memcpy(toD+start,fromD+pos,(end-start+1)*sizeof(double));
		pos += end-start+1;
	}
	
    return DTMutableMesh2D(grid,toReturn);
}

DTMutableMesh2DBanded ConvertToBanded(const DTMesh2D &A)
{
    if (A.Grid().MaskDefined()==false) {
        DTErrorMessage("ConvertToBanded(DTMesh2D)","The mesh needs to have a mask defined");
        return DTMutableMesh2DBanded();
    }
    DTIntArray intervals = A.Grid().Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTMutableDoubleArray values(A.Grid().Mask().NumberOfPoints());
    double *valuesD = values.Pointer();
    DTDoubleArray arr = A.DoubleData();
    const double *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[pos++] = arrD[ij];
        }
    }
    
    return DTMutableMesh2DBanded(A.Grid(),values);
}

void CopyValues(DTMutableMesh2DBanded &A,const DTMesh2D &B)
{
	if (B.Grid().MaskDefined()) {
		if (A.Grid()!=B.Grid()) {
			DTErrorMessage("CopyValues(DTMesh2D,DTMesh2DBanded)","Grid needs to be the same");
			return;
		}
	}
	else {
		if (SameExceptForMask(A.Grid(),B.Grid())==false) {  // The size has to match
			DTErrorMessage("CopyValues(DTMesh2D,DTMesh2DBanded)","Not defined on the same region of space");
			return;
		}
	}
	if (B.DoublePrecision()==false) {
		// This can easily be added.
		DTErrorMessage("CopyValues(DTMesh2D,DTMesh2DBanded)","The mesh needs to be double precision.");
		return;
	}
	
	DTIntArray intervals = A.Grid().Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    DTMutableDoubleArray values = A.Data();
	double *valuesD = values.Pointer();
    DTDoubleArray arr = B.DoubleData();
    const double *arrD = arr.Pointer();

    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[pos++] = arrD[ij];
        }
    }
}

extern DTRegion2D BoundingBox(const DTMesh2DBanded &M)
{
    return BoundingBox(M.Grid());
}

DTRegion1D ValueRange(const DTMesh2DBanded &M)
{
    return ValueRange(M.Data());
}

void Read(const DTDataStorage &input,const std::string &name,DTMesh2DBanded &toReturn)
{
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh2DBanded &theMesh)
{
    Write(output,name+"_bbox2D",BoundingBox(theMesh));
    WriteNoSize(output,name+"_loc",theMesh.Grid());
    Write(output,name+"_dom",theMesh.Grid().Mask());
    Write(output,name,theMesh.Data());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh2DBanded &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Mesh");
    output.Flush();
}

