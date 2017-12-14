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
#include <ostream>

/** @brief spaces to format text statistics - level 1 */
#define SAVE_A1 "  "
/** @brief spaces to format text statistics - level 2 */
#define SAVE_A2 SAVE_A1 SAVE_A1
/** @brief spaces to format text statistics - level 3 */
#define SAVE_A3 SAVE_A2 SAVE_A1

extern "C" {
    struct CI_HWINFO;  // defined in ci/ci_api_struct.h
}

namespace ISPC {
class Camera;  // defined in ispc/Camera.h
class Pipeline;  // defined in ispc/Pipeline.h
class Shot;  // defined in ispc/Shot.h
struct Buffer;  // defined in ispc/Shot.h

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
        Display, /**< @brief Display output (RGB or Ycc444) */
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

    /** @brief Open Display output */
    IMG_RESULT openDisplayFile(ePxlFormat fmt, unsigned int w, unsigned int h);
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
    IMG_RESULT saveBayer(const Buffer &bayerBuffer);
    /**
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a RAW2DEXT
     * buffer
     * @return IMG_ERROR_FATAL if conversion to Bayer format with
     * convertToPlanarBayerTiff() failed
     */
    IMG_RESULT saveTiff(const Buffer &raw2dBuffer);
    /**
     * @bries saves output from display RGB or Ycc444
     * @see Delegates to SaveFile_writeFrame()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a RGB buffer
     */
    IMG_RESULT saveDisplay(const Buffer &rgbBuffer);

    /**
     * @bries saves output from display RGB or Ycc444
     * @see Delegates to SaveFile_writeFrame()
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a RGB buffer
     */
    IMG_RESULT saveRGB(const Buffer &rgbBuffer);

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
    IMG_RESULT saveYUV(const Buffer &yuvBuffer);

    /**
     * @see Delegates to SaveFile_writeFrame()
     *      serves YCC packed formats
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPOPRTED if given shot does not have a YUV buffer
     */
    IMG_RESULT saveYUV_Packed(const ISPC::Buffer &yuvBuffer);

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
    static IMG_RESULT untileYUV(const Buffer &buffer,
        unsigned int tileW, unsigned int tileH, Buffer &result);

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
    static IMG_RESULT untileRGB(const Buffer &buffer,
        unsigned int tileW, unsigned int tileH, Buffer &result);
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
    IMG_RESULT open(SaveType type, const Pipeline &context,
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
    IMG_RESULT save(const Shot &shot);

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
    IMG_RESULT saveENS(const Shot &shot);

    /**
     * @brief Save the Shot's statistics to the file
     *
     * Saves the stats are raw binary from HW. To save stats readable by
     * humans use @ref saveTxtStats()
     *
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS (even if statistics were partially saved)
     * @return IMG_ERROR_NOT_SUPPORTED if Save::type is not Bytes
     * @return IMG_ERROR_NOT_INITIALISED of file is not opened
     */
    IMG_RESULT saveStats(const Shot &shot);

    IMG_RESULT saveCameraState(const Camera &camera);

    /**
     * @brief Save the Shot's statistics to the file in text format
     *
     * The stats are written in human readable values. To save the raw HW
     * format check @ref saveStats().
     *
     * @see Delegates to SaveFile_write()
     * @return IMG_SUCCESS (even if statistics were partially saved)
     * @return IMG_ERROR_NOT_SUPPORTED if Save::type is not Bytes
     * @return IMG_ERROR_NOT_INITIALISED of file is not opened
     */
    IMG_RESULT saveTxtStats(const Shot &shot);

    /**
     * @brief Produces an output filename using YUVs configuration from the
     * given Pipeline object
     */
    static std::string getEncoderFilename(const Pipeline &context);

    /**
     * @brief Produces an output filename using Displays configuration from the
     * given Pipeline object
     *
     * @brief supports YUV444
     */
    static std::string getDisplayFilename(const ISPC::Pipeline &context);

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
    static IMG_RESULT Single(const Pipeline &context,
        const Shot &shot, SaveType type, const std::string &filename);

    /**
     * @brief Create, open and save a Shot DPF data in a single function
     *
     * If wanting to save multi-frames in the same output file use a Save
     * object, otherwise it may be simpler to use this function.
     */
    static IMG_RESULT SingleDPF(const Pipeline &context,
        const Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a Shot ENS data in a single function
     *
     * If wanting to save multi-frames in the same output file use a Save
     * object, otherwise it may be simpler to use this function.
     */
    static IMG_RESULT SingleENS(const Pipeline &context,
        const Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a Shot save_structure (statistics)
     *
     * If wanting to save multi-frames in the same output file use a Save
     * object, otherwise it may be simpler to use this function
     */
    static IMG_RESULT SingleStats(const Pipeline &context,
        const Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a Shot statistics as text
     *
     * If a Camera is given then also save the Camera state
     *
     * If wanting to save multi-frames in the same output file use a Save
     * object, otherwise it may be simpler to use this function
     */
    static IMG_RESULT SingleTxtStats(const Camera &camera,
        const Shot &shot, const std::string &filename);

    /**
     * @brief Create, open and save a HDF insertion buffer
     */
    static IMG_RESULT SingleHDF(const Pipeline &context,
        const CI_BUFFER &hdrBuffer, const std::string &filename);
};

} /* namespace ISPC */

std::ostream& operator<<(std::ostream &os, const CI_HWINFO &info);

#endif /* ISPC_AUX_SAVE_H_ */
