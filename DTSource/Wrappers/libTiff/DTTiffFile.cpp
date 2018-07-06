//
//  DTTiffFile.cpp
//
//  Created by David Adalsteinsson on 9/14/15.
//
//

#include "DTTiffFile.h"

DTTiffFileStorage::DTTiffFileStorage(const std::string &nm)
{
    tif = TIFFOpen(nm.c_str(), "r");
    if (tif==NULL) {
        DTErrorMessage("DTTiffFile(name)","Could not open the file " + nm);
    }
}

DTTiffFile::DTTiffFile(const std::string &nm)
{
    DTTiffFileStorage *ptr = new DTTiffFileStorage(nm);
    content = DTPointer<DTTiffFileStorage>(ptr);
}

int DTTiffFile::NumberOfImages(void) const
{
    if (IsOpen()==false)
        return 0;
    else
        return TIFFNumberOfDirectories(RawPointer());
}

struct ExtraSamples
{
    unsigned short count;
    unsigned short *pointer;
};

DTBitmap2D DTTiffFile::ReadImage(int imageNumber) const
{
    DTBitmap2D toReturn;
    
    TIFF *tif = RawPointer();
    if (tif==NULL) {
        DTErrorMessage("DTTiffFile::ReadImage","File is not open");
        return toReturn;
    }

    int ret;

    ret = TIFFSetDirectory(tif,imageNumber);

    // TIFFCurrentDirectory();

    uint32 w,h,config;
    int row;
    uint16 nsamples,bitsPerSample,photometric,samplesPerPixel,orientation;
    ret = TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    ret = TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    ret = TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config); // Saving method
    ret = TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    ret = TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    ret = TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
    ret = TIFFGetField(tif, TIFFTAG_ORIENTATION, &orientation);
    
    DTMesh2DGrid grid(DTPoint2D(0,0),1.0);

    tdata_t buf = _TIFFmalloc(TIFFScanlineSize(tif));
    
    if (bitsPerSample==16) {
        // 2 bytes per channel
        uint16 *data=(uint16*)buf;
        
        if (samplesPerPixel==1) {
            // Gray scale
            DTMutableUShortIntArray grayScale(w,h);
            for (row = 0; row < h; row++) {
                TIFFReadScanline(tif, buf, row, 0);
                memcpy(grayScale.Pointer()+row*w,data,w*2);
            }
            toReturn = DTBitmap2D(grid,grayScale);
        }
        else {
            DTErrorMessage("DTTiffFile::ReadImage","Not implemented yet");
        }
    }
    else {
        DTErrorMessage("DTTiffFile::ReadImage","Not implemented yet");
    }
    
    _TIFFfree(buf);
    
    return toReturn;
}
