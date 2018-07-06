// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPlot1D.h"

#include "DTError.h"
#include "DTRegion1D.h"

#include <math.h>
#include <cstring>

#ifdef WIN32
#   include <float.h>
#   define isfinite _finite
#endif

DTPlot1D::DTPlot1D(const DTDoubleArray &input)
{
    // The format is
    // [ #  x x x ... x #  x x ... x #  ....]
    // [ N1 y y y ... y N2 y y ... y N3 ....]
    // This allows multiple connected loops to be stored inside a single plot.
    // This is the same format as is used to represent a path as a single array.
    // If you want to create a plot from two lists, use the DTPlot1D(x,y) creator.

    if (input.IsEmpty())
        return;

    if (input.m()!=2) {
        DTErrorMessage("DTPlot1D(Array)","Need two rows");
        return;
    }

    ssize_t loc = 0;
    ssize_t length = input.n();
    ssize_t loopLength;
    while (loc<length) {
        if (input(1,loc)!=floor(input(1,loc)) || input(1,loc)<1) {
            // The format expects each block in this array to start as
            // [ # ....]
            // [ N ....]
            // Where N is the number of points in this loop.
            DTErrorMessage("DTPlot1D(Array)","Invalid loop length.");
            break;
        }
        loopLength = int(input(1,loc));
        if (loopLength+loc+1>length) {
            // The last block had an incorrect length information.
            DTErrorMessage("DTPlot1D(Array)","A loop length is too long.");
            break;
        }
        loc += loopLength+1;
    }

    if (loc<length)
        return;

    data = input;
}

DTPlot1D::DTPlot1D(const DTDoubleArray &x,const DTDoubleArray &y)
{
    if (x.Length()!=y.Length()) {
        DTErrorMessage("DTPlot1D(Array,Array)","Incompatible lengths");
        return;
    }
    if (x.Length()==0) return;

    DTMutableDoubleArray temp(2,x.Length()+1);
    ssize_t i,len = x.Length();
    temp(0,0) = 0;
    temp(1,0) = double(len);
    double *tempD = temp.Pointer();
    const double *xD = x.Pointer();
    const double *yD = y.Pointer();
    ssize_t posInCombined = 2;
    for (i=0;i<len;i++) {
        tempD[posInCombined++] = xD[i];
        tempD[posInCombined++] = yD[i];
    }

    data = temp;
}

ssize_t DTPlot1D::NumberOfPoints(void) const
{
    ssize_t howLong,loc = 0;
    ssize_t length = data.n();
    ssize_t toReturn = 0;
    while (loc<length) {
        howLong = int(data(1,loc));
        toReturn += howLong;
        loc += howLong+1;
    }

    return toReturn;
}

ssize_t DTPlot1D::NumberOfLoops(void) const
{
    ssize_t toReturn = 0;
    DTDoubleArray pathData = Data();
    ssize_t loc = 0;
    ssize_t len = pathData.n();
    ssize_t loopLength;
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        loc += 1+loopLength;
        toReturn++;
    }
    
    return toReturn;
}

bool DTPlot1D::UniformlySpaced(void) const
{
    // Single loop and uniform step size.
    if (data.n()<3) return false;

    if (data(1,0)!=data.n()-1) return false;

    double h = data(0,2)-data(0,1);
    if (h==0.0) return false;
    ssize_t len = data.n();
    ssize_t i;
    for (i=2;i<len;i++) {
        if ( fabs((data(0,i)-data(0,i-1))/h-1) > 1e-5)
            break;
    }

    return (i==len);
}

DTPlot1D DTPlot1D::ExtractLoop(ssize_t loop) const
{
    ssize_t loopNumber = 0;
    DTDoubleArray pathData = Data();
    ssize_t loc = 0;
    ssize_t len = pathData.n();
    ssize_t loopLength;
    DTMutableDoubleArray block;
    
    if (loop<0) {
        DTErrorMessage("DTPlot1D::ExtractLoop","Loop out of bounds (negative)");
        return DTPlot1D();
    }
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        if (loopNumber==loop) {
            block = DTMutableDoubleArray(2,loopLength+1);
            std::memcpy(block.Pointer(),pathData.Pointer()+2*loc,2*(loopLength+1)*sizeof(double));
            break;
        }
        loc += 1+loopLength;
        loopNumber++;
    }
    
    DTPlot1D toReturn;
    if (loc<len) {
        toReturn = DTPlot1D(block);
    }
    else {
        DTErrorMessage("DTPlot1D::ExtractLoop","Loop out of bounds (too large)");
    }

    return toReturn;
}

bool DTPlot1D::ContainsX(double x) const
{
    ssize_t loopNumber = 0;
    DTDoubleArray pathData = Data();
    ssize_t loc = 0;
    ssize_t len = pathData.n();
    ssize_t loopLength;
    ssize_t i,until;
    double x0,x1;
    
    while (loc<len) {
        loopLength = int(pathData(1,loc));
        until = loc+loopLength;
        for (i=loc+1;i<until;i++) {
            x0 = pathData(0,i);
            x1 = pathData(0,i+1);
            if ((x0-x)*(x1-x)<=0.0)
                break;
        }
        if (i<until)
            break;
        loc += 1+loopLength;
        loopNumber++;
    }

    return (loc<len);
}

DTMutableDoubleArray DTPlot1D::XValues(void) const
{
    DTMutableDoubleArray toReturn(NumberOfPoints());
    
    ssize_t pos = 0;
    ssize_t until,loc = 0;
    ssize_t length = data.n();
    while (loc<length) {
        until = int(data(1,loc))+loc+1;
        loc++;
        while (loc<until) {
            toReturn(pos) = data(0,loc);
            pos++;
            loc++;
        }
    }

    return toReturn;
}

DTMutableDoubleArray DTPlot1D::YValues(void) const
{
    DTMutableDoubleArray toReturn(NumberOfPoints());
    
    ssize_t pos = 0;
    ssize_t until,loc = 0;
    ssize_t length = data.n();
    while (loc<length) {
        until = int(data(1,loc))+loc+1;
        loc++;
        while (loc<until) {
            toReturn(pos) = data(1,loc);
            pos++;
            loc++;
        }
    }

    return toReturn;
}

void DTPlot1D::pinfo(void) const
{
    if (IsEmpty())
        std::cerr << "Empty Plot\n";
    else if (NumberOfLoops()==1)
        std::cerr << NumberOfPoints() << " xy values.\n";
    else
        std::cerr << NumberOfPoints() << " xy values, " << NumberOfLoops() << " loops.\n";
    std::cerr << flush;
}

bool operator==(const DTPlot1D &A,const DTPlot1D &B)
{
    return (A.Data()==B.Data());
}

bool operator!=(const DTPlot1D &A,const DTPlot1D &B)
{
    return (A.Data()!=B.Data());
}

DTRegion1D BoundingRange(const DTPlot1D &plot)
{
    DTDoubleArray data = plot.Data();
    
    // Compute the value range.  By definition this excludes all Inf and NaN numbers.
    ssize_t until,loc = 0;
    ssize_t length = data.n();
    double minV = HUGE_VAL;
    double maxV = -HUGE_VAL;
    double v;
    while (loc<length) {
        until = loc + int(data(1,loc)) + 1;
        for (loc++; loc<until; loc++) {
            v = data(0,loc);
            if (isfinite(v)) {
                if (v<minV) minV = v;
                if (v>maxV) maxV = v;
            }
        }
    }

    if (isfinite(minV)) {
        return DTRegion1D(minV,maxV);
    }
    else {
        return DTRegion1D();
    }
}

DTRegion1D ValueRange(const DTPlot1D &plot)
{
    DTDoubleArray data = plot.Data();
    
    // Compute the value range.  By definition this excludes all Inf and NaN numbers.
    ssize_t until,loc = 0;
    ssize_t length = data.n();
    double minV = HUGE_VAL;
    double maxV = -HUGE_VAL;
    double v;
    while (loc<length) {
        until = loc + int(data(1,loc)) + 1;
        for (loc++; loc<until; loc++) {
            v = data(1,loc);
            if (isfinite(v)) {
                if (v<minV) minV = v;
                if (v>maxV) maxV = v;
            }
        }
    }

    if (isfinite(minV)) {
        return DTRegion1D(minV,maxV);
    }
    else {
        return DTRegion1D();
    }
}

void Read(const DTDataStorage &input,const std::string &name,DTPlot1D &toReturn)
{
    DTDoubleArray inArr = input.ReadDoubleArray(name);
    toReturn = DTPlot1D(inArr);
}

void Write(DTDataStorage &output,const std::string &name,const DTPlot1D &thePath)
{
    output.Save(thePath.Data(),name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPlot1D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"1D Plot");
    output.Flush();
}

