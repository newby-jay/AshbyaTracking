// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTriangularGrid2D.h"

#include "DTRegion2D.h"
#include "DTRegion1D.h"
#include "DTError.h"

#include <algorithm>
using std::sort;

DTTriangularGrid2D::DTTriangularGrid2D() : connections(), points()
{
	optional = DTPointer<DTTriangularGridOptionalData>(new DTTriangularGridOptionalData());
}

DTTriangularGrid2D::DTTriangularGrid2D(const DTIntArray &conn,const DTDoubleArray &pts)
{
	optional = DTPointer<DTTriangularGridOptionalData>(new DTTriangularGridOptionalData());
	
    if (pts.IsEmpty() && conn.IsEmpty())
        return;
    
    if (pts.IsEmpty() && conn.NotEmpty()) {
        DTErrorMessage("DTTriangularGrid2D(conn,pts)","Invalid array sizes (one is empty).");
        return;
    }

    if ((pts.m()!=0 && pts.m()!=2) || conn.m()!=3 || conn.o()>1 || pts.o()>1) {
        DTErrorMessage("DTTriangularGrid2D(conn,pts)","Invalid array sizes.");
        return;
    }
    
    DTRegion1D offRange = ValueRange(conn);
    if (offRange.minV<0 || offRange.maxV>=pts.n()) {
        DTErrorMessage("DTTriangularGrid2D(conn,pts)","Offset array refers to points out of range.");
        return;
    }
    
    // Check just the dimensions.
    // If one is empty, everything has to be empty.
    points = pts;
    connections = conn;
}

DTTriangularGrid2D::DTTriangularGrid2D(const DTIntArray &conn,const DTDoubleArray &pts,const DTIntArray &pointN,const DTIntArray &nextTri,const DTPath2D &bdry)
{
    // Do not call this directly.  This constructor assumes that the input arrays are valid, and just assigns them.
    points = pts;
    connections = conn;
	optional = DTPointer<DTTriangularGridOptionalData>(new DTTriangularGridOptionalData());
    optional->pointNumbers = pointN;
    optional->nextTriangles = nextTri;
    optional->domainBoundary = bdry;
}

DTPath2D DTTriangularGrid2D::DomainBoundary(void) const
{
    if (optional->domainBoundary.IsEmpty() && connections.NotEmpty()) {
        DTErrorMessage("DTTriangularGrid2D::DomainBoundary","Boundary has not been set");
    }
    return optional->domainBoundary;
}

DTIntArray DTTriangularGrid2D::NextTriangles(void) const
{
    if (optional->nextTriangles.IsEmpty() && !connections.IsEmpty()) {
        DTErrorMessage("DTTriangularGrid2D::NextTriangles","Not specified");
    }
    return optional->nextTriangles;
}

DTIntArray DTTriangularGrid2D::PointNumbers(void) const
{
    if (optional->pointNumbers.IsEmpty() && points.NotEmpty()) {
        DTErrorMessage("DTTriangularGrid2D::PointNumbers","Point numbers have not been set");
    }
    return optional->pointNumbers;
}

void DTTriangularGrid2D::pinfo(void) const
{
    std::cerr << connections.n() << " triangles, " << points.n() << " points";
    
    if (BoundaryDefined())
        std::cerr << ", boundary defined";
    
    if (NextTrianglesDefined())
        std::cerr << ", connected triangles";
    
    if (PointNumbersSpecified())
        std::cerr << ", point numbers";
    
    std::cerr << ".\n" << flush;
}

DTTriangularGrid2D AddDomain(const DTTriangularGrid2D &tri,const DTPath2D &path)
{
    DTIntArray pointNumbers;
    if (tri.PointNumbersSpecified()) pointNumbers = tri.PointNumbers();
    DTIntArray nextTriangles;
    if (tri.NextTrianglesDefined()) nextTriangles = tri.NextTriangles();
    
    return DTTriangularGrid2D(tri.Connections(),tri.Points(),pointNumbers,nextTriangles,path);
}

DTTriangularGrid2D AddPointNumbers(const DTTriangularGrid2D &tri,const DTIntArray &ptNo)
{
    if (ptNo.NotEmpty() && (ptNo.o()>1 || ptNo.n()>1 || ptNo.Length()!=tri.NumberOfPoints())) {
        DTErrorMessage("AddPointNumbers(DTTriangularGrid2D,...)","Invalid array size");
        return DTTriangularGrid2D();
    }
    
    DTIntArray nextTriangles;
    if (tri.NextTrianglesDefined()) nextTriangles = tri.NextTriangles();
    DTPath2D domainPath;
    if (tri.BoundaryDefined()) domainPath = tri.DomainBoundary();
    
    return DTTriangularGrid2D(tri.Connections(),tri.Points(),ptNo,nextTriangles,domainPath);
}

DTTriangularGrid2D AddNextTriangleInformation(const DTTriangularGrid2D &tri,const DTIntArray &nextTri)
{
    if (nextTri.NotEmpty()) {
        if (nextTri.o()>1 || nextTri.m()!=3 || nextTri.n()!=tri.NumberOfTriangles()) {
            DTErrorMessage("AddNextTriangleInformation(DTTriangularGrid2D,DTIntArray)","Invalid array size.");
            return DTTriangularGrid2D();
        }
        DTRegion1D valRegion = ValueRange(nextTri);
        if (valRegion.minV<-1 || valRegion.maxV>=tri.NumberOfTriangles()*3) {
            DTErrorMessage("AddNextTriangleInformation(DTTriangularGrid2D,DTIntArray)","Invalid offset");
            return DTTriangularGrid2D();
        }
    }
    
    DTPath2D domainPath;
    if (tri.BoundaryDefined()) domainPath = tri.DomainBoundary();
    DTIntArray pointNumbers;
    if (tri.PointNumbersSpecified()) pointNumbers = tri.PointNumbers();
    
    return DTTriangularGrid2D(tri.Connections(),tri.Points(),pointNumbers,nextTri,domainPath);
}

struct DTTriangleSideLong {
    
    bool operator<(const DTTriangleSideLong &A) const {return (label<A.label);}
    
    unsigned long long int label;
    int positionInOffset;
};

DTTriangularGrid2D AddNextTriangleInformation(const DTTriangularGrid2D &tri)
{
    if (tri.NextTrianglesDefined()) return tri;
    
    DTIntArray toReturn;
    DTIntArray offsets = tri.Connections();
    int Len = (int)offsets.n();
    int i;
    int Ai,Bi,Ci,ind1,ind2,temp;
    
    // Can save the offset as a single unsigned long long.
    DTMutableList<DTTriangleSideLong> offsetInfo(3*Len);
    DTTriangleSideLong side;
    
    for (i=0;i<Len;i++) {
        Ai = offsets(0,i);
        Bi = offsets(1,i);
        Ci = offsets(2,i);
        
        // Numbering is AB,BC,CA
        
        // The AB side.
        ind1 = Ai;
        ind2 = Bi;
        // Sort the indices with bubble sort.
        if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
        side.label = ind2;
        side.label *= Len;
        side.label += ind1;
        side.positionInOffset = 3*i;
        offsetInfo(3*i) = side;
        
        // BC
        ind1 = Bi;
        ind2 = Ci;
        if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
        side.label = ind2;
        side.label *= Len;
        side.label += ind1;
        side.positionInOffset = 3*i+1;
        offsetInfo(3*i+1) = side;
        
        // CA
        ind1 = Ci;
        ind2 = Ai;
        if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
        side.label = ind2;
        side.label *= Len;
        side.label += ind1;
        side.positionInOffset = 3*i+2;
        offsetInfo(3*i+2) = side;
    }
    
    // Sort the list
    sort(offsetInfo.Pointer(),offsetInfo.Pointer()+offsetInfo.Length());
    
    // Each side should be repeated.  Go through the sorted list and connect the pairs.
    
    DTMutableIntArray NextTri(3,Len);
    NextTri = -1;
    
    int pos = 0;
    int pos1,pos2;
    
    while (pos<3*Len-1) {
        if (offsetInfo(pos).label==offsetInfo(pos+1).label) {
            pos1 = offsetInfo(pos).positionInOffset;
            pos2 = offsetInfo(pos+1).positionInOffset;
            NextTri(pos1%3,pos1/3) = pos2;
            NextTri(pos2%3,pos2/3) = pos1;
            pos+=2;
        }
        else {
            // Not connected
            pos++;
        }
    }
    
    return AddNextTriangleInformation(tri,NextTri);
}

DTIntArray EdgeSegments(const DTTriangularGrid2D &tri)
{
    if (tri.NextTrianglesDefined()==false) {
        // This function should not call the AddNextTriangleInformation(...) function.
        // That is consided an unwanted side-effect.  Move that call higher up in the calling sequence.
        DTErrorMessage("EdgeSegments(DTTriangularGrid2D)","Missing connectivity information.  Use grid = AddNextTriangleInformation(grid) before calling this.");
        return DTIntArray();
    }
    
    // Go over all of the triangles, and search out triangles that have an outside.
    DTIntArray connections = tri.Connections();

    DTIntArray nextTriangles = tri.NextTriangles();
    int howManyTriangles = (int)nextTriangles.n();
    int triN;
    DTMutableIntArray edges(2,50);
    int posInEdges = 0;
    int trigger = 50-3;
    
    for (triN=0;triN<howManyTriangles;triN++) {
        if (trigger<posInEdges) {
            edges = IncreaseSize(edges,edges.Length());
            trigger = (int)edges.n()-3;
        }
        if (nextTriangles(0,triN)==-1) {
            edges(0,posInEdges) = connections(0,triN);
            edges(1,posInEdges) = connections(1,triN);
            posInEdges++;
        }
        if (nextTriangles(1,triN)==-1) {
            edges(0,posInEdges) = connections(1,triN);
            edges(1,posInEdges) = connections(2,triN);
            posInEdges++;
        }
        if (nextTriangles(2,triN)==-1) {
            edges(0,posInEdges) = connections(2,triN);
            edges(1,posInEdges) = connections(0,triN);
            posInEdges++;
        }
    }
    
    edges = TruncateSize(edges,posInEdges*2);
    
    return edges;
}

DTDoubleArray TriangleAreas(const DTTriangularGrid2D &grid)
{
	DTIntArray connections = grid.Connections();
	DTDoubleArray points = grid.Points();
	int tri,howMany = (int)connections.n();
	DTMutableDoubleArray toReturn(howMany);
	double Ax,Ay,Bx,By,Cx,Cy;
	int iA,iB,iC;
	for (tri=0;tri<howMany;tri++) {
		iA = connections(0,tri);
		iB = connections(1,tri);
		iC = connections(2,tri);
		Ax = points(0,iA);
		Ay = points(1,iA);
		Bx = points(0,iB);
		By = points(1,iB);
		Cx = points(0,iC);
		Cy = points(1,iC);
		toReturn(tri) = 0.5*(fabs((Bx-Ax)*(Cy-Ay) - (Cx-Ax)*(By-Ay)));
	}
	return toReturn;
}

bool operator==(const DTTriangularGrid2D &A,const DTTriangularGrid2D &B)
{
    return (A.Points()==B.Points() && A.Connections()==B.Connections());
}

bool operator!=(const DTTriangularGrid2D &A,const DTTriangularGrid2D &B)
{
    return (A.Points()!=B.Points() || A.Connections()!=B.Connections());
}

DTRegion2D BoundingBox(const DTTriangularGrid2D &G)
{
    return BoundingBox2D(G.Points());
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularGrid2D &toReturn)
{
    // Check if there is a redirect.
    std::string theName = input.ResolveName(name);
    
    DTIntArray conn;
    DTDoubleArray pts;
    Read(input,theName+"_pts",pts);
    Read(input,theName,conn);
    
    toReturn = DTTriangularGrid2D(conn,pts);
    
    if (input.Contains(theName+"_next")) {
        DTIntArray nextArr;
        Read(input,theName+"_next",nextArr);
        toReturn = AddNextTriangleInformation(toReturn,nextArr);
    }
    
    if (input.Contains(theName+"_ptno")) {
        DTIntArray ptNo;
        Read(input,theName+"_ptno",ptNo);
        toReturn = AddPointNumbers(toReturn,ptNo);
    }
    
    if (input.Contains(theName+"_bdry")) {
        DTPath2D bdry;
        Read(input,theName+"_bdry",bdry);
        toReturn = AddDomain(toReturn,bdry);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite)
{
    if (toWrite.NextTrianglesDefined())
        Write(output,name+"_next",toWrite.NextTriangles());

    if (toWrite.PointNumbersSpecified())
        Write(output,name+"_ptno",toWrite.PointNumbers());
    
    if (toWrite.BoundaryDefined())
        Write(output,name+"_bdry",toWrite.DomainBoundary());
    
    Write(output,name+"_bbox2D",BoundingBox(toWrite));
    Write(output,name+"_pts",toWrite.Points());
    Write(output,name,toWrite.Connections());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Triangular Grid");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTriangularGrid2D &toReturn,DTTriangularGrid2D_SaveInfo &SaveInfo)
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

void Write(DTDataStorage &output,const std::string &name,const DTTriangularGrid2D &toWrite,DTTriangularGrid2D_SaveInfo &SaveInfo)
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

