// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesBitmap2D_Header
#define DTSeriesBitmap2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTBitmap2D;
class DTBitmap2DGrid;
class DTDoubleArray;

class DTSeriesBitmap2D : public DTSeries
{
public:
    DTSeriesBitmap2D() : DTSeries("Bitmap2D") {}
    DTSeriesBitmap2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Bitmap2D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesBitmap2D ByName(string);
    
    void Add(DTBitmap2D v,double time);
    
    DTBitmap2D Get(double time) const;
    
private:
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesBitmap2D &toReturn);

#endif

