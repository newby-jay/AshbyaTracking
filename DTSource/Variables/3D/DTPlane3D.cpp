// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTPlane3D.h"

#include "DTError.h"
#include "DTDoubleArray.h"

void DTPlane3D::pinfo(void) const
{
    std::cerr << "Point = (" << center.x << ", " << center.y << ", " << center.z << ") - ";
    std::cerr << "Normal = (" << normal.x << ", " << normal.y << ", " << normal.z << ") - ";
    std::cerr << "Up = (" << up.x << ", " << up.y << ", " << up.z << ")\n" << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTPlane3D &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=9) {
        DTErrorMessage("ReadFromArray(DTPlane3D)","Invalid length of array.");
        toReturn = DTPlane3D();
    }
    else {
        toReturn = DTPlane3D(DTPoint3D(theArr(0),theArr(1),theArr(2)),
                             DTVector3D(theArr(3),theArr(4),theArr(5)),
                             DTVector3D(theArr(6),theArr(7),theArr(8)));
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTPlane3D &theVar)
{
    DTMutableDoubleArray theArr(3,3);
    DTPoint3D c = theVar.Center();
    theArr(0,0) = c.x;
    theArr(1,0) = c.y;
    theArr(2,0) = c.z;
    DTVector3D n = theVar.Normal();
    theArr(0,1) = n.x;
    theArr(1,1) = n.y;
    theArr(2,1) = n.z;
    DTVector3D u = theVar.Up();
    theArr(0,2) = u.x;
    theArr(1,2) = u.y;
    theArr(2,2) = u.z;
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTPlane3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Plane");
    output.Flush();
}

