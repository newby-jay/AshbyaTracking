// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTCoordinateLine2D.h"

#include "DTError.h"
#include "DTDoubleArray.h"

void DTCoordinateLine2D::pinfo(void) const
{
    switch (type) {
        case DTCoordinateLine2D::XFixed:
            std::cerr << "x line at " << value << std::endl;
            break;
        case DTCoordinateLine2D::YFixed:
            std::cerr << "y line at " << value << std::endl;
            break;
        case DTCoordinateLine2D::Empty:
            std::cerr << "Empty";
    }
    std::cerr << std::flush;
}

void Read(const DTDataStorage &input,const std::string &name,DTCoordinateLine2D &toReturn)
{
    DTDoubleArray theArr = input.ReadDoubleArray(name);
    if (theArr.Length()!=2) {
        DTErrorMessage("ReadFromArray(DTCoordinateLine2D)","Invalid length of array.");
        toReturn = DTCoordinateLine2D();
    }
    else if (theArr(0)==1) {
        toReturn = DTCoordinateLine2D(DTCoordinateLine2D::XFixed,theArr(1));
    }
    else if (theArr(0)==2) {
        toReturn = DTCoordinateLine2D(DTCoordinateLine2D::YFixed,theArr(1));
    }
    else {
        toReturn = DTCoordinateLine2D();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTCoordinateLine2D &theVar)
{
    DTMutableDoubleArray theArr(2);
    switch (theVar.Type()) {
        case DTCoordinateLine2D::XFixed:
            theArr(0) = 1;
            theArr(1) = theVar.Value();
            break;
        case DTCoordinateLine2D::YFixed:
            theArr(0) = 2;
            theArr(1) = theVar.Value();
            break;
        default:
            theArr(0) = 0;
            theArr(1) = 0.0;
            break;
    }
    output.Save(theArr,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTCoordinateLine2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Coordinate Line");
    output.Flush();
}

