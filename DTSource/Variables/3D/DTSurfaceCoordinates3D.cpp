// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSurfaceCoordinates3D.h"
#include "DTRegion3D.h"
#include "DTError.h"

DTSurfaceCoordinates3D::DTSurfaceCoordinates3D(const DTSurface3D &s,const DTFloatArray &uarr,const DTFloatArray &varr)
{
    if (s.NumberOfPoints()!=uarr.Length() || uarr.Length()!=varr.Length()) {
        DTErrorMessage("DTSurfaceCoordinates3D(surface,u,v)","Not the right number of points saved in u,v");
        return;
    }
    if (u.Length()!=u.m() || v.Length()!=v.m()) {
        DTErrorMessage("DTSurfaceCoordinates3D(surface,u,v)","u and v have to be vectors");
        return;
    }
    
    surface = s;
    u = uarr;
    v = varr;
}
    
void DTSurfaceCoordinates3D::pinfo() const
{
    surface.pinfo();
}

DTRegion3D BoundingBox(const DTSurfaceCoordinates3D &S)
{
    return BoundingBox(S.Surface());
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceCoordinates3D &toReturn)
{
    DTSurface3D surface;
    Read(input,name,surface);
    
    DTFloatArray u = input.ReadFloatArray(input.ResolveName(name+"_u"));
    DTFloatArray v = input.ReadFloatArray(input.ResolveName(name+"_v"));
    
    toReturn = DTSurfaceCoordinates3D(surface,u,v);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite)
{
    Write(output,name,toWrite.Surface());
    Write(output,name+"_u",toWrite.U());
    Write(output,name+"_v",toWrite.V());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Surface Coordinates");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceCoordinates3D &toReturn,DTSurfaceCoordinates3D_SaveInfo &info)
{
    DTSurface3D surface;
    DTFloatArray u,v;

    Read(input,name,surface,info.surface);
    
    std::string uname = input.ResolveName(name+"_u");
    if (info.uname==uname) {
        u = info.u;
    }
    else {
        u = input.ReadFloatArray(uname);
    }
    
    std::string vname = input.ResolveName(name+"_v");
    if (info.vname==vname) {
        v = info.v;
    }
    else {
        v = input.ReadFloatArray(vname);
    }
    
    toReturn = DTSurfaceCoordinates3D(surface,u,v);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceCoordinates3D &toWrite,DTSurfaceCoordinates3D_SaveInfo &info)
{
    Write(output,name,toWrite.Surface(),info.surface);
    DTFloatArray u = toWrite.U();
    if (info.u==u) {
        Write(output,name+"_u",info.uname);
    }
    else {
        Write(output,name+"_u",u);
        info.u = u;
        info.uname = name+"_u";
    }
    DTFloatArray v = toWrite.V();
    if (info.v==v) {
        Write(output,name+"_v",info.vname);
    }
    else {
        Write(output,name+"_v",v);
        info.v = v;
        info.vname = name+"_v";
    }
}

