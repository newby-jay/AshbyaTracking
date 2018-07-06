// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTStructuredGrid3D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"

DTStructuredGrid3D::DTStructuredGrid3D(const DTFloatArray &Xin,const DTFloatArray &Yin,const DTFloatArray &Zin)
: maskDefined(false)
{
    int mV = int(Xin.m());
    int nV = int(Yin.n());
    int oV = int(Zin.o());
    if (mV==0 || nV==0 || oV==0)
        return;

    // There are restrictions on the dimensions.
    if ((nV!=Xin.n() && Xin.n()!=1) || (oV!=Xin.o() && Xin.o()!=1) ||
        (mV!=Yin.m() && Yin.m()!=1) || (oV!=Yin.o() && Yin.o()!=1) ||
        (mV!=Zin.m() && Zin.m()!=1) || (nV!=Zin.n() && Zin.n()!=1)) {
        DTErrorMessage("DTStructuredGrid3D(X,Y,Z)","Invalid array sizes.");
        return;
    }

    Xa = Xin;
    Ya = Yin;
    Za = Zin;
}

DTStructuredGrid3D::DTStructuredGrid3D(const DTFloatArray &Xin,const DTFloatArray &Yin,const DTFloatArray &Zin,
                                       const DTMask &theMask)
: maskDefined(false)
{
    int mV = int(Xin.m());
    int nV = int(Yin.n());
    int oV = int(Zin.o());
    if (mV==0 || nV==0 || oV==0)
        return;

    // There are restrictions on the dimensions.
    if ((nV!=Xin.n() && Xin.n()!=1) || (oV!=Xin.o() && Xin.o()!=1) ||
        (mV!=Yin.m() && Yin.m()!=1) || (oV!=Yin.o() && Yin.o()!=1) ||
        (mV!=Zin.m() && Zin.m()!=1) || (nV!=Zin.n() && Zin.n()!=1)) {
        DTErrorMessage("DTStructuredGrid3D(X,Y,Z,Mask)","Invalid array sizes.");
        return;
    }

    if (theMask.m()!=mV || theMask.n()!=nV || theMask.o()!=oV) {
        DTErrorMessage("DTStructuredGrid3D(X,Y,Z,Mask)","Invalid mask.");
    }
    else {
        mask = theMask;
        maskDefined = true;
    }

    Xa = Xin;
    Ya = Yin;
    Za = Zin;
}

DTFloatArray DTStructuredGrid3D::FullXArray(void) const
{
    if (Xa.n()!=1)
        return Xa;

    DTMutableFloatArray toReturn(m(),n(),o());

    int i,j,k;
    int mV = m();
    int nV = n();
    int oV = o();

    float v;

    for (i=0;i<mV;i++) {
        v = Xa(i);
        for (k=0;k<oV;k++) {
            for (j=0;j<nV;j++) {
                toReturn(i,j,k) = v;
            }
        }
    }
        
    return toReturn;
}

DTFloatArray DTStructuredGrid3D::FullYArray(void) const
{
    if (Ya.m()!=1)
        return Ya;

    DTMutableFloatArray toReturn(m(),n(),o());

    int i,j,k;
    int mV = m();
    int nV = n();
    int oV = o();

    float v;

    for (j=0;j<nV;j++) {
        v = Ya(j);
        for (k=0;k<oV;k++) {
            for (i=0;i<mV;i++) {
                toReturn(i,j,k) = v;
            }
        }
    }

    return toReturn;
}

DTFloatArray DTStructuredGrid3D::FullZArray(void) const
{
    if (Za.m()!=1)
        return Za;

    DTMutableFloatArray toReturn(m(),n(),o());

    int i,j,k;
    int mV = m();
    int nV = n();
    int oV = o();

    float v;

    for (k=0;k<oV;k++) {
        v = Za(k);
        for (j=0;j<nV;j++) {
            for (i=0;i<mV;i++) {
                toReturn(i,j,k) = v;
            }
        }
    }

    return toReturn;
}

void DTStructuredGrid3D::pinfo(void) const
{
    std::cerr << m() << " x " << n() << " x " << o();
    if (MaskDefined())
        std::cerr << ". Mask = " << Mask().NumberOfPoints() << " points.";
    std::cerr << std::flush;
}

DTMutableStructuredGrid3D::DTMutableStructuredGrid3D(const DTMutableFloatArray &Xin,const DTMutableFloatArray &Yin,const DTMutableFloatArray &Zin)
: DTStructuredGrid3D(Xin,Yin,Zin)
{
    // See if it worked
    if (Xa.IsEmpty())
        return;
    
    // Keep a mutable version.  This will point to the exact same data as Xa,Ya,Za, but can be changed.
    Xm = Xin;
    Ym = Yin;
    Zm = Zin;
}    
    
bool operator==(const DTStructuredGrid3D &A,const DTStructuredGrid3D &B)
{
    return (A.X()==B.X() && A.Y()==B.Y() && A.Z()==B.Z());
}

bool operator!=(const DTStructuredGrid3D &A,const DTStructuredGrid3D &B)
{
    return (A.X()!=B.X() || A.Y()!=B.Y() || A.Z()!=B.Z());
}

DTRegion3D BoundingBox(const DTStructuredGrid3D &G)
{
    DTRegion1D xRange = ValueRange(G.X());
    DTRegion1D yRange = ValueRange(G.Y());
    DTRegion1D zRange = ValueRange(G.Z());
    
    if (!xRange.isSet || !yRange.isSet || !zRange.isSet)
        return DTRegion3D();
    
    return DTRegion3D(DTPoint3D(xRange.minV,yRange.minV,zRange.minV),DTPoint3D(xRange.maxV,yRange.maxV,zRange.maxV));
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid3D &toReturn)
{
    std::string theName = input.ResolveName(name);

    // See the write routine.
    DTIntArray mask;
    Read(input,theName,mask);

    DTFloatArray X,Y,Z;
    Read(input,theName+"_X",X);
    Read(input,theName+"_Y",Y);
    Read(input,theName+"_Z",Z);

    if (mask.IsEmpty())
        toReturn = DTStructuredGrid3D(X,Y,Z);
    else {
        DTMask Mask = DTMask(mask,X.m(),Y.n(),Z.o());
        toReturn = DTStructuredGrid3D(X,Y,Z,Mask);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite)
{
    Write(output,name+"_bbox3D",BoundingBox(toWrite));

    // Save the domain as name, the rest as name_X, ...
    Write(output,name+"_X",toWrite.X());
    Write(output,name+"_Y",toWrite.Y());
    Write(output,name+"_Z",toWrite.Z());
    if (toWrite.MaskDefined())
        Write(output,name,toWrite.Mask().Intervals());
    else
        Write(output,name,DTIntArray());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Structured Grid");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTStructuredGrid3D &toReturn,DTStructuredGrid3D_SaveInfo &SaveInfo)
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

void Write(DTDataStorage &output,const std::string &name,const DTStructuredGrid3D &toWrite,DTStructuredGrid3D_SaveInfo &SaveInfo)
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

