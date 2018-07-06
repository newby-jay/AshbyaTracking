// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPath3DValues.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"
#include "DTPath3D.h"
#include "DTPoint3DValue.h"
#include "DTDoubleArrayOperators.h"

#include <math.h>
#include <limits>
#include <cstring>

DTPath3DValues::DTPath3DValues(const DTPath3D &pathIn,const DTDoubleArray &valuesIn)
{
    // Makes sure that this is a valid format.
    if (pathIn.IsEmpty())
        return;
    
    DTDoubleArray Data = pathIn.Data();
    if (Data.n()!=valuesIn.m() || valuesIn.m()!=valuesIn.Length()) {
        DTErrorMessage("DTPath3DValues(path,values)","incompatible arguments.  Need to have the same length");
        return;
    }
    
    int loc = 0;
    int length = (int)Data.n();
    int loopLength;
    while (loc<length) {
        if (valuesIn(loc)!=0 && Data(2,loc)!=valuesIn(loc)) {
            DTErrorMessage("DTPath3DValues(path,values)","value array does not have the same loop structure as the path.");
            break;
        }
        loopLength = int(Data(2,loc));
        loc += loopLength+1;
    }
    
    if (loc<length)
        return;
    
    path = pathIn;
    values = valuesIn;
}

void DTPath3DValues::pinfo(void) const
{
    path.pinfo();
}

void DTPath3DValues::pall(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty\n";
    }
	else {
        if (path.NumberOfLoops()==1) {
			DTDoubleArray data = path.Data();
			int i,N = (int)data.n();
			for (i=1;i<N;i++) {
				cerr << i-1 << " : " << data(0,i) << ", " << data(1,i) << ", " << data(2,i) << " : " << values(i) << std::endl;
			}
		}
		else {
			int len = path.NumberOfLoops();
			for (int i=0;i<len;i++) {
				cerr << "Loop #" << i << std::endl;
				ExtractLoop(*this,i).pall();
			}
		}
	}
}

DTPath3DValues ExtractLoop(const DTPath3DValues &P,int i)
{
    DTDoubleArray pathData = P.Grid().Data();
	DTDoubleArray valueData = P.Values();
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
        DTErrorMessage("ExtractLoop(DTPath3DValues,int)","Index out of bounds");
        return DTPath3DValues();
    }
    
    DTMutableDoubleArray section(3,until-loc);
	std::memcpy(section.Pointer(),pathData.Pointer()+3*loc,section.Length()*sizeof(double));
    
    DTPath3D returnPath(section,P.Grid().CroppingBox());
	DTMutableDoubleArray returnValues(until-loc);
	std::memcpy(returnValues.Pointer(),valueData.Pointer()+loc, returnValues.Length()*sizeof(double));

	return DTPath3DValues(returnPath,returnValues);
}

DTPath3DValues Combine(const DTList<DTPath3DValues> &List)
{
	int i,howMany = (int)List.Length();
	int totalLength = 0;
	if (howMany==0) return DTPath3DValues();
	if (howMany==1) return List(0);
	for (i=0;i<howMany;i++) {
		totalLength += List(i).Values().Length();
	}
	DTMutableDoubleArray combinedPath(3,totalLength);
	DTMutableDoubleArray combinedValues(totalLength);
	int posInCombined = 0;
	DTDoubleArray array;
	for (i=0;i<howMany;i++) {
		array = List(i).Grid().Data();
		if (array.Length()) {
			std::memcpy(combinedPath.Pointer()+3*posInCombined,array.Pointer(),array.Length()*sizeof(double));
			array = List(i).Values();
			std::memcpy(combinedValues.Pointer()+posInCombined,array.Pointer(),array.Length()*sizeof(double));
			posInCombined+=array.Length();
		}
	}
	
	DTPath3D thePath(combinedPath);
	return DTPath3DValues(thePath,combinedValues);
}

DTPoint3DValue FirstPoint(const DTPath3DValues &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("FirstPoint(DTPath3DValues)","Path is empty");
		return DTPoint3DValue(DTPoint3D(NAN,NAN,NAN),NAN);
	}

	DTDoubleArray data = P.Path().Data();
    DTDoubleArray values = P.Values();
    return DTPoint3DValue(DTPoint3D(data(0,1),data(1,1),data(2,1)),values(1));
}

DTPoint3DValue LastPoint(const DTPath3DValues &P)
{
	if (P.IsEmpty()) {
		DTErrorMessage("LastPoint(DTPath3DValues)","Path is empty");
		return DTPoint3DValue(DTPoint3D(NAN,NAN,NAN),NAN);
	}

	DTDoubleArray data = P.Path().Data();
    DTDoubleArray values = P.Values();
	int n = (int)data.n();
    return DTPoint3DValue(DTPoint3D(data(0,n-1),data(1,n-1),data(2,n-1)),values(n-1));
}

DTRegion3D BoundingBox(const DTPath3DValues &P)
{
    return BoundingBox(P.Path());
}

bool operator==(const DTPath3DValues &A,const DTPath3DValues &B)
{
    return (A.Values()==B.Values() && A.Path()==B.Path());
}

bool operator!=(const DTPath3DValues &A,const DTPath3DValues &B)
{
    return (A.Values()!=B.Values() || A.Path()!=B.Path());
}

DTRegion1D ValueRange(const DTPath3DValues &P)
{
    // This is not a very efficient method to finding the value range, but
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
    int length = (int)D.Length();
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

double Minimum(const DTPath3DValues &P)
{
    // This is not a very efficient method to finding the value range, but
    // gives a template for how the loop data structure can be parsed.

#if defined(WIN32) && !defined(INFINITY)
#define INFINITY std::numeric_limits<float>::infinity();
#endif
    double vmin = INFINITY;

    if (P.IsEmpty())
        return INFINITY;
    DTDoubleArray D = P.Values();

    double v;
    int i;

    // Packed.
    int length = (int)D.Length();
    int until;
    int loc = 0;
    while (loc<length) {
        until = loc+1+int(D(loc));
        for (i=loc+1;i<until;i++) {
            v = D(i);
            vmin = (v < vmin ? v : vmin);
        }
        loc = until;
    }

    return vmin;
}

DTPath3DValues operator-(const DTPath3DValues &A,const DTPath3DValues &B)
{
	if (A.Grid()!=B.Grid()) {
		DTErrorMessage("DTPath3DValues-DTPath3DValues","Needs to be defined on the same path");
		return DTPath3DValues();
	}
	DTDoubleArray Arr = A.Path().Data();
	DTMutableDoubleArray newValues = A.Values()-B.Values();

	// Needs to have the same loop structure
	int length = (int)Arr.n();
	int len,loc = 0;
    while (loc<length) {
		len = int(Arr(2,loc));
		newValues(loc) = len;
		loc += 1+len;
    }
	
	return DTPath3DValues(A.Grid(),newValues);
}

void Read(const DTDataStorage &input,const std::string &name,DTPath3DValues &toReturn)
{
    DTPath3D path;
    Read(input,name,path);
    DTDoubleArray values;
    Read(input,name+"_V",values);
    toReturn = DTPath3DValues(path,values);
}

void Write(DTDataStorage &output,const std::string &name,const DTPath3DValues &thePath)
{
    Write(output,name+"_V",thePath.Values());
    Write(output,name,thePath.Path());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPath3DValues &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Path Values");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTPath3DValues &toReturn,DTPath3D_SaveInfo &shared)
{
    DTPath3D path;
    Read(input,name,path,shared);
    DTDoubleArray values;
    Read(input,name+"_V",values);
    toReturn = DTPath3DValues(path,values);
}

void Write(DTDataStorage &output,const std::string &name,const DTPath3DValues &thePath,DTPath3D_SaveInfo &shared)
{
    Write(output,name+"_V",thePath.Values());
    Write(output,name,thePath.Path(),shared);
}

#pragma mark mutable version

DTMutablePath3DValues::DTMutablePath3DValues(const DTPath3D &P)
{
	if (P.StoredAsLoops()==false) {
		DTErrorMessage("DTMutablePath3DValues(Path)","Path needs to be stored as segments");
		return;
	}
	path = P;
	mutableValues = DTMutableDoubleArray(path.Data().n());
	values = mutableValues;
	
	// Set the lengths
	DTDoubleArray Arr = path.Data();
	int length = (int)Arr.n();
	int len,loc = 0;
    while (loc<length) {
		len = int(Arr(2,loc));
		mutableValues(loc) = len;
		loc += 1+len;
    }
}

DTMutablePath3DValues &DTMutablePath3DValues::operator=(double v)
{
	DTDoubleArray Arr = path.Data();
	int length = (int)Arr.n();
	int until,len,loc = 0;
    while (loc<length) {
		len = int(Arr(2,loc));
		loc++;
		until = loc+len;
		while (loc<until) {
			mutableValues(loc) = v;
			loc++;
		}
    }
	
	return *this;
}
	
