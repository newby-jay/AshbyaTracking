// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.
//
//  Created by David Adalsteinsson on 9/14/15.
//

// Needs libTiff to work.  To install, go to
// http://www.remotesensing.org/libtiff/
// Click on the download site - http://download.osgeo.org/libtiff/
// Once uncompressed go to the libtiff directory and execute the following in the terminal
// ./configure
// make
// sudo make install
//
// Once you have that you need to include libtiff.a from /usr/local/lib and tiffio.h from /usr/local/include
// In Xcode 6.4 and possibly all others, you need to add /usr/local/lib to the search path
// Go into Build Settings -> Search Paths -> Library Search Paths and add /usr/local/lib



#ifndef DTTiffFile_Header
#define DTTiffFile_Header

#include "DTBitmap2D.h"
#include "tiffio.h"
#include "DTPointer.h"

struct DTTiffFileStorage
{
    DTTiffFileStorage(const std::string &nm);
    ~DTTiffFileStorage(void) {if (tif) TIFFClose(tif);}

    TIFF *tif;
};

// Class for reading
class DTTiffFile
{
public:
    DTTiffFile(const std::string &nm);

    bool IsOpen(void) const {return (content->tif!=NULL);}
    TIFF *RawPointer(void) const {return content->tif;}

    int NumberOfImages(void) const;

    DTBitmap2D ReadImage(int imageNumber) const;

private:
    DTPointer<DTTiffFileStorage> content;
};

#endif

// To write a tiff file
// http://research.cs.wisc.edu/graphics/Courses/638-f1999/libtiff_tutorial.htm