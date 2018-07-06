// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPath2DValues.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"
#include "DTPath2D.h"
#include "DTDoubleArrayOperators.h"

#include <math.h>
#include <limits>
#include <cstring>

DTPath2DValues::DTPath2DValues(const DTPath2D &pathIn,const DTDoubleArray &valuesIn)
{
    // Makes sure that this is a valid format.
    if (pathIn.IsEmpty())
        return;
    
    if (valuesIn.m()!=valuesIn.Length()) {
        DTErrorMessage("DTPath2DValues(path,values)","The value list has to be a vector, i.e. m x 1.");
        return;
    }

    int loc = 0;
    int loopLength;

    DTDoubleArray Data = pathIn.Data();
    ssize_t length = Data.n();
    if (Data.n()==valuesIn.m()) {
        // Should be packed the same way as the path.
        while (loc<length) {
            if (Data(1,loc)!=valuesIn(loc)) {
                DTErrorMessage("DTPath2DValues(path,values)","value list does not have the same loop structure as the path.");
                break;
            }
            loopLength = int(Data(1,loc));
            loc += loopLength+1;
        }
        
        if (loc<length)
            return;
    
        path = pathIn;
        values = valuesIn;
    }
    else if (valuesIn.m()==pathIn.NumberOfPoints()) {
        // One value per point.
        DTMutableDoubleArray valueList(length);
        int pos = 0;
        int until;
        while (loc<length) {
            valueList(loc) = Data(1,loc);
            loopLength = int(Data(1,loc));
            until = loc+1+loopLength;
            if (Data(0,loc+1)==Data(0,until-1) && Data(1,loc+1)==Data(1,until-1)) {
                loopLength--;
                valueList(loc+1+loopLength) = valuesIn(pos); // Repeated.
            }
            std::memcpy(valueList.Pointer()+loc+1,valuesIn.Pointer()+pos,loopLength*sizeof(double));
            pos += loopLength;
            loc = until;
        }
        
        path = pathIn;
        values = valueList;
    }
    else {
        DTErrorMessage("DTPath2DValues(path,values)","value list does not have a valid length.");
    }
}

void DTPath2DValues::pinfo(void) const
{
    path.pinfo();
}

void DTPath2DValues::pall(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n";
		return;
	}
	ssize_t i,len;
	if (NumberOfLoops()==1) {
		DTDoubleArray pathData = path.Data();
		len = pathData.n();
		for (i=1;i<len;i++) {
			cerr << "(" << pathData(0,i) << ", " << pathData(1,i) << ") = " << values(i) << std::endl;
		}
	}
	else {
		len = NumberOfLoops();
		for (i=0;i<len;i++) {
			cerr << "Loop #" << i << std::endl;
			ExtractLoop(*this,i).pall();
		}
	}
	cerr << flush;
}

void DTPath2DValues::ppath(void) const
{
	Path().pall();
}

DTRegion2D BoundingBox(const DTPath2DValues &P)
{
    return BoundingBox(P.Path());
}

bool operator==(const DTPath2DValues &A,const DTPath2DValues &B)
{
    return (A.Grid()==B.Grid() && A.Values()==B.Values());
}

DTRegion1D ValueRange(const DTPath2DValues &P)
{
    // This is not a very efficient method to finding the bounding box, but
    // gives a template for how the loop data structure can be parsed.

    if (P.IsEmpty())
        return DTRegion1D();
    DTDoubleArray D = P.Values();

#if defined(WIN32) && !defined(INFINITY)
#define INFINITY std::numeric_limits<float>::infinity();
#endif
    double vmin = INFINITY;
    double vmax = -INFINITY;

    double v;
    int i;

    // Packed.
    ssize_t length = D.Length();
    int until;
    int loc = 0;
    while (loc<length) {
        until = loc+1+int(D(loc));
        for (i=loc+1;i<until;i++) {
            v = D(i);
            vmin = (v < vmin ? v : vmin);
            vmax = (vmax < v ? v : vmax);
        }
        loc = until;
    }

    if (vmin>vmax)
        return DTRegion1D();
    else
        return DTRegion1D(vmin,vmax);
}

double Maximum(const DTPath2DValues &P)
{
    double vmax = -INFINITY;
	
    double v;
    int i;
	
	if (P.IsEmpty())
        return NAN;
    DTDoubleArray D = P.Values();
	
    // Packed.
    ssize_t length = D.Length();
    int until;
    int loc = 0;
    while (loc<length) {
        until = loc+1+int(D(loc));
        for (i=loc+1;i<until;i++) {
            v = D(i);
            vmax = (vmax < v ? v : vmax);
        }
        loc = until;
    }
	
	return vmax;
}

DTPath2DValues operator-(const DTPath2DValues &A,const DTPath2DValues &B)
{
	if (A.Grid()!=B.Grid()) {
		DTErrorMessage("DTPath2DValues-DTPath2DValues","Needs to be defined on the same path");
		return DTPath2DValues();
	}
	DTDoubleArray Arr = A.Path().Data();
	DTMutableDoubleArray newValues = A.Values()-B.Values();
	
	// Needs to have the same loop structure
	ssize_t length = Arr.n();
	int len,loc = 0;
    while (loc<length) {
		len = int(Arr(1,loc));
		newValues(loc) = len;
		loc += 1+len;
    }
	
	return DTPath2DValues(A.Grid(),newValues);
}

DTMutableDoubleArray Points(const DTPath2DValues &PV)
{
    return Points(PV.Path());
}

DTMutableDoubleArray Values(const DTPath2DValues &PV)
{
    int howManyPoints = 0;
    int loc = 0;
    int loopLength,until;
    DTPath2D path = PV.Path();
    DTDoubleArray values = PV.Values();
    DTDoubleArray Data = path.Data();
    ssize_t len = values.Length();
    while (loc<len) {
        loopLength = int(values(loc));
        until = loc+1+loopLength;
        howManyPoints += loopLength;
        if (Data(0,loc+1)==Data(0,until-1) && Data(1,loc+1)==Data(1,until-1))
            howManyPoints--;
        loc = until;
    }
    
    DTMutableDoubleArray toReturn(howManyPoints);
    
    loc = 0;
    int pos = 0;
    while (loc<len) {
        loopLength = int(values(loc));
        until = loc+1+loopLength;
        if (Data(0,loc+1)==Data(0,until-1) && Data(1,loc+1)==Data(1,until-1)) {
            loopLength--;
        }
        std::memcpy(toReturn.Pointer()+pos,values.Pointer()+(loc+1),loopLength*sizeof(double));
        pos += loopLength;
        loc = until;
    }
    
    return toReturn;
}

DTPath2DValues ExtractLoop(const DTPath2DValues &P,ssize_t loop)
{
    int loopNumber = 0;
    DTDoubleArray pathData = P.Path().Data();
	DTDoubleArray valueData = P.Values();
    int loc = 0;
    ssize_t len = pathData.n();
    int loopLength;
    DTMutableDoubleArray block,blockValues;
    
    if (loop<0) {
        DTErrorMessage("ExtractLoop(DTPath2DValues,int)","Loop out of bounds (negative)");
        return DTPath2DValues();
    }
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        if (loopNumber==loop) {
            block = DTMutableDoubleArray(2,loopLength+1);
			blockValues = DTMutableDoubleArray(loopLength+1);
            std::memcpy(block.Pointer(),pathData.Pointer()+2*loc,2*(loopLength+1)*sizeof(double));
            std::memcpy(blockValues.Pointer(),valueData.Pointer()+loc,(loopLength+1)*sizeof(double));
            break;
        }
        loc += 1+loopLength;
        loopNumber++;
    }
    
    DTPath2DValues toReturn;
    if (loc<len) {
        toReturn = DTPath2DValues(DTPath2D(block),blockValues);
    }
    else {
        DTErrorMessage("ExtractLoop(DTPath2DValues,int)","Loop out of bounds (too large)");
    }
	
    return toReturn;
}

DTPath2DValues ChangeOrientation(const DTPath2DValues &Pv)
{
	if (Pv.IsEmpty()) return Pv;
	DTPath2D newPath = ChangeOrientation(Pv.Path());
	DTDoubleArray val = Pv.Values();
	ssize_t loc=0,i,len,length = val.Length();
	DTMutableDoubleArray newValues(length);
	while (loc<length) {
		len = int(val(loc));
		newValues(loc) = len;
		for (i=0;i<len;i++) {
			newValues(loc+1+i) = val(loc+len-i);
		}
		loc += len+1;
	}
	return DTPath2DValues(newPath,newValues);
}

void Read(const DTDataStorage &input,const std::string &name,DTPath2DValues &toReturn)
{
    DTPath2D path;
    Read(input,name,path);
    DTDoubleArray values;
    Read(input,name+"_V",values);
    toReturn = DTPath2DValues(path,values);
}

DTPath2DValues Combine(const DTList<DTPath2DValues> &List)
{
	ssize_t i,howMany = List.Length();
	int totalLength = 0;
	if (howMany==0) return DTPath2DValues();
	if (howMany==1) return List(0);
	for (i=0;i<howMany;i++) {
		totalLength += List(i).Values().Length();
	}
	DTMutableDoubleArray combinedPath(2,totalLength);
	DTMutableDoubleArray combinedValues(totalLength);
	int posInCombined = 0;
	DTDoubleArray array;
	for (i=0;i<howMany;i++) {
		array = List(i).Grid().Data();
		if (array.Length()) {
			std::memcpy(combinedPath.Pointer()+2*posInCombined,array.Pointer(),array.Length()*sizeof(double));
			array = List(i).Values();
			std::memcpy(combinedValues.Pointer()+posInCombined,array.Pointer(),array.Length()*sizeof(double));
			posInCombined+=array.Length();
		}
	}
	
	DTPath2D thePath(combinedPath);
	return DTPath2DValues(thePath,combinedValues);
}

void Write(DTDataStorage &output,const std::string &name,const DTPath2DValues &thePath)
{
    Write(output,name+"_V",thePath.Values());
    Write(output,name,thePath.Path());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPath2DValues &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Path Values");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPath2DValues &toReturn,DTPath2D_SaveInfo &shared)
{
    DTPath2D grid;
    DTDoubleArray v;
    Read(input,name+"_V",v);
    Read(input,name,grid,shared);
    
    toReturn = DTPath2DValues(grid,v);
}

void Write(DTDataStorage &output,const std::string &name,const DTPath2DValues &toWrite,DTPath2D_SaveInfo &shared)
{
    Write(output,name+"_V",toWrite.Values());
    Write(output,name,toWrite.Grid(),shared);
}

