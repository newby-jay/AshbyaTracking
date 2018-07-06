// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTSurfaceTexture3D.h"

#include "DTRegion3D.h"

DTSurfaceTexture3D::DTSurfaceTexture3D(const DTSurfaceCoordinates3D &s,const DTBitmap2D &im)
{
    coordinates = s;
    image = im;
}
    
void DTSurfaceTexture3D::pinfo() const
{
    coordinates.pinfo();
}
    
DTRegion3D BoundingBox(const DTSurfaceTexture3D &var)
{
    return BoundingBox(var.Coordinates());
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceTexture3D &toReturn)
{
    DTSurfaceCoordinates3D coord;
    Read(input,name,coord);
    DTBitmap2D image;
    Read(input,input.ResolveName(name+"_Image"),image);
    toReturn = DTSurfaceTexture3D(coord,image);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite)
{
    Write(output,name,toWrite.Coordinates());
    Write(output,name+"_Image",toWrite.Image());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Surface Texture");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTSurfaceTexture3D &toReturn,DTSurfaceCoordinates3D_SaveInfo &info)
{
    DTSurfaceCoordinates3D coord;
    Read(input,name,coord,info);
    DTBitmap2D image;
    Read(input,input.ResolveName(name+"_Image"),image);
    toReturn = DTSurfaceTexture3D(coord,image);
}

void Write(DTDataStorage &output,const std::string &name,const DTSurfaceTexture3D &toWrite,DTSurfaceCoordinates3D_SaveInfo &info)
{
    Write(output,name,toWrite.Coordinates(),info);
    Write(output,name+"_Image",toWrite.Image());
}


