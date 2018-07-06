// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTCoordinatePlane3D.h"

#include "DTError.h"
#include "DTDoubleArray.h"
#include "DTVector3D.h"

DTVector3D DTCoordinatePlane3D::Normal() const
{
    DTVector3D toReturn(0,0,0);
    
    switch (type) {
        case DTCoordinatePlane3D::XPlane:
            toReturn.x = 1.0;
            break;
        case DTCoordinatePlane3D::YPlane:
            toReturn.y = 1.0;
            break;
        case DTCoordinatePlane3D::ZPlane:
            toReturn.z = 1.0;
            break;
        case DTCoordinatePlane3D::Empty:
            break;
    }

    return toReturn;
}

void DTCoordinatePlane3D::pinfo(void) const
{
    switch (type) {
        case DTCoordinatePlane3D::XPlane:
            std::cerr << "x plane at ";
            break;
        case DTCoordinatePlane3D::YPlane:
            std::cerr << "y plane at ";
            break;
        case DTCoordinatePlane3D::ZPlane:
            std::cerr << "z plane at ";
            break;
        case DTCoordinatePlane3D::Empty:
            std::cerr << "Empty";
            return;
    }
    
    std::cerr << value << std::flush;
}

DTCoordinatePlane3D XYPlane(double z)
{
	return DTCoordinatePlane3D(DTCoordinatePlane3D::ZPlane,z);
}

DTCoordinatePlane3D XZPlane(double y)
{
	return DTCoordinatePlane3D(DTCoordinatePlane3D::YPlane,y);
}

DTCoordinatePlane3D YZPlane(double x)
{
	return DTCoordinatePlane3D(DTCoordinatePlane3D::XPlane,x);
}

void Read(const DTDataStorage &input,const std::string &name,DTCoordinatePlane3D &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=2) {
        DTErrorMessage("ReadFromArray(DTCoordinatePlane3D)","Invalid length of array.");
        toReturn = DTCoordinatePlane3D();
    }
    else if (theArr(0)==1) {
        toReturn = DTCoordinatePlane3D(DTCoordinatePlane3D::XPlane,theArr(1));
    }
    else if (theArr(0)==2) {
        toReturn = DTCoordinatePlane3D(DTCoordinatePlane3D::YPlane,theArr(1));
    }
    else if (theArr(0)==3) {
        toReturn = DTCoordinatePlane3D(DTCoordinatePlane3D::ZPlane,theArr(1));
    }
    else {
        toReturn = DTCoordinatePlane3D();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTCoordinatePlane3D &theVar)
{
    DTMutableDoubleArray theArr(2);
    switch (theVar.Type()) {
        case DTCoordinatePlane3D::XPlane:
            theArr(0) = 1;
            theArr(1) = theVar.Value();
            break;
        case DTCoordinatePlane3D::YPlane:
            theArr(0) = 2;
            theArr(1) = theVar.Value();
            break;
        case DTCoordinatePlane3D::ZPlane:
            theArr(0) = 3;
            theArr(1) = theVar.Value();
            break;
        default:
            theArr(0) = 0;
            theArr(1) = 0.0;
            break;
    }
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTCoordinatePlane3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Coordinate Plane");
    output.Flush();
}

