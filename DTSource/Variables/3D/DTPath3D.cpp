// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPath3D.h"

#include "DTError.h"
#include "DTDataStorage.h"
#include "DTIntArray.h"
#include "DTPath2D.h" // For the ConnectSegmentIndices call.
#include "DTShiftScale3D.h"
#include "DTArrayConversion.h"
#include "DTFloatArray.h"

#include <math.h>
#include <limits>
#include <algorithm>
#include <cstring>


DTPath3D::DTPath3D(const DTDoubleArray &input)
{
    SetStorage(input);
}

DTPath3D::DTPath3D(const DTDoubleArray &input,DTRegion3D crop)
{
    if (SetStorage(input))
        _cropBox = crop;
}

bool DTPath3D::SetStorage(const DTDoubleArray &input)
{
    if  (input.IsEmpty()) return false;

    // Check dimension.
    if ((input.m()!=3 && input.m()!=6) || input.o()!=1) {
        DTErrorMessage("DTPath(Array)","Invalid size of path array.");
        return false;
    }

    // Check to see that the loop format is correct.
    if (input.m()==3) {
        int loc = 0;
        int length = (int)input.n();
        int loopLength;
        while (loc<length) {
            if (input(2,loc)!=floor(input(2,loc)) || input(2,loc)<=1) {
                DTErrorMessage("DTPath(Array)","Invalid loop length.");
                break;
            }
            loopLength = int(input(2,loc));
            if (loopLength+loc+1>length) {
                DTErrorMessage("DTPath(Array)","A loop length is too long.");
                break;
            }
            loc += loopLength+1;
        }

        if (loc<length)
            return false;
    }

    _data = input;

    return true;
}

int DTPath3D::NumberOfPoints(void) const
{
    if (StoredAsSegments()) {
        DTErrorMessage("DTPath3D::NumberOfPoints","Stored as segments");
        return 0;
    }
    
    int toReturn = 0;
    DTDoubleArray pathData = Data();
    int loc = 0;
    int len = (int)pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+1+loopLength;
        toReturn += loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1) && pathData(2,loc+1)==pathData(2,until-1))
            toReturn--;
        loc = until;
    }
    
    return toReturn;
}

int DTPath3D::NumberOfLoops(void) const
{
    if (StoredAsSegments()) {
        DTErrorMessage("DTPath3D::NumberOfLoops","Stored as segments");
        return 0;
    }
    
    int toReturn = 0;
    DTDoubleArray pathData = Data();
    int loc = 0;
    int len = (int)pathData.n();
    int loopLength;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        loc += 1+loopLength;
        toReturn++;
    }
    
    return toReturn;
}

void DTPath3D::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty Path\n";
    }
    else if (StoredAsSegments()) {
        if (_data.n()==1) {
            std::cerr << "one segment\n";
        }
        else {
            std::cerr << _data.n() << " segments\n";
        }
    }
    else if (NumberOfLoops()==1) {
        std::cerr << NumberOfPoints() << " points, 1 loop.\n";
    }
    else {
        std::cerr << NumberOfPoints() << " points, " << NumberOfLoops() << " loops.\n";
    }
    std::cerr << std::flush;
}

void DTPath3D::pall(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n";
    }
    else if (StoredAsSegments()) {
        _data.pall();
    }
    else {
        if (NumberOfLoops()==1) {
            DTMutableDoubleArray temp(3,_data.n()-1);
            std::memcpy(temp.Pointer(),_data.Pointer()+3,temp.Length()*sizeof(double));
            temp.pall();
        }
        else {
            int len = NumberOfLoops();
            for (int i=0;i<len;i++) {
                std::cerr << "Loop #" << i << std::endl;
                ExtractLoop(*this,i).pall();
            }
        }
    }
}

DTPath3D ExtractLoop(const DTPath3D &P,int i)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("ExtractLoop(DTPath3D,int)","Stored as segments");
        return DTPath3D();
    }
    
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    int until = 0;
    int len = (int)pathData.n();
    int loopLength;
    int thisIsLoop = 0;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+1+loopLength;
        if (thisIsLoop==i)
            break;
        loc = until;
        thisIsLoop++;
    }
    
    if (loc==len) {
        DTErrorMessage("ExtractLoop(DTPath3D,int)","Index out of bounds");
        return DTPath3D();
    }
    
    DTMutableDoubleArray section(3,until-loc);
    std::memcpy(section.Pointer(),pathData.Pointer()+3*loc,section.Length()*sizeof(double));
    
    DTPath3D toReturn(section,P.CroppingBox());
    
    return toReturn;
}

DTPath3D Combine(const DTList<DTPath3D> &List)
{
	int i,howMany = (int)List.Length();
	int totalLength = 0;
	if (howMany==0) return DTPath3D();
	if (howMany==1) return List(0);
	i = 0;
	while (i<howMany && List(i).IsEmpty()) i++;
	int firstIndex = i;
	if (firstIndex==howMany) return DTPath3D();
	bool typeOfFirst = List(i).StoredAsLoops();
	while (i<howMany) {
		if (typeOfFirst!=List(i).StoredAsLoops())
			break;
		totalLength += List(i).Data().n();
		i++;
	}
	if (i<howMany) {
		DTErrorMessage("Combine(List of DTPath3D)", "All the paths have to have the same type (Loops or Segments)");
		return DTPath3D();
	}
	DTMutableDoubleArray combinedList(List(firstIndex).Data().m(),totalLength);
	int posInCombined = 0;
	DTDoubleArray array;
	for (i=firstIndex;i<howMany;i++) {
		array = List(i).Data();
		std::memcpy(combinedList.Pointer()+posInCombined,array.Pointer(),array.Length()*sizeof(double));
		posInCombined+=array.Length();
	}
	
	return DTPath3D(combinedList);
}

DTPath3D CombineLoops(const DTPath3D &P,double eps)
{
	if (P.IsEmpty()) return P;
	if (P.StoredAsLoops()==false) {
		DTErrorMessage("CombineLoops(DTPath3D,eps)","Path needs to be stored as loops");
		return DTPath3D();
	}
	DTDoubleArray D = P.Data();
	if (D(0,2)+1==D.n()) return P; // Only one loop stored.
	
	DTMutableDoubleArray newArray(3,D.n()-P.NumberOfLoops()+1);
	
	// The first loop
	int loopLength = int(D(2,0));
	std::memcpy(newArray.Pointer()+3,D.Pointer()+3,3*loopLength*sizeof(double));
	int pos = 1+loopLength;
	int loc = 1+loopLength;
	int len = (int)D.n();
    while (loc<len) {
        loopLength = int(D(2,loc));
		if ((D(0,loc-1)-D(0,loc+1))*(D(0,loc-1)-D(0,loc+1)) + (D(1,loc-1)-D(1,loc+1))*(D(1,loc-1)-D(1,loc+1)) + (D(2,loc-1)-D(2,loc+1))*(D(2,loc-1)-D(2,loc+1))<eps*eps) {
			std::memcpy(newArray.Pointer()+3*pos,D.Pointer()+3*(loc+2),(loopLength-1)*3*sizeof(double));
			pos += loopLength-1;
		}
		else {
			std::memcpy(newArray.Pointer()+3*pos,D.Pointer()+3*(loc+1),loopLength*3*sizeof(double));
			pos += loopLength;
		}
		loc += 1+loopLength;
    }
	if (pos<newArray.n())
		newArray = TruncateSize(newArray,3*pos);
	newArray(0,0) = 0;
	newArray(1,0) = 0;
	newArray(2,0) = pos-1;
	
	return DTPath3D(newArray);
}

DTPath3D operator*(const DTShiftScale3D &M,const DTPath3D &P)
{
	if (P.StoredAsSegments()) {
		DTErrorMessage("DTShiftScale3D*DTPath3D","Not implemented yet for a segmented path");
		return DTPath3D();
	}
	DTPoint3D p = M.Shift();
	double xs = M.ScaleX();
	double ys = M.ScaleX();
	double zs = M.ScaleX();
	
	if (xs==1.0 && ys==1.0 && zs==1.0 && p.x==0.0 && p.y==0.0 && p.z==0.0) return P;
	
	DTMutableDoubleArray pathData = P.Data().Copy();
    int loc = 0;
    int len = (int)pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
		loc++;
        until = loc+loopLength;
		while (loc<until) {
			pathData(0,loc) = pathData(0,loc)*xs + p.x;
			pathData(1,loc) = pathData(1,loc)*ys + p.y;
			pathData(2,loc) = pathData(2,loc)*zs + p.z;
			loc++;
		}
    }
	
	return DTPath3D(pathData);
}

double Length(const DTPath3D &P)
{
	DTDoubleArray pathData = P.Data();
    int loc = 0;
    int len = (int)pathData.n();
    int loopLength,until;
	double x,y,z;
	double toReturn = 0.0;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+loopLength;
		loc++;
		while (loc<until) {
			x = pathData(0,loc+1)-pathData(0,loc);
			y = pathData(1,loc+1)-pathData(1,loc);
			z = pathData(2,loc+1)-pathData(2,loc);
			toReturn += sqrt(x*x+y*y+z*z);
			loc++;
		}
		loc++;
    }

	return toReturn;
}

DTPath3D SegmentForm(const DTPath3D &P)
{
    if (P.IsEmpty() || P.StoredAsSegments()) {
        return P;
    }
    
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    int len = (int)pathData.n();
    int loopLength;
    int howManySegments = 0;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        loc += 1+loopLength;
        howManySegments += loopLength-1;
    }
    
    DTMutableDoubleArray toReturn(4,howManySegments);
    loc = 0;
    int i,until;
    int posInSegments = 0;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+loopLength;
        for (i=loc+1;i<until;i++) {
            toReturn(0,posInSegments) = pathData(0,i);
            toReturn(1,posInSegments) = pathData(1,i);
            toReturn(2,posInSegments) = pathData(2,i);
            toReturn(3,posInSegments) = pathData(0,i+1);
            toReturn(4,posInSegments) = pathData(1,i+1);
            toReturn(5,posInSegments) = pathData(2,i+1);
            posInSegments++;
        }
        loc += 1+loopLength;
        howManySegments += loopLength-1;
    }
    
    return DTPath3D(toReturn);
}

class DTPath3DPointAndIndex {
public:
    DTPath3DPointAndIndex() {}
    DTPath3DPointAndIndex(double xv,double yv,double zv,int ind) : x(xv), y(yv), z(zv), index(ind) {}
    
    // Dictionary sort.  That is first compare the y coordinate, and if they are the same, compare the x.
    int operator<(const DTPath3DPointAndIndex &C) const {return (z<C.z || (z==C.z && y<C.y) || (z==C.z && y==C.y && x<C.x));}
    int operator!=(const DTPath3DPointAndIndex &C) const {return (x!=C.x || y!=C.y || z!=C.z);}
    
    double x,y,z;
    int index;
};

DTPath3D LoopForm(const DTPath3D &P)
{
    if (P.StoredAsLoops()) return P;
    
    DTDoubleArray segments = P.Data();
    int numberOfSegments = (int)segments.n();
    DTMutableList<DTPath3DPointAndIndex> TempPointAndIndex(numberOfSegments*2);
    
    // Sort the points using a dictionary sort.
    int i;
    int numberOfPoints = 0;
    for (i=0;i<numberOfSegments;i++) {
        if (segments(0,i)!=segments(2,i) || segments(1,i)!=segments(3,i)) {
            TempPointAndIndex(numberOfPoints) = DTPath3DPointAndIndex(segments(0,i),segments(1,i),segments(2,i),numberOfPoints);
            numberOfPoints++;
            TempPointAndIndex(numberOfPoints) = DTPath3DPointAndIndex(segments(3,i),segments(4,i),segments(5,i),numberOfPoints);
            numberOfPoints++;
        }
    }
    numberOfSegments = numberOfPoints/2; // In case there was an empty line segment
    if (numberOfPoints==0) return DTPath3D(); // Only empty segments.
    std::sort(TempPointAndIndex.Pointer(),TempPointAndIndex.Pointer()+numberOfPoints);
    
    // Now create a 2 x numberOfSegments array of indices into this list.
    DTMutableIntArray segmentOffsets(2,numberOfSegments);
    segmentOffsets = -1;
    DTMutableDoubleArray points(3,numberOfPoints);
    int compareTo = 0;
    int numberOfCompareToPoint = 0;
    points(0,numberOfCompareToPoint) = TempPointAndIndex(0).x;
    points(1,numberOfCompareToPoint) = TempPointAndIndex(0).y;
	points(2,numberOfCompareToPoint) = TempPointAndIndex(0).z;
	segmentOffsets(TempPointAndIndex(0).index) = numberOfCompareToPoint;
    for (i=1;i<numberOfPoints;i++) {
        if (TempPointAndIndex(compareTo)!=TempPointAndIndex(i)) {
            // There was a change
            compareTo = i;
            numberOfCompareToPoint++;
            points(0,numberOfCompareToPoint) = TempPointAndIndex(i).x;
            points(1,numberOfCompareToPoint) = TempPointAndIndex(i).y;
            points(2,numberOfCompareToPoint) = TempPointAndIndex(i).z;
            segmentOffsets(TempPointAndIndex(i).index) = numberOfCompareToPoint;
        }
        else { // Same point as last.
            segmentOffsets(TempPointAndIndex(i).index) = numberOfCompareToPoint;
        }
    }
    
    // Tie together the index list, ignoring the points for a moment.
    DTIntArray loops = ConnectSegmentIndices(segmentOffsets);
    
    // Reconnect the points
    int len = (int)loops.Length();
    DTMutableDoubleArray loopData(3,len);
    int loc = 0;
    int ptN,loopLength,loopStarts,loopEnds;
    while (loc<len) {
        loopLength = loops(loc);
        loopStarts = loc+1;
        loopEnds = loc+1+loopLength;
        loopData(0,loc) = 0;
        loopData(1,loc) = 0;
        loopData(2,loc) = loopLength;
        for (ptN=loopStarts;ptN<loopEnds;ptN++) {
            loopData(0,ptN) = points(0,loops(ptN));
            loopData(1,ptN) = points(1,loops(ptN));
            loopData(2,ptN) = points(2,loops(ptN));
        }
        loc = loopEnds; // Prepare for the next loop.
    }
    
    return DTPath3D(loopData);
}

bool operator==(const DTPath3D &A,const DTPath3D &B)
{
    return (A.Data()==B.Data());
}

bool operator!=(const DTPath3D &A,const DTPath3D &B)
{
    return (A.Data()!=B.Data());
}

DTPath3D operator+(const DTPath3D &A,const DTPath3D &B)
{
    if (A.IsEmpty()) return B;
    if (B.IsEmpty()) return A;
    
    DTDoubleArray AD = A.Data();
    DTDoubleArray BD = B.Data();
    
    if (A.StoredAsSegments()!=B.StoredAsSegments()) {
        DTErrorMessage("Path3D+Path3D","Both have to be stored as segments or loops.");
        return DTPath3D();
    }
    
    // Just combine the columns. Ignore the cropping path.
    return DTPath3D(CombineColumns(AD,BD));
}


DTRegion3D BoundingBox(const DTPath3D &P)
{
    // This is not a very efficient method to finding the bounding box, but
    // gives a template for how the loop data structure can be parsed.

    if (P.IsEmpty())
        return DTRegion3D();
    DTDoubleArray D = P.Data();

#if defined(WIN32) && !defined(INFINITY)
#define INFINITY std::numeric_limits<float>::infinity();
#endif
    double xmin,ymin,zmin,xmax,ymax,zmax;
    xmin = ymin = zmin = INFINITY;
    xmax = ymax = zmax = -INFINITY;
    
    double x,y,z;
    int i;

    if (P.StoredAsSegments()) {
        // Line segments.  Access it as a 3x? array
        int len = (int)D.Length();
        for (i=0;i<len;i+=3) {
            x = D(i);
            y = D(i+1);
            z = D(i+2);
            xmin = (x < xmin ? x : xmin);
            xmax = (xmax < x ? x : xmax);
            ymin = (y < ymin ? y : ymin);
            ymax = (ymax < y ? y : ymax);
            zmin = (z < zmin ? z : zmin);
            zmax = (zmax < z ? z : zmax);
        }
    }
    else {
        // Packed.
        int length = (int)D.n();
        int until;
        int loc = 0;
        while (loc<length) {
            until = loc+1+int(D(2,loc));
            for (i=loc+1;i<until;i++) {
                x = D(0,i);
                y = D(1,i);
                z = D(2,i);
                // min/max is not standard.  This will ignore NaN numbers.
                xmin = (x < xmin ? x : xmin);
                xmax = (xmax < x ? x : xmax);
                ymin = (y < ymin ? y : ymin);
                ymax = (ymax < y ? y : ymax);
                zmin = (z < zmin ? z : zmin);
                zmax = (zmax < z ? z : zmax);
            }
            loc = until;
        }
    }

    DTRegion3D toReturn;
    if (xmin<=xmax && ymin<=ymax && zmin<=zmax)
        toReturn = DTRegion3D(DTPoint3D(xmin,ymin,zmin),DTPoint3D(xmax,ymax,zmax));
    
    return toReturn;
}

DTPoint3D FirstPoint(const DTPath3D &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("FirstPoint(DTPath3D)","Path is empty");
		return DTPoint3D(NAN,NAN,NAN);
	}

	DTDoubleArray Data = P.Data();
	if (P.StoredAsLoops()) {
		return DTPoint3D(Data(0,1),Data(1,1),Data(2,1));
	}
	else {
		return DTPoint3D(Data(0),Data(1),Data(2));
	}
}

DTPoint3D LastPoint(const DTPath3D &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("LastPoint(DTPath3D)","Path is empty");
		return DTPoint3D(NAN,NAN,NAN);
	}
	
	DTDoubleArray Data = P.Data();
	int n = (int)Data.n();
	if (P.StoredAsLoops()) {
		return DTPoint3D(Data(0,n-1),Data(1,n-1),Data(2,n-1));
	}
	else {
		return DTPoint3D(Data(3,n),Data(4,n),Data(5,n));
	}
}

DTMutableDoubleArray Points(const DTPath3D &P)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("DTPath3D::Points","Stored as segments");
        return DTMutableDoubleArray();
    }
    
    int howManyPoints = 0;
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+1+loopLength;
        howManyPoints += loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1) && pathData(2,loc+1)==pathData(2,until-1))
            howManyPoints--;
        loc = until;
    }
    
    DTMutableDoubleArray toReturn(3,howManyPoints);
    
    loc = 0;
    int pos = 0;
    while (loc<len) {
        loopLength = int(pathData(2,loc));
        until = loc+1+loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1) && pathData(2,loc+1)==pathData(2,until-1)) {
            loopLength--;
        }
        std::memcpy(toReturn.Pointer()+pos,pathData.Pointer()+3*(loc+1),loopLength*3*sizeof(double));
        pos += loopLength*3;
        loc = until;
    }
    
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTPath3D &toReturn)
{
    std::string theName = input.ResolveName(name);

    // The main array stores the loop structure
    DTDoubleArray loop = input.ReadDoubleArray(theName);
    std::string cropName = theName+"_loc";
    if (input.Contains(cropName)) {
        DTRegion3D crop;
        Read(input,theName+"_loc",crop);
        toReturn = DTPath3D(loop,crop);
    }
    else {
        toReturn = DTPath3D(loop);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPath3D &thePath)
{
    Write(output,name+"_bbox3D",BoundingBox(thePath));
    output.Save(thePath.Data(),name);
}

void WriteFloat(DTDataStorage &output,const std::string &name,const DTPath3D &thePath)
{
    Write(output,name+"_bbox3D",BoundingBox(thePath));
    output.Save(ConvertToFloat(thePath.Data()),name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPath3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Path");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPath3D &toReturn,DTPath3D_SaveInfo &SaveInfo)
{
    std::string theName = input.ResolveName(name);
    if (SaveInfo.name==theName) {
        toReturn = SaveInfo.path; // Already read this grid in.
        return;
    }
    
    Read(input,theName,toReturn);
    
    SaveInfo.path = toReturn;
    SaveInfo.name = theName;
}

void Write(DTDataStorage &output,const std::string &name,const DTPath3D &toWrite,DTPath3D_SaveInfo &SaveInfo)
{
    if (SaveInfo.name.length() && SaveInfo.path==toWrite) {
        // Just save the reference.
        Write(output,name,SaveInfo.name);
    }
    else {
        Write(output,name,toWrite);
        SaveInfo.path = toWrite;
        SaveInfo.name = name;
    }
}

