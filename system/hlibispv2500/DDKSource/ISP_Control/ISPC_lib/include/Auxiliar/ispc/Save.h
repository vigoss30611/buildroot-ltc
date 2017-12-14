/**
*******************************************************************************
 @file Save.h

 @brief ISPC Save - handle saving of images to disk from ISPC::Shot

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/
#ifndef ISPC_AUX_SAVE_H_
#define ISPC_AUX_SAVE_H_

struct SaveFile;  // defined in savefile.h
#include <img_types.h>
#include <ci/ci_api_structs.h>

#include <string>

namespace ISPC {
class Pipeline;  // defined in ispc/Pipeline.h
class Shot;  // defined in ispc/Shot.h
struct Buffer;  // defined in ispc/Shot.h

// begin added by linyun.xiong @2015-12-11
#ifdef ISP_SUPPORT_SCALER
	typedef struct ScalerImg
	{
		const IMG_UINT8 *data; ///< @brief Pointer to actual buffer data (available data from CI_SHOT)
		
		IMG_UINT32 stride; ///< @brief in Bytes of accessible data
		IMG_UINT32 width;
		IMG_UINT32 height;				
	} ScalerImg;
#endif //ISP_SUPPORT_SCALER
// end added by linyun.xiong @2015-12-11

        
/**
 * @brief Save images from ISPC::Shot to disk
 *
 * Once open for a particular type allows saving multiple frames to the same 
 * file.
 *
 * @note If type is Bytes needs to use a specialised save function
 */
class Save
{
public:
    enum SaveType {
        Bayer, /**< @brief Data Extraction (Bayer) */
        Bayer_TIFF, /**< @brief Raw2D Extraction (Bayer saved as TIFF) */
        RGB, /**< @brief Display output (RGB) */
        RGB_EXT, /**< @brief HDR Extraction format */
        RGB_INS, /**< @brief HDR Insertion format */
        YUV, /**< @brief Encoder output (YUV) */

        Bytes /**< @brief Special types such as DPF output */
    };

protected:
    /** @brief output filename */
    std::string filename;
    /** @brief save context */
    SaveFile *file;
    /** @brief file format */
    SaveType type;
    /**
     * @brief either 256 or 512 scheme
     *
     * Populated when opening the file from Camera's information
     */
    unsigned int tileScheme;

    /**
     * @name Specialised open functions
     * @brief Specialised functions to open each format supported by SaveType
     * @{
     */

    /** @brief Open RGB output */
    IMG_RESULT openRGB(ePxlFormat fmt, unsigned int w, unsigned int h);
    /**
     * @note Uses attached context's sensor to get the output size
     *
     * @copydoc openRGB
     * @return IMG_ERROR_FATAL if attached context does not have a sensor
     */
    IMG_RESULT openBayer(ePxlFormat fmt, MOSAICType mosaic,
        unsigned int w, unsigned int h);
    /** @copydoc openBayer() */
    IMG_RESULT openTiff(ePxlFormat fmt, MOSAICType mosaic,
        unsigned int w, unsigned int h);
    /**
     * @note Also used for YUV
     *
     * @see Delegates to SaveFile_open()
     * @return IMG_SUCCESS
     */
    IMG_RESULT openBytes();

    /**
     * @}
     * @name Specialised save functions
     * @brief Specialised functions to save each format supported by SaveType
     * @{
     */

    /**
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a BAYER 
     * buffer
     * @return IMG_ERROR_FATAL if conversion to Bayer format with 
     * convertToPlanarBayer() failed
     */
    IMG_RESULT saveBayer(const ISPC::Buffer &bayerBuffer);
    IMG_RESULT saveBayer(const ISPC::Buffer &bayerBuffer, int nCam = 1);
    /**
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a RAW2DEXT 
     * buffer
     * @return IMG_ERROR_FATAL if conversion to Bayer format with 
     * convertToPlanarBayerTiff() failed
     */
    IMG_RESULT saveTiff(const ISPC::Buffer &raw2dBuffer);
    /**
     * @see Delegates to SaveFile_writeFrame()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a RGB buffer
     */
    IMG_RESULT saveRGB(const ISPC::Buffer &rgbBuffer);

    /**
     * @brief Save HDR insertion buffer as RGB file
     *
     * @warning assumes there is no offset in the HDF buffer
     */
    IMG_RESULT saveRGB(const CI_BUFFER &hdfBuffer);

    /**
     * @see Delegates to SaveFile_writeFrame()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a YUV buffer
     */
    IMG_RESULT saveYUV(const ISPC::Buffer &yuvBuffer, int nCam = 1);
	//begin added by linyun.xiong@2015-12-11
#ifdef ISP_SUPPORT_SCALER   
	IMG_RESULT fetchEscYUV(/*const */ISPC::Buffer &yuvBuffer, ISPC::Buffer &result);
#endif //ISP_SUPPORT_SCALER
	//end added by linyun.xiong@2015-12-11

    /**
     * @}
     * @name Specialised untile functions
     * @{
     */

    /**
     * @brief Create new untiled buffer (with new memory!)
     *
     * @param[in] buffer input buffer
     * @param[in] tileW in pixels (expecting 256 or 512)
     * @param[in] tileH in pixels (expecting 16 or 8)
     * @param[out] result output untiled buffer
     * @warning Allocates new memory in result.data that needs to be freed
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if given buffer is not tiled
     * @return IMG_ERROR_NOT_SUPPORTED if given buffer is not YUV
     * @return IMG_ERROR_MALLOC_FAILED if allocation of new memory failed
     * @return IMG_ERROR_FATAL if detiling failed
     */
    static IMG_RESULT untileYUV(const ISPC::Buffer &buffer,
        unsigned int tileW, unsigned int tileH, ISPC::Buffer &result);

    /**
     * @brief Create new untiled buffer (with new memory!)
     *
     * Works for both HDR and RGB
     *
     * @param[in] buffer input buffer
     * @param[in] tileW in pixels (expecting 256 or 512)
     * @param[in] tileH in pixels (expecting 16 or 8)
     * @param[out] result output untiled buffer
     * @warning Allocates new memory in result.data that needs to be freed
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if given buffer is not tiled
     * @return IMG_ERROR_NOT_SUPPORTED if given buffer is not YUV
     * @return IMG_ERROR_MALLOC_FAILED if allocation of new memory failed
     * @return IMG_ERROR_FATAL if detiling failed
     */
    static IMG_RESULT untileRGB(const ISPC::Buffer &buffer,
        unsigned int tileW, unsigned int tileH, ISPC::Buffer &result);
    /**
     * @}
     */

public:
    /** @brief Create a save object based on a Pipeline's configuration */
    Save();
    virtual ~Save();

    const std::string & getFilename() const;

    bool isOpened() const;

    SaveType format() const;

    /**
     * @brief Open the file with a given type
     *
     * @see Delegates to one of the specialised open functions
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if type is invalid
     * @return IMG_ERROR_ALREADY_INITIALISED if file is already opened
     *
     */
    IMG_RESULT open(SaveType type, const ISPC::Pipeline &context,
        const std::string &filename);

    /**
     * @brief Close an opened file
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPPORTED if the file is already closed
     */
    IMG_RESULT close();

    /**
     * @brief Save the Shot's image to the file according to Save's type
     *
     * @warning Save::type must be different from Bytes.
     *
     * Will use the correct protected save according to the current type if the
     * Shot contains the right image.
     *
     * If the type is RGB, YUV or RGB_EXT (HDR) it will untile the buffer
     * before saving it.
     *
     * Look at @ref saveDPF() @ref saveENS() @ref saveStats() for non-image 
     * file formats (Bytes).
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if file is not opened
     * @return IMG_ERROR_NOT_SUPPORTED if type is Bytes
     * Delegates to @ref saveYUV(), @ref saveRGB(), @ref saveBayer(), 
     * @ref saveTiff() and saveHDRExt() for saving.
     * Delegates to @ref untileYUV() and untileRGB() to untile buffers (latter
     * used for both RGB and HDR).
     */
    IMG_RESULT save(const Shot &shot, int nCam = 1);

    /**
     * @brief Until the Shot's image into a new allocated Buffer
     *
     * Will use the correct protected untile according to the current type if
     * the shot contains the right image.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPPORTED if type cannot be untiled
     * Delegates to @ref untileYUV() and untileRGB() to untile buffers (latter
     * used for both RGB and HDR).
     */
    IMG_RESULT untile(const Shot &shot, Buffer &result);

    /**
     * @brief Save the Shot's DPF information to the file
     *
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS (even if no corrections are saved)
     * @return IMG_ERROR_NOT_SUPPORTED if Save::type is not Bytes
     * @return IMG_ERROR_NOT_INITIALISED of file is not opened
     */
    IMG_RESULT saveDPF(const Shot &shot);

    /**
     * @brief Save the Shot's ENS to the file
     *
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS (even if no corrections are saved)
     * @return IMG_ERROR_NOT_SUPPORTED if Save::type is not Bytes
     * @return IMG_ERROR_NOT_INITIALISED of file is not opened
     */
    IMG_RESULT saveENS(const ISPC::Shot &shot);

    /**
     * @brief Save the Shot's statistics to the file
     *
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS (even if statistics were partially saved)
     * @return IMG_ERROR_NOT_SUPPORTED if Save::type is not Bytes
     * @return IMG_ERROR_NOT_INITIALISED of file is not opened
     */
    IMG_RESULT saveStats(const ISPC::Shot &shot);

    /**
     * @brief Produces an output filename using YUV's configuration from the
     * given Pipeline object
     */
    static std::string getEncoderFilename(const ISPC::Pipeline &context);

    /**
     * @brief Create, open and save a Shot's image in a single function
     *
     * If wanting to save multi-frames in the same output file use a Save 
     * object, otherwise it may be simpler to use this function.
     *
     * @see Delegates to Save::open() and Save::save()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPPORTED if given type is Save::Bytes
     */
    static IMG_RESULT Single(const ISPC::Pipeline &context,
        const ISPC::Shot &shot, ISPC::Save::SaveType type,
        const std::string &filename);

    /**
     * @brief Create, open and save a Shot DPF data in a single function
     *
     * If wanting to save multi-frames in the same output file use a Save 
     * object, otherwise it may be simpler to use this function.
     */
    static IMG_RESULT SingleDPF(const ISPC::Pipeline &context,
        const ISPC::Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a Shot ENS data in a single function
     *
     * If wanting to save multi-frames in the same output file use a Save 
     * object, otherwise it may be simpler to use this function.
     */
    static IMG_RESULT SingleENS(const ISPC::Pipeline &context,
        const ISPC::Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a Shot save_structure (statistics)
     * 
     * If wanting to save multi-frames in the same output file use a Save
     * object, otherwise it may be simpler to use this function
     */
    static IMG_RESULT SingleStats(const ISPC::Pipeline &context,
        const ISPC::Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a HDF insertion buffer
     */
    static IMG_RESULT SingleHDF(const ISPC::Pipeline &context,
        const CI_BUFFER &hdrBuffer, const std::string &filename);
//begin added by linyun.xiong@2015-12-11
#ifdef ISP_SUPPORT_SCALER   
	static IMG_RESULT GetEscImg(ISPC::ScalerImg *EscImg, const ISPC::Pipeline &context, const ISPC::Shot &shot);
#endif
//end added by linyun.xiong@2015-12-11
};

} /* namespace ISPC */

#endif /* ISPC_AUX_SAVE_H_ */
