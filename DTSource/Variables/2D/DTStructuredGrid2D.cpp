// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredGrid2D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion2D.h"

DTStructuredGrid2D::DTStructuredGrid2D(const DTFloatArray &Xin,const DTFloatArray &Yin)
: maskDefined(false)
{
    ssize_t mV = Xin.m();
    ssize_t nV = Yin.n();
    if (mV==0 || nV==0)
        return;

    // There are restrictions on the dimensions.
    if ((nV!=Xin.n() && Xin.n()!=1) || (mV!=Yin.m() && Yin.m()!=1)) {
        DTErrorMessage("DTStructuredGrid2D(X,Y)","Invalid array sizes.");
        return;
    }

    Xa = Xin;
    Ya = Yin;
}

DTStructuredGrid2D::DTStructuredGrid2D(const DTFloatArray &Xin,const DTFloatArray &Yin,const DTMask &theMask)
: maskDefined(false)
{
    ssize_t mV = Xin.m();
    ssize_t nV = Yin.n();
    if (mV==0 || nV==0)
        return;

    // There are restrictions on the dimensions.
    if ((nV!=Xin.n() && Xin.n()!=1) || (mV!=Yin.m() && Yin.m()!=1)) {
        DTErrorMessage("DTStructuredGrid2D(X,Y,Mask)","Invalid array sizes.");
        return;
    }

    if (theMask.m()!=mV || theMask.n()!=nV || theMask.o()!=1) {
        DTErrorMessage("DTStructuredGrid2D(X,Y,Mask)","Invalid mask.");
    }
    else {
        mask = theMask;
        maskDefined = true;
    }

    Xa = Xin;
    Ya = Yin;
}

DTFloatArray DTStructuredGrid2D::FullXArray(void) const
{
    if (Xa.n()!=1)
        return Xa;

    DTMutableFloatArray toReturn(m(),n());

    int i,j;
    ssize_t mV = m();
    ssize_t nV = n();

    float v;

    for (i=0;i<mV;i++) {
        v = Xa(i);
        for (j=0;j<nV;j++) {
            toReturn(i,j) = v;
        }
    }

    return toReturn;
}

DTFloatArray DTStructuredGrid2D::FullYArray(void) const
{
    if (Ya.m()!=1)
        return Ya;

    DTMutableFloatArray toReturn(m(),n());

    int i,j;
    ssize_t mV = m();
    ssize_t nV = n();

    float v;

    for (j=0;j<nV;j++) {
        v = Ya(j);
        for (i=0;i<mV;i++) {
            toReturn(i,j) = v;
        }
    }

    return toReturn;
}

void DTStructuredGrid2D::pinfo(void) const
{
    std::cerr << m() << "x" << n() << " structured mesh";
    if (MaskDefined())
        std::cerr << ". Mask = " << Mask().NumberOfPoints() << " points.";
    std::cerr << std::flush;
}

DTRegion2D BoundingBox(const DTStructuredGrid2D &G)
{
    DTRegion1D xRange = ValueRange(G.X());
    DTRegion1D yRange = ValueRange(G.Y());

    if (!xRange.isSet || !yRange.isSet)
        return DTRegion2D();

    return DTRegion2D(DTPoint2D(xRange.minV,yRange.minV),DTPoint2D(xRange.maxV,yRange.maxV));
}

bool operator==(const DTStructuredGrid2D &A,const DTStructuredGrid2D &B)
{
    if (A.MaskDefined()!=B.MaskDefined())
        return false;

    if (A.MaskDefined())
        return (A.X()==B.X() && A.Y()==B.Y());
    else
        return (A.X()==B.X() && A.Y()==B.Y() && A.Mask()==B.Mask());
}

bool operator!=(const DTStructuredGrid2D &A,const DTStructuredGrid2D &B)
{
    return !(A==B);
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid2D &toReturn)
{
    std::string theName = input.ResolveName(name);

    // See the write routine.
    DTIntArray mask;
    Read(input,theName,mask);

    DTFloatArray X,Y,Z;
    Read(input,theName+"_X",X);
    Read(input,theName+"_Y",Y);

    if (mask.IsEmpty())
        toReturn = DTStructuredGrid2D(X,Y);
    else {
        DTMask Mask = DTMask(mask,X.m(),Y.n());
        toReturn = DTStructuredGrid2D(X,Y,Mask);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite)
{
    // Save the domain as name, the rest as name_X, ...
    Write(output,name+"_bbox2D",BoundingBox(toWrite));

    Write(output,name+"_X",toWrite.X());
    Write(output,name+"_Y",toWrite.Y());
    if (toWrite.MaskDefined())
        Write(output,name,toWrite.Mask().Intervals());
    else
        Write(output,name,DTIntArray());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Structured Grid");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid2D &toReturn,DTStructuredGrid2D_SaveInfo &SaveInfo)
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

void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid2D &toWrite,DTStructuredGrid2D_SaveInfo &SaveInfo)
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

