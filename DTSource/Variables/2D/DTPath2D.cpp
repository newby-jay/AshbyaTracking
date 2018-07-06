// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPath2D.h"

#include "DTError.h"
#include "DTDataStorage.h"
#include "DTUtilities.h"
#include "DTIntArray.h"
#include "DTDataStorage.h"

#include <algorithm>
#include <math.h>
#include <limits>
#include <cstring>

DTPath2D::DTPath2D(const DTDoubleArray &input)
{
    SetStorage(input);
}

DTPath2D::DTPath2D(const DTDoubleArray &input,DTRegion2D crop)
{
    if (SetStorage(input))
        _cropBox = crop;
}

DTPath2D::DTPath2D(const DTIntArray &connections,const DTDoubleArray &points)
{
    if (connections.IsEmpty()) return;
    
    if (connections.n()!=1 || connections.o()!=1 || points.m()!=2 || points.o()!=1) {
        DTErrorMessage("DTPath(connections,points)","Invalid input arrays");
        return;
    }
    
    DTMutableDoubleArray theData(2,connections.m());
    ssize_t loc=0,length,until = connections.m();
    bool failed = false;
    while (loc<until) {
        length = connections(loc);
        if (length<2 || loc+length>=until) {
            failed = true;
            break;
        }
        theData(0,loc) = 0.0;
        theData(1,loc) = length;
        loc++;
        until = loc+length;
        while (loc<until) {
            theData(0,loc) = points(0,connections(loc));
            theData(1,loc) = points(1,connections(loc));
            loc++;
        }
    }
    if (failed) {
        DTErrorMessage("DTPath(connections,points)","Invalid input format");
        return;
    }
    
    _data = theData;
}

bool DTPath2D::SetStorage(const DTDoubleArray &input)
{
    if  (input.IsEmpty()) return false;

    // Check dimension.
    if ((input.m()!=2 && input.m()!=4) || input.o()!=1) {
        DTErrorMessage("DTPath(Array)","Invalid size of path array.");
        return false;
    }

    // Check to see that the loop format is correct.
    if (input.m()==2) {
        ssize_t loc = 0;
        ssize_t length = input.n();
        ssize_t loopLength;
        while (loc<length) {
            if (input(1,loc)!=floor(input(1,loc)) || input(1,loc)<=1) {
                DTErrorMessage("DTPath(Array)","Invalid loop length in column "+DTInt2String(int(loc))+".");
                break;
            }
            loopLength = int(input(1,loc));
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

int DTPath2D::NumberOfPoints(void) const
{
	if (IsEmpty()) return 0;
    if (StoredAsSegments()) {
        DTErrorMessage("DTPath2D::NumberOfPoints","Stored as segments");
        return 0;
    }
    
    int toReturn = 0;
    DTDoubleArray pathData = Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        toReturn += loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1))
            toReturn--;
        loc = until;
    }

    return toReturn;
}

bool DTPath2D::IsClosed(void) const
{
	if (StoredAsSegments()) {
		DTErrorMessage("DTPath2D::IsClosed","Stored as segments, convert to loop form");
        return 0;
	}
	
	int toReturn = 0;
    DTDoubleArray pathData = Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        toReturn += loopLength;
        if (pathData(0,loc+1)!=pathData(0,until-1) || pathData(1,loc+1)!=pathData(1,until-1))
			break;
        loc = until;
    }
	
	return (loc==len);
}

int DTPath2D::NumberOfLoops(void) const
{
    if (StoredAsLoops()==false) {
        DTErrorMessage("DTPath2D::NumberOfLoops","Not stored as loops");
        return 0;
    }
    
    int toReturn = 0;
    DTDoubleArray pathData = Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        loc += 1+loopLength;
        toReturn++;
    }
    
    return toReturn;
}

void DTPath2D::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty Path";
    }
    else if (StoredAsSegments()) {
        if (_data.n()==1) {
            std::cerr << "one segment";
        }
        else {
            std::cerr << _data.n() << " segments";
        }
    }
    else if (NumberOfLoops()==1) {
        std::cerr << NumberOfPoints() << " points, 1 loop";
    }
    else {
        std::cerr << NumberOfPoints() << " points, " << NumberOfLoops() << " loops";
    }
    
    if (_cropBox.isSet) {
        std::cerr << ", crop = [" << _cropBox.xmin << ", " << _cropBox.xmax << "] x [" << _cropBox.ymin << ", " << _cropBox.ymax << "]\n";
    }
    else {
        std::cerr << "\n";
    }
    std::cerr << flush;
}

void DTPath2D::pall(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n";
    }
    else if (StoredAsSegments()) {
        _data.pall();
    }
    else {
        if (NumberOfLoops()==1) {
            DTMutableDoubleArray temp(2,_data.n()-1);
            std::memcpy(temp.Pointer(),_data.Pointer()+2,temp.Length()*sizeof(double));
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

bool operator==(const DTPath2D &A,const DTPath2D &B)
{
    return (A.Data()==B.Data());
}

bool operator!=(const DTPath2D &A,const DTPath2D &B)
{
    return (A.Data()!=B.Data());
}

DTPoint2D FirstPoint(const DTPath2D &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("FirstPoint(DTPath2D)","Path is empty");
		return DTPoint2D(NAN,NAN);
	}
	
	DTDoubleArray Data = P.Data();
	if (P.StoredAsLoops()) {
		return DTPoint2D(Data(0,1),Data(1,1));
	}
	else {
		return DTPoint2D(Data(0),Data(1));
	}
}

DTPoint2D LastPoint(const DTPath2D &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("LastPoint(DTPath2D)","Path is empty");
		return DTPoint2D(NAN,NAN);
	}
	
	DTDoubleArray Data = P.Data();
	ssize_t n = Data.n();
	if (P.StoredAsLoops()) {
		return DTPoint2D(Data(0,n-1),Data(1,n-1));
	}
	else {
		return DTPoint2D(Data(2,n),Data(3,n));
	}
}

DTRegion2D BoundingBox(const DTPath2D &P)
{
    // This is not a very efficient method to finding the bounding box, but
    // gives a template for how the loop data structure can be parsed.

    if (P.IsEmpty())
        return DTRegion2D();
    DTDoubleArray D = P.Data();

    double xmin,ymin,xmax,ymax;
#if defined(WIN32) && !defined(INFINITY)
#define INFINITY std::numeric_limits<float>::infinity();
#endif
    xmin = ymin = INFINITY;
    xmax = ymax = -INFINITY;

    double x,y;
    int i;

    if (P.StoredAsSegments()) {
        // Line segments.  Access it as a 2x? array
        ssize_t len = D.Length();
        for (i=0;i<len;i+=2) {
            x = D(i);
            y = D(i+1);
            xmin = (x < xmin ? x : xmin);
            xmax = (xmax < x ? x : xmax);
            ymin = (y < ymin ? y : ymin);
            ymax = (ymax < y ? y : ymax);
        }
    }
    else {
        // Packed.
        ssize_t length = D.n();
        int until;
        int loc = 0;
        while (loc<length) {
            until = loc+1+int(D(1,loc));
            for (i=loc+1;i<until;i++) {
                x = D(0,i);
                y = D(1,i);
                // min/max is not standard.  This will ignore NaN numbers.
                xmin = (x < xmin ? x : xmin);
                xmax = (xmax < x ? x : xmax);
                ymin = (y < ymin ? y : ymin);
                ymax = (ymax < y ? y : ymax);
            }
            loc = until;
        }
    }

    DTRegion2D toReturn;
    if (xmin<=xmax && ymin<=ymax)
        toReturn = DTRegion2D(DTPoint2D(xmin,ymin),DTPoint2D(xmax,ymax));
    
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTPath2D &toReturn)
{
    // The main array stores the loop structure
    std::string theName = input.ResolveName(name);
    DTDoubleArray loop = input.ReadDoubleArray(theName);
    std::string cropName = theName+"_loc";
    if (input.Contains(cropName)) {
        DTRegion2D crop;
        Read(input,theName+"_loc",crop);
        toReturn = DTPath2D(loop,crop);
    }
    else {
        toReturn = DTPath2D(loop);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPath2D &thePath)
{
    Write(output,name+"_bbox2D",BoundingBox(thePath));
    
    Write(output,name,thePath.Data());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPath2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Path");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPath2D &toReturn,DTPath2D_SaveInfo &SaveInfo)
{
    std::string theName = input.ResolveName(name);
    if (SaveInfo.name==theName) {
        toReturn = SaveInfo.path;
        return;
    }
    
    Read(input,theName,toReturn);
    
    SaveInfo.path = toReturn;
    SaveInfo.name = theName;
}

void Write(DTDataStorage &output,const std::string &name,const DTPath2D &toWrite,DTPath2D_SaveInfo &SaveInfo)
{
    if (SaveInfo.name.length() && SaveInfo.path==toWrite) {
        Write(output,name,SaveInfo.name);
    }
    else {
        Write(output,name,toWrite);
        SaveInfo.path = toWrite;
        SaveInfo.name = name;
    }
}

#pragma mark External Functions

DTPath2D ExtractLoop(const DTPath2D &P,int i)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("ExtractLoop(DTPath2D,int)","Stored as segments");
        return DTPath2D();
    }
    
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    int until = 0;
    ssize_t len = pathData.n();
    int loopLength;
    int thisIsLoop = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        if (thisIsLoop==i)
            break;
        loc = until;
        thisIsLoop++;
    }
    
    if (loc==len) {
        DTErrorMessage("ExtractLoop(DTPath2D,int)","Index out of bounds");
        return DTPath2D();
    }
    
    DTMutableDoubleArray section(2,until-loc);
    std::memcpy(section.Pointer(),pathData.Pointer()+2*loc,section.Length()*sizeof(double));
    
    DTPath2D toReturn(section,P.CroppingBox());
    
    return toReturn;
}

DTPath2D ChangeOrientation(const DTPath2D &P)
{
	if (P.IsEmpty()) return P;
	DTDoubleArray data = P.Data();
	ssize_t loc=0,i,len,length = data.n();
	if (data.m()==2) {
		DTMutableDoubleArray newData(2,length);
		while (loc<length) {
			len = int(data(1,loc));
			newData(0,loc) = 0.0;
			newData(1,loc) = len;
			for (i=0;i<len;i++) {
				newData(0,loc+1+i) = data(0,loc+len-i);
				newData(1,loc+1+i) = data(1,loc+len-i);
			}
			loc += len+1;
		}
		return DTPath2D(newData);
	}
	else {
		DTMutableDoubleArray newSegments(4,length);
		for (loc=0;loc<length;loc++) {
			newSegments(0,loc) = data(2,loc);
			newSegments(1,loc) = data(3,loc);
			newSegments(2,loc) = data(0,loc);
			newSegments(3,loc) = data(1,loc);
		}
		return DTPath2D(newSegments);
	}
}

DTMutableDoubleArray Points(const DTPath2D &P)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("DTPath2D::Points","Stored as segments");
        return DTMutableDoubleArray();
    }
    
    int howManyPoints = 0;
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        howManyPoints += loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1))
            howManyPoints--;
        loc = until;
    }
    
    DTMutableDoubleArray toReturn(2,howManyPoints);
    
    loc = 0;
    int pos = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1)) {
            loopLength--;
        }
        std::memcpy(toReturn.Pointer()+pos,pathData.Pointer()+2*(loc+1),loopLength*2*sizeof(double));
        pos += loopLength*2;
        loc = until;
    }
    
    return toReturn;
}

DTPath2D Combine(const DTList<DTPath2D> &List)
{
	ssize_t i,howMany = List.Length();
	int totalLength = 0;
	if (howMany==0) return DTPath2D();
	if (howMany==1) return List(0);
	i = 0;
	while (i<howMany && List(i).IsEmpty()) i++;
	ssize_t firstIndex = i;
	if (firstIndex==howMany) return DTPath2D();
	bool typeOfFirst = List(i).StoredAsLoops();
	while (i<howMany) {
		if (typeOfFirst!=List(i).StoredAsLoops())
			break;
		totalLength += List(i).Data().n();
		i++;
	}
	if (i<howMany) {
		DTErrorMessage("Combine(List of DTPath2D)", "All the paths have to have the same type (Loops or Segments)");
		return DTPath2D();
	}
	DTMutableDoubleArray combinedList(List(firstIndex).Data().m(),totalLength);
	int posInCombined = 0;
	DTDoubleArray array;
	for (i=firstIndex;i<howMany;i++) {
		array = List(i).Data();
		std::memcpy(combinedList.Pointer()+posInCombined,array.Pointer(),array.Length()*sizeof(double));
		posInCombined+=array.Length();
	}
	
	return DTPath2D(combinedList);
}

DTMutableList<DTPath2D> Separate(const DTPath2D &p)
{
    DTPath2D splitThis = p;
    if (p.IsEmpty()) {
        return DTMutableList<DTPath2D>();
    }
    if (p.StoredAsLoops()==false) {
        splitThis = LoopForm(p);
        if (splitThis.IsEmpty()) return DTMutableList<DTPath2D>();
    }

    int howMany = splitThis.NumberOfLoops();
    DTMutableList<DTPath2D> toReturn(howMany);
    int posInReturn = 0;
    
    DTDoubleArray pathData = splitThis.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        toReturn(posInReturn++) = DTPath2D(ExtractColumns(pathData,DTRange(loc,loopLength+1)));
        loc += 1+loopLength;
    }
    
    return toReturn;
}

DTMutableIntArray SegmentsAsOffsets(const DTPath2D &P)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("DTPath2D::SegmentsAsOffsets","Stored as segments");
        return DTMutableIntArray();
    }
    
    int howManyPoints = 0;
    int howManySegments = 0;
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        howManyPoints += loopLength;
        howManySegments += loopLength-1;
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1))
            howManyPoints--;
        loc = until;
    }
    
    DTMutableIntArray toReturn(2,howManySegments);
    
    loc = 0;
    int pos = 0;
    int i;
    int pointNumber = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+1+loopLength;
        for (i=loc+1;i<until-1;i++) {
            toReturn(0,pos) = pointNumber;
            toReturn(1,pos) = pointNumber+1;
            pos++;
            pointNumber++;
        }
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1)) {
            // Last point is the same as the first point.
            toReturn(1,pos-1) = pointNumber-loopLength+1;
        }
        loc = until;
    }
    
    return toReturn;
}

DTMutableIntArray PointOffsets(const DTPath2D &P)
{
    if (P.StoredAsSegments()) {
        DTErrorMessage("DTPath2D::Points","Stored as segments");
        return DTMutableIntArray();
    }
    
    DTDoubleArray pathData = P.Data();
    ssize_t len = pathData.n();
    int loopLength,until;
    
    DTMutableIntArray toReturn(len);
    
    int loc = 0;
    int i,numberOfFirstPoint,ptNumber = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        toReturn(loc) = loopLength;
        until = loc+1+loopLength;
        numberOfFirstPoint = ptNumber;
        for (i=loc+1;i<until-1;i++) {
            toReturn(i) = ptNumber++;
        }
        if (pathData(0,loc+1)==pathData(0,until-1) && pathData(1,loc+1)==pathData(1,until-1)) {
            toReturn(i) = numberOfFirstPoint;
        }
        else {
            toReturn(i) = ptNumber++;
        }
        loc = until;
    }
    
    return toReturn;
}

DTPath2D SegmentForm(const DTPath2D &P)
{
    if (P.IsEmpty() || P.StoredAsSegments()) {
        return P;
    }
    
    DTDoubleArray pathData = P.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength;
    int howManySegments = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        loc += 1+loopLength;
        howManySegments += loopLength-1;
    }
    
    DTMutableDoubleArray toReturn(4,howManySegments);
    loc = 0;
    int i,until;
    int posInSegments = 0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+loopLength;
        for (i=loc+1;i<until;i++) {
            toReturn(0,posInSegments) = pathData(0,i);
            toReturn(1,posInSegments) = pathData(1,i);
            toReturn(2,posInSegments) = pathData(0,i+1);
            toReturn(3,posInSegments) = pathData(1,i+1);
            posInSegments++;
        }
        loc += 1+loopLength;
        howManySegments += loopLength-1;
    }
    
    return DTPath2D(toReturn,P.CroppingBox());
}

double Length(const DTPath2D &P)
{
	DTDoubleArray pathData = P.Data();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength,until;
	double x,y;
	double toReturn = 0.0;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+loopLength;
		loc++;
		while (loc<until) {
			x = pathData(0,loc+1)-pathData(0,loc);
			y = pathData(1,loc+1)-pathData(1,loc);
			toReturn += sqrt(x*x+y*y);
			loc++;
		}
		loc++;
    }
	
	return toReturn;
}

double Area(const DTPath2D &path)
{
    if (path.StoredAsLoops()==false) {
        DTErrorMessage("Area(Path2D)","Not stored as loops");
        return 0.0;
    }
    
    DTDoubleArray loops = path.Data();
    ssize_t loc = 0;
    ssize_t len = loops.n();
    ssize_t ptN,loopLength,loopStarts,loopEnds;
    
    double Ax,Ay,Bx,By;
    double areaIntegral = 0;
    
    while (loc<len) {
        loopLength = int(loops(1,loc));
        loopStarts = loc+1;
        loopEnds = loc+1+loopLength;
        
        if (loops(0,loopStarts)==loops(0,loopEnds-1) && loops(1,loopStarts)==loops(1,loopEnds-1)) {
            // Otherwise skip over this
            
            for (ptN=loopStarts;ptN<loopEnds-1;ptN++) {
                // x = loops(0,ptN);
                // y = loops(1,ptN);
                
                Ax = loops(0,ptN);
                Ay = loops(1,ptN);
                Bx = loops(0,ptN+1);
                By = loops(1,ptN+1);
                
                // Calculate the integral from A to B.  The normal is given by (C.y,-C.x)/length(C)
                // Where C = B-A.  The length of the interval is equal to length(C).
                // The contribution from the interval is \int_0^1 (A+tC)\cdot C^r dt
                // where C^r is C rotated 90 degrees (C.y,-C.x).
                // This integral is equal to A\cdot C^r.
                areaIntegral += Ax*(By-Ay)-Ay*(Bx-Ax);
            }
        }
        else {
            areaIntegral = NAN;
            break;
        }
        loc = loopEnds; // Prepare for the next loop.
    }

    return areaIntegral*0.5;
}

DTMutableDoubleArray Areas(const DTPath2D &path)
{
    if (path.StoredAsLoops()==false) {
        DTErrorMessage("Area(Path2D)","Not stored as loops");
        return DTMutableDoubleArray();
    }
    
    int howMany = path.NumberOfLoops();
    DTMutableDoubleArray toReturn(howMany);
    int posInReturn = 0;
    
    DTDoubleArray loops = path.Data();
    int loc = 0;
    int len = loops.n();
    int ptN,loopLength,loopStarts,loopEnds;
    
    double Ax,Ay,Bx,By;
    double areaIntegral = 0;
    
    while (loc<len) {
        loopLength = int(loops(1,loc));
        loopStarts = loc+1;
        loopEnds = loc+1+loopLength;
        
        areaIntegral = 0;
        if (loops(0,loopStarts)==loops(0,loopEnds-1) && loops(1,loopStarts)==loops(1,loopEnds-1)) {
            
            for (ptN=loopStarts;ptN<loopEnds-1;ptN++) {
                // x = loops(0,ptN);
                // y = loops(1,ptN);
                
                Ax = loops(0,ptN);
                Ay = loops(1,ptN);
                Bx = loops(0,ptN+1);
                By = loops(1,ptN+1);
                
                // Calculate the integral from A to B.  The normal is given by (C.y,-C.x)/length(C)
                // Where C = B-A.  The length of the interval is equal to length(C).
                // The contribution from the interval is \int_0^1 (A+tC)\cdot C^r dt
                // where C^r is C rotated 90 degrees (C.y,-C.x).
                // This integral is equal to A\cdot C^r.
                areaIntegral += Ax*(By-Ay)-Ay*(Bx-Ax);
            }
        }
        else {
            areaIntegral = NAN;
        }
        toReturn(posInReturn++) = areaIntegral*0.5;
        loc = loopEnds; // Prepare for the next loop.
    }
    
    return toReturn;
}

struct DTPairSortedByFirst {
    int operator<(const DTPairSortedByFirst &C) const {return (first<C.first);}
    int first,second;
};

DTIntArray ConnectSegmentIndices(const DTIntArray &segments)
{
    if (segments.m()!=2 || segments.o()!=1)
        return DTIntArray();
    
    int i;
    DTMutableIntArray segmentOffsets = segments.Copy();
    int *segmentOffsetsD = segmentOffsets.Pointer();
    int numberOfSegments = (int)segmentOffsets.n();
    
    // Now tie together the segments.
    // Begin by sorting the segment array, sorted by the first point in the segment

    // Sort the array by first index.
    const int SegmentNotDone = numberOfSegments+3;
    const int SegmentIsDone = 1;

    DTMutableIntArray ConnectionsArray(numberOfSegments*3); // Maximum possible size, no connections.
        
    // Sort the segmentOffsets by the first element.
    sort((DTPairSortedByFirst *)segmentOffsets.Pointer(),((DTPairSortedByFirst *) segmentOffsets.Pointer())+numberOfSegments);

    // Create the SegmentInformation array.
    DTMutableIntArray SegmentInformation(numberOfSegments);
    SegmentInformation = SegmentNotDone;
    //for (i=0;i<numberOfSegments;i++)
    //    SegmentInformation(i) = SegmentNotDone;

    // Create the EndingIndices array.
    DTMutableIntArray EndingIndices(2,numberOfSegments);
    int *EndingIndicesD = EndingIndices.Pointer();
    for (i=0;i<numberOfSegments;i++) {
    	EndingIndicesD[  2*i] = segmentOffsetsD[1+2*i];
    	EndingIndicesD[1+2*i] = i;
    }
    sort((DTPairSortedByFirst *)EndingIndices.Pointer(),((DTPairSortedByFirst *) EndingIndices.Pointer())+numberOfSegments);
    
    int PositionInConnections = 0; // where things are being added to the connections list.
    int StartOfThisLoop; // Where the size information should be put.
    
    int currentlyLookingAtSegment = 0;
    int lookForIndex, startFlip,howManyToFlip,tempInt, locatedAbove, locatedBelow, halfpoint;

    while (true) {
    	StartOfThisLoop = PositionInConnections;
    	PositionInConnections++;
    	
    	// Put the first segment in, backwards.
    	ConnectionsArray(PositionInConnections  ) = segmentOffsets(currentlyLookingAtSegment*2+1);
        ConnectionsArray(PositionInConnections+1) = lookForIndex = segmentOffsets(currentlyLookingAtSegment*2);
    	PositionInConnections+=2;
        SegmentInformation(currentlyLookingAtSegment) = SegmentIsDone;
    	
    	// Now keep looking backwards through the list, and filling in the connections array.
    	while (true) {
    		locatedAbove = 0;
    		locatedBelow = numberOfSegments;
    		while (locatedAbove+1<locatedBelow) {
    			halfpoint = (locatedAbove+locatedBelow)/2;
    			if (lookForIndex<EndingIndicesD[2*halfpoint])
                    locatedBelow = halfpoint;
    			else
                    locatedAbove = halfpoint;
    		}
            if (lookForIndex!=EndingIndicesD[2*locatedAbove])
                break; // Did not find it.
            
            tempInt = locatedAbove;
            while (locatedAbove<numberOfSegments && lookForIndex==EndingIndicesD[2*locatedAbove] && SegmentInformation(EndingIndicesD[2*locatedAbove+1])==SegmentIsDone)
                locatedAbove++;
    		
            if (locatedAbove==numberOfSegments || lookForIndex!=EndingIndices(2*locatedAbove)) {
                locatedAbove = tempInt-1;
                while (locatedAbove>=0 && lookForIndex==EndingIndices(2*locatedAbove) && SegmentInformation(EndingIndices(2*locatedAbove+1))==SegmentIsDone)
                    locatedAbove--;
                if (locatedAbove<=0 || lookForIndex!=EndingIndices(2*locatedAbove)) {
                    locatedAbove = 0;
                    break;
                }
            }
            
            // Found the next segment.  Add the starting point to ConnectionsArray.
            SegmentInformation(EndingIndices(2*locatedAbove+1)) = SegmentIsDone;
            ConnectionsArray(PositionInConnections++) = lookForIndex = segmentOffsets(2*EndingIndices(2*locatedAbove+1));
    	}
    	
    	// Take the point list that has accumulated, and flip it.
    	startFlip = StartOfThisLoop+1;
    	howManyToFlip = (PositionInConnections-startFlip)/2;
    	for (i=0;i<howManyToFlip;i++) {
    		tempInt = ConnectionsArray(startFlip+i);
    		ConnectionsArray(startFlip+i) = ConnectionsArray(PositionInConnections-1-i);
    		ConnectionsArray(PositionInConnections-i-1) = tempInt;
    	}
    	
    	// Trace forwards through the list until we stop.
    	lookForIndex = ConnectionsArray(PositionInConnections-1);
    	while (true) {
    		locatedAbove = 0;
    		locatedBelow = numberOfSegments;
            while (locatedAbove+1<locatedBelow) {
                halfpoint = (locatedAbove+locatedBelow)/2;
                if (lookForIndex<segmentOffsets(2*halfpoint))
                    locatedBelow = halfpoint;
                else
                    locatedAbove= halfpoint;
    		}
    		
            if (lookForIndex!=segmentOffsets(2*locatedAbove))
                break; // Did not find it.
            tempInt = locatedAbove;
            while (locatedAbove<numberOfSegments && lookForIndex==segmentOffsets(2*locatedAbove) && SegmentInformation(locatedAbove)==SegmentIsDone)
                locatedAbove++;
            if (locatedAbove==numberOfSegments || lookForIndex!=segmentOffsets(2*locatedAbove)) {
                locatedAbove = tempInt-1;
                while (locatedAbove>=0 && lookForIndex==segmentOffsets(2*locatedAbove) && SegmentInformation(locatedAbove)==SegmentIsDone)
                    locatedAbove--;
                if (locatedAbove<=0 || lookForIndex!=segmentOffsets(2*locatedAbove)) {
                    locatedAbove = 0;
                    break;
                }
            }
    		// Found the next segment.  Add the starting point to ConnectionsArray.
            SegmentInformation(locatedAbove)=SegmentIsDone;
            ConnectionsArray(PositionInConnections++) = lookForIndex = segmentOffsets(2*locatedAbove+1);
    	}
    	
    	// Fill in the length (finishes the loop)
    	ConnectionsArray(StartOfThisLoop) = PositionInConnections - StartOfThisLoop -1;

    	// Find the new currentlyLookingAtSegment (or exit if we can't find it).
    	while (currentlyLookingAtSegment<numberOfSegments &&
               SegmentInformation(currentlyLookingAtSegment)==SegmentIsDone)
    		currentlyLookingAtSegment++;
    	
    	if (currentlyLookingAtSegment==numberOfSegments)
            break;
    }
    
    ConnectionsArray = TruncateSize(ConnectionsArray,PositionInConnections);
    
    return ConnectionsArray;
}

class DTPath2DPointAndIndex {
public:
    DTPath2DPointAndIndex() {}
    DTPath2DPointAndIndex(double xv,double yv,int ind) : x(xv), y(yv), index(ind) {}
    
    // Dictionary sort.  That is first compare the y coordinate, and if they are the same, compare the x.
    int operator<(const DTPath2DPointAndIndex &C) const {return (y<C.y || (y==C.y && x<C.x));}
    int operator!=(const DTPath2DPointAndIndex &C) const {return (x!=C.x || y!=C.y);}
    
    double x,y;
    int index;
};

DTPath2D LoopForm(const DTPath2D &P)
{
    if (P.StoredAsLoops()) return P;
    
    DTDoubleArray segments = P.Data();
    const double *segmentsD = segments.Pointer();
    ssize_t numberOfSegments = segments.n();
    DTMutableList<DTPath2DPointAndIndex> TempPointAndIndex(numberOfSegments*2);
    DTPath2DPointAndIndex *TempPointAndIndexD = TempPointAndIndex.Pointer();
    
    // Sort the points using a dictionary sort.
    int i;
    int numberOfPoints = 0;
    for (i=0;i<numberOfSegments;i++) {
        if (segmentsD[4*i]!=segmentsD[2+4*i] || segmentsD[1+4*i]!=segmentsD[3+4*i]) {
            TempPointAndIndexD[numberOfPoints] = DTPath2DPointAndIndex(segmentsD[4*i],segmentsD[1+4*i],numberOfPoints);
            numberOfPoints++;
            TempPointAndIndexD[numberOfPoints] = DTPath2DPointAndIndex(segmentsD[2+4*i],segmentsD[3+4*i],numberOfPoints);
            numberOfPoints++;
        }
    }
    numberOfSegments = numberOfPoints/2; // In case there was an empty line segment
    if (numberOfPoints==0) return DTPath2D(); // Only empty segments.
    sort(TempPointAndIndex.Pointer(),TempPointAndIndex.Pointer()+numberOfPoints);
    
    // Now create a 2 x numberOfSegments array of indices into this list.
    DTMutableIntArray segmentOffsets(2,numberOfSegments);
    int *segmentOffsetsD = segmentOffsets.Pointer();
    segmentOffsets = -1;
    DTMutableDoubleArray points(2,numberOfPoints);
    double *pointsD = points.Pointer();
    int compareTo = 0;
    int numberOfCompareToPoint = 0;
    points(0,numberOfCompareToPoint) = TempPointAndIndex(0).x;
    points(1,numberOfCompareToPoint) = TempPointAndIndex(0).y;
    segmentOffsets(TempPointAndIndex(0).index) = numberOfCompareToPoint;
    for (i=1;i<numberOfPoints;i++) {
        if (TempPointAndIndexD[compareTo]!=TempPointAndIndexD[i]) {
            // There was a change
            compareTo = i;
            numberOfCompareToPoint++;
            pointsD[  2*numberOfCompareToPoint] = TempPointAndIndexD[i].x; // points(0,numberOfCompareToPoint) = TempPointAndIndex(i).x;
            pointsD[1+2*numberOfCompareToPoint] = TempPointAndIndexD[i].y; // points(1,numberOfCompareToPoint) = TempPointAndIndex(i).y;
            segmentOffsetsD[TempPointAndIndexD[i].index] = numberOfCompareToPoint;
        }
        else { // Same point as last.
            segmentOffsetsD[TempPointAndIndexD[i].index] = numberOfCompareToPoint;
        }
    }
    
    // Tie together the index list, ignoring the points for a moment.
    DTIntArray loops = ConnectSegmentIndices(segmentOffsets);
    const int *loopsD = loops.Pointer();
    
    // Reconnect the points
    ssize_t len = loops.Length();
    DTMutableDoubleArray loopData(2,len);
    double *loopDataD = loopData.Pointer();
    int loc = 0;
    int ptN,loopLength,loopStarts,loopEnds;
    while (loc<len) {
        loopLength = loops(loc);
        loopStarts = loc+1;
        loopEnds = loc+1+loopLength;
        loopData(0,loc) = 0;
        loopData(1,loc) = loopLength;
        for (ptN=loopStarts;ptN<loopEnds;ptN++) {
            loopDataD[2*ptN] = pointsD[loopsD[ptN]*2]; // loopData(0,ptN) = points(0,loops(ptN));
            loopDataD[1+2*ptN] = pointsD[1+loopsD[ptN]*2]; // loopData(1,ptN) = points(1,loops(ptN));
        }
        loc = loopEnds; // Prepare for the next loop.
    }
    
    return DTPath2D(loopData);
}

DTPath2D operator+(const DTPath2D &A,const DTPath2D &B)
{
    if (A.IsEmpty()) return B;
    if (B.IsEmpty()) return A;
    
    DTDoubleArray AD = A.Data();
    DTDoubleArray BD = B.Data();
    
    if (A.StoredAsSegments()!=B.StoredAsSegments()) {
        DTErrorMessage("Path+Path","Both have to be stored as segments or loops.");
        return DTPath2D();
    }
    
    // Just combine the columns. Ignore the cropping path.
    return DTPath2D(CombineColumns(AD,BD));
}

DTPath2D CreatePath(const DTRegion2D &r)
{
    DTMutableDoubleArray loop(2,6);
    loop(0,0) = 0;
    loop(1,0) = 5;
    loop(0,1) = r.xmin;
    loop(1,1) = r.ymin;
    loop(0,2) = r.xmax;
    loop(1,2) = r.ymin;
    loop(0,3) = r.xmax;
    loop(1,3) = r.ymax;
    loop(0,4) = r.xmin;
    loop(1,4) = r.ymax;
    loop(0,5) = r.xmin;
    loop(1,5) = r.ymin;
    return DTPath2D(loop);
}

DTPath2D Circle(DTPoint2D center,double radius,int howMany)
{
    if (howMany<=1) return DTPath2D();
    
    DTMutableDoubleArray loop(2,howMany+2);
    double dtheta = M_PI*2/howMany;
    int i;
    loop(0,0) = 0;
    loop(1,0) = howMany+1;
    for (i=0;i<howMany;i++) {
        loop(0,i+1) = radius*cos(i*dtheta);
        loop(1,i+1) = radius*sin(i*dtheta);
    }
    loop(0,howMany-1) = loop(0,1);
    loop(1,howMany-1) = loop(1,1);
    
    return DTPath2D(loop);
}

DTPath2D SegmentsAsOffsetsAndPoints(const DTIntArray &offsets,const DTDoubleArray &pts)
{
    if (offsets.IsEmpty()) return DTPath2D();
    if (offsets.m()!=2 || offsets.o()!=1) {
        DTErrorMessage("SegmentsAsOffsetsAndPoints(offsets,points)","Invalid size for offset array.");
    }
    if (pts.m()!=2 || pts.o()!=1) {
        DTErrorMessage("SegmentsAsOffsetsAndPoints(offsets,points)","Invalid size for point array.");
    }

    ssize_t i,howMany = offsets.n();
    int offsA,offsB;
    DTMutableDoubleArray segments(4,howMany);
    segments = 0.0;
    ssize_t numPts = pts.n();
    for (i=0;i<howMany;i++) {
        offsA = offsets(0,i);
        offsB = offsets(1,i);
        if (offsA<0 || offsB<0 || offsA>=numPts || offsB>=numPts) continue;
        segments(0,i) = pts(0,offsA);
        segments(1,i) = pts(1,offsA);
        segments(2,i) = pts(0,offsB);
        segments(3,i) = pts(1,offsB);
    }
    
    return DTPath2D(segments);
}

