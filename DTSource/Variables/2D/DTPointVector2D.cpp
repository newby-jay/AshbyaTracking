// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPointVector2D.h"

#include "DTDoubleArray.h"
#include "DTError.h"

void DTPointVector2D::pinfo(void) const
{
    std::cerr << "(" << point.x << ", " << point.y << ") = (" << vector.x << ", " << vector.y << ")\n" << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTPointVector2D &var)
{
    DTDoubleArray arr;
    Read(input,name,arr);
    if (arr.Length()==0) {
        var = DTPointVector2D();
    }
    else if (arr.Length()==4) {
        var = DTPointVector2D(DTPoint2D(arr(0),arr(1)),DTVector2D(arr(2),arr(3)));
    }
    else {
        DTErrorMessage("Read(input,name,DTPointVector2D)","Invalid format");
        var = DTPointVector2D();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPointVector2D &var)
{
    DTMutableDoubleArray arr(2,2);
    arr(0,0) = var.point.x;
    arr(1,0) = var.point.y;
    arr(0,1) = var.vector.x;
    arr(1,1) = var.vector.y;

    output.Save(arr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPointVector2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Vector");
    output.Flush();
}

