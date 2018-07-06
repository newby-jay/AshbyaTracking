// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPointVector3D.h"

#include "DTDoubleArray.h"
#include "DTError.h"

void DTPointVector3D::pinfo(void) const
{
    std::cerr << "(" << point.x << ", " << point.y << ") = (" << vector.x << ", " << vector.y << ")\n" << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTPointVector3D &var)
{
    DTDoubleArray arr;
    Read(input,name,arr);
    if (arr.Length()==0) {
        var = DTPointVector3D();
    }
    else if (arr.Length()==6) {
        var = DTPointVector3D(DTPoint3D(arr(0),arr(1),arr(2)),DTVector3D(arr(3),arr(4),arr(5)));
    }
    else {
        DTErrorMessage("Read(input,name,DTPointVector3D)","Invalid format");
        var = DTPointVector3D();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPointVector3D &var)
{
    DTMutableDoubleArray arr(3,2);
    arr(0,0) = var.point.x;
    arr(1,0) = var.point.y;
    arr(2,0) = var.point.z;
    arr(0,1) = var.vector.x;
    arr(1,1) = var.vector.y;
    arr(2,1) = var.vector.z;

    output.Save(arr,name);
}

