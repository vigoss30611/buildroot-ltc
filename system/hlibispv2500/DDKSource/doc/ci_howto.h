/**
 * @file ci_howto.h
 *
 * This header is not intended to be included when developing code. It contains some doxygen documentation
 */
 
/**
 * @page CI_USER_howToUse Capture Interface: Getting started
 * 

 @section CI_USER_howToUse_Setup Using the Capture Interface
 
 The CI is responsible for the interaction with the kernel-module side driver.
 This section is going to explain what are the steps to set it up in order to run a capture.
 
 @ talk about MC
 
 @subsection CI_USER_howToUse_Setup_config Configuration of the Pipeline
 
 The first step is to configure the capture (function run_drivers() in the example).
 Once configured the capture parameters have to be <i>registered</i> to the kernel-side (or updated if they were already registered).
 Capture buffers can then be added (the configuration defines the amount of memory needed) and finally the capture can be started.
 Once the capture is started captures can be triggered and frame retrieves as explained in the next section.
 
 @li Connect with the kernel-module (it is using the open() system call): CI_DriverInit()
 @li Create the user-side pipeline object: CI_PipelineCreate()
 @li [optional] Configure the pipeline object using the MC layer (this is done in a few more steps detailed below).
 If the MC layer is not used to configure the pipeline you must configure every CI structures with the appropriate register values.
 @li Register your pipeline object: CI_PipelineRegister()
 @li Add available buffers: CI_PipelineAddPool()
 @li Start the capture (ask for the ownership of the HW context used): CI_PipelineStartCapture()
 
 The configuration itself needs a few steps (at this point the MC pipeline and CI pipeline structures are available) (function configureMC() in the example).
 @li configure the MC IIF module. This module also controls which imager is used: MC_IIF::ui8Imager
 @li [optional] initialise the MC pipeline values: MC_PipelineInit() - this will put default values to all MC modules according to the values that are in the MC_IIF
 @li configure the CI only parameters (CI_PIPELINE::ui8Context, CI_PIPELINE::eOutputConfig, CI_PIPELINE::ui8PrivateValue)
 @li configure the MC output format (MC_PIPELINE::eEncOutput, MC_PIPELINE::eDispOutput, MC_PIPELINE::eDEPoint)
 @li configure every MC module
 @li Apply the MC configuration on the CI pipeline: MC_PipelineConvert()
 
 @note The parameters that are not accessed by the MC should be the one that are not part of any module structure.
 
 @subsection CI_USER_howToUse_Setup_run Run the capture
 
 Once the capture is started shots can be triggered and captured retrieved (function run_capture() in the example).
 @li trigger a shot using CI_PipelineTriggerShoot() or CI_PipelineTriggerShootNB() (will wait until HW waiting list has an available slot or will return and error)
 @li retrieve a capture result using CI_PipelineAcquireShot() or CI_PipelineAcquireShotNB() (will wait until a frame is available or will return and error)
 @li use the frame information (e.g. check if CI_SHOT::bFrameError is set)
 @li release the acquired shot (so that it can be reused in a later trigger): CI_PipelineReleaseShot()
 
 Once all the capture are done, the capture should be stopped.
 
 @subsection CI_USER_howToUse_Setup_stop Stopping and cleaning
 
 The stopping and cleaning is done with a few functions calls:
 @li The capture can be stooped using CI_PipelineStopCapture().
 This will remove the ownership of the HW context and therefore no more capture can be triggered until it is re-started.
 @li Destroying the pipeline configuration: CI_PipelineDestroy()
 This will destroy all user space and kernel space objects (including the capture frames).
 @li Disconnecting from the kernel module: CI_DriverFinalise()
 This will prevent new pipeline objects from being created and registered to the kernel-module.
 
 @note You can create and destroy as many pipeline object as you want per connection to the kernel module.
 
 @subsection CI_USER_howToUse_Setup_code Example Code
 
 @code // example derived from driver_test
int run_drivers(param *parameters); // parameters contains the information about the run, such as the image size
int configureMC(parameters, CI_PIPELINE *pConfig, CI_CONNECTION *pDriver); // configure the CI pipeline using the MC simplified layer 
int run_capture(parameters, CI_PIPELINE *pCapture);

int run_drivers(param *parameters)
{
	CI_CONNECTION *pDriver = NULL;
	int returnCode = EXIT_SUCCESS;
	
	CI_PIPELINE *pConfig = NULL;
	
	// connection to the kernel side
	if ( CI_DriverInit(&pDriver) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}
	
	// creation of the pipeline object related to that capture
	if ( CI_PipelineCreate(&pConfig) != IMG_SUCCESS )
	{
		returnCode = EXIT_FAILURE;
		goto cleanup;
	}
	
	// configure the HW modules for the capture
	if ( (returnCode = configureMC(pConfig, parameters)) != EXIT_SUCCESS )
	{
		goto cleanup;
	}
	
	// register the pipeline object to the kernel side, later update will need to use CI_PipelineUpdate()
	if ( CI_PipelineRegister(pConfig) != IMG_SUCCESS )
	{
		returnCode = EXIT_FAILURE;
		goto cleanup;
	}

	// add available buffers
	if ( CI_PipelineAddPool(pConfig, parameters->ui32NumberBuffers) != IMG_SUCCESS )
	{
		returnCode = EXIT_FAILURE;
		goto cleanup;
	}
	
	// acquire configure HW for capture
	if ( CI_PipelineStartCapture(pConfig) != IMG_SUCCESS )
	{
		returnCode = EXIT_FAILURE;
		goto cleanup;
	}
	
	run_capture(parameters, pConfig);
	
cleanup:
	if ( pConfig != NULL )
	{
		CI_PipelineStopCapture(pConfig); // this will fail if not started but it's OK
		
		CI_PipelineDestroy(pConfig);
	}
	
	CI_DriverFinalise();
	return returnCode;
}

int configureMC(parameters, CI_PIPELINE *pConfig, CI_CONNECTION *pDriver)
{
	MC_PIPELINE sPipelineConfig;
	
	if ( MC_IIFInit(&(sPipelineConfig.sIIF)) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}
	
	// setup everything in the imager interface or that is context related
	sPipelineConfig.sIIF.eBayerFormat = MOSAIC_RGGB;
	sPipelineConfig.sIIF.ui8Imager = parameters->ui8imager;
	sPipelineConfig.sIIF.ui16ImagerSize[0] = parameters->uiWidth;
	sPipelineConfig.sIIF.ui16ImagerSize[1] = parameters->uiHeight;
	// leave offset to 0
	
	// configure default values for all MC modules
	if ( MC_PipelineInit(&sPipelineConfig, &(pDriver->sHWInfo)) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}

	// default output is NO output
	sPipelineConfig.eEncOutput = YUV_NONE;
	sPipelineConfig.eDispOutput = RGB_NONE;
	pConfig->eEncType.eBuffer = TYPE_NONE;
	pConfig->eDispType.eBuffer = TYPE_NONE;
	
	// CI pipeline only parameters
	pConfig->ui8Context = parameters->hwContext;
	if ( parameters->bNoStats == IMG_TRUE )
	{
		pConfig->eOutputConfig = CI_SAVE_NO_STATS;
	}

	// configure the modules
	
	// example: configuration of the scaler
	if ( parameters->bHasEncoderOutput )
	{
		sPipelineConfig.eEncOutput = YVU_420_PL12_8; // could also choose other YUV formats from felix_common/pixel_format.h
		
		if ( parameters->escPitch == 1.0 )
		{
			sPipelineConfig.sESC.bBypassESC = IMG_TRUE; // no scale down
		}
		else
		{
			sPipelineConfig.sESC.aPitch[0] = parameters->escPitch; // using same pitch vertically and horizontally
			sPipelineConfig.sESC.aPitch[1] = parameters->escPitch;
			sPipelineConfig.sESC.aOutputSize[0] = (IMG_UINT16)sPipelineConfig.sIIF.ui16ImagerSize[0]/parameters->escPitch;
			sPipelineConfig.sESC.aOutputSize[1] = (IMG_UINT16)sPipelineConfig.sIIF.ui16ImagerSize[1]/parameters->escPitch;

			if ( sPipelineConfig.sESC.aOutputSize[0]%2 != 0 ) // insure size is even
			{
				sPipelineConfig.sESC.aOutputSize[0]++;
			}
		}
	}

	// apply the configuration on the CI pipeline
	if ( (MC_PipelineConvert(&sPipelineConfig, pConfig) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int run_capture(parameters, CI_PIPELINE *pCapture)
{
	CI_SHOT *pBuffer = NULL;
	
	// trigger the capture, it will be added to the list - will fail if there is no available buffer and will wait if HW waiting list is full
	if ( CI_PipelineTriggerShoot(pCapture) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}
	
	if ( CI_PipelineAcquireShot(pCapture, &pBuffer) != IMG_SUCCESS ) // blocking call - will wait until a shot is available
	{
		return EXIT_FAILURE;
	}
	
	if ( pBuffer->bFrameError )
	{
		fprintf(stderr, "Error while capturing\n");
	}
	
	if ( CI_PipelineReleaseShot(pCapture, pBuffer) != IMG_SUCCESS )
	{
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
 @endcode
 
 @section CI_USER_howToUse_FakeDev Using the Fake Device
 
 This is how to use the CI in user space running with a fake device (i.e. the simulator).
 In this mode no kernel module is created, therefore some code must be executed before CI_DriverInit() to have a similar effect than the one of insmod().
 This code is inspired by what is done in the driver_test project.
 
 The kernel module initialises the CI and DG at the same time (they are part of the same kernel module file).
 Therefore the same should be done when faking the behaviour of the insmod() process.
 
 @subsection CI_USER_howToUse_FakeDev_cmake CMake
 
 In CMake you will need to find both FelixAPI and FelixDG and use their variables.
 You can know if felix is using a kernel build using the CMake option <i>FELIX_BUILD_KERNEL_MODULE</i> (on WIN32 this is always false, on linux it is true by default).
 You will also be interest to copy the <i>FELIXAPI_INSTALL</i> files into your working directory (the fake mode uses Pthread as an external library and you will need the dll on windows).
 This variable also contains the path to the TAL config file use.
 @verbatim # CMakeLists of your project
find_package(FelixAPI REQUIRED)
if (FELIX_BUILD_KERNEL_MODULE)
   find_package(FelixDG REQUIRED)
endif()

include_directories(
	${FELIXAPI_INCLUDE_DIRS}
	${FELIXDG_INCLUDE_DIRS}
)

add_definitions(
	${FELIXAPI_DEFINITIONS}
	${FELIXDG_DEFINITIONS}
)

if (DEFINED FELIXAPI_INSTALL)
	file(COPY ${FELIXAPI_INSTALL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# other things for you library defining SOURCES

add_executable(myApp ${SOURCES})
target_link_libraries(myApp ${FELIXAPI_LIBRARIES} ${FELIXDG_LIBRARIES})
add_dependencies(myApp ${FELIXAPI_DEPENDENCIES}) # dependencies to register definition and exeternal libs
@endverbatim

 @subsection CI_USER_howToUse_FakeDev_code Code
 
 In the code you will have to initialise the CI and DG modules.
 The application can know if CI is using the fake device by verifying that <i>FELIX_FAKE</i> preprocessor is defined.
 The path to TAL configuration file can simply be the name of the file you copied in CMake: "Felix_SIM_TAL_Config.txt" (as long as it was copied into the running directory of your application).
 
 @note The ui8PudmpFlags value should be: 0 for no dumping, 1 for single pdump file, 2 for several pdump files (1 per HW context).
 
 @code // in your application source file
#ifdef FELIX_FAKE
#include <ci_kernel/ci_kernel.h>
#include <dg_kernel/dg_camera.h>
#endif

// if using previous section's example this should be called before run_drivers()
int init_kernel(IMG_BOOL8 bUseMMU, IMG_UINT8 ui8PdumpFlag)
{
#ifdef FELIX_FAKE
	static KRN_CI_DRIVER sCIDriver; // resilient to the function but could be part of another structure
	static KRN_DG_DRIVER sDGDriver; // resilient to the function but could be part of another structure
	
	if ( KRN_CI_DriverCreate(&sCIDriver, bUseMMU, ui8PdumpFlag, "Felix_SIM_TAL_Config.txt") != IMG_SUCCESS )
	{
		fprintf(stderr, "Failed to create global CI driver\n");
		return EXIT_FAILURE;
	}
	if ( KRN_DG_DriverCreate(&sDGDriver) != IMG_SUCCESS )
	{
		fprintf(stderr, "Failed to create global DG driver\n");
		KRN_CI_DriverDestroy(&sCIDriver);
		return EXIT_FAILURE;
	}
#else
	(void)bUseMMU;
	(void)ui8PdumpFlag;
	(void)pszTalFile;
#endif
	return EXIT_SUCCESS;
}

// if using the previous section's example this should be called after run_drivers() terminated
void finalise_kernel()
{
#ifdef FELIX_FAKE
	KRN_DG_DriverDestroy(g_pDGDriver); // pointer to the currently used driver from <dg_kernel/dg_camera.h>
	KRN_CI_DriverDestroy(g_psCIDriver); // pointer to the currently used driver from <ci_kernel/ci_kernel.h>
#endif
}
@endcode 
 
*
*/ // end of page CI_USER_howToUse
