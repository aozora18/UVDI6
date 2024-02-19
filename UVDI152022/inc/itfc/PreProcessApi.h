/**********************************************************************************************************************
 ** PreProcessApi.h - Pre-process API interface definition
 ** 
 ** Copyright ?2017. VISITECH AS
 **
 ** Defines all interface functions in PreProcessApi.dll that is needed to perform pre-processing, and then be able to
 ** expose the gerber on a machine with LLS RLT photoheads.
 **
 ** The results from pre-processing will be loaded as a job in LURIA before exposure.
 **********************************************************************************************************************/

#ifndef __PREPROCESS_API_H__
#define __PREPROCESS_API_H__

#ifdef PP_EXPORTS
#define PP_API extern "C" __declspec(dllexport) 
#else
#define PP_API extern "C" __declspec(dllimport) 
#endif



// --------------- STATUS CODES ---------------------------------------------------------------------------------------

enum PPResult
{
	SUCCESS=0,
	INVALID_INPUT,
	CONVERSION_FAIL,
	PIXEL_CALC_FAIL,
	FIDUCIAL_FAIL,
	PANEL_DATA_FAIL,
	SAVE_PREVIEW_FAIL,
	STRIPE_SPLIT_FAIL,
	SAVE_TO_FILE_FAIL,
	RESULT_FOLDER_FAIL,
	LOGGING_FAILURE,
	LICENSE_ERROR,
	INIT_ERROR,
	ALREADY_RUNNING,
	EXIT_ERROR,
	DLL_PATH_ERROR,
	LOAD_FROM_FILE_ERROR,
	TEXT_TOO_LONG_ERROR,
	INTERNAL_ERROR,
	OPC_FAIL,
	ABORTED,
	ODBPP_NOT_LOADED = 0xffff,
	ODBPP_NOT_PROCESSED,
	ODBPP_OUT_OF_RANGE,
	ODBPP_ALREADY_PROCESSED,
	ODBPP_INTERMEDIATE_FILE_ERROR,
	ODBPP_COULD_NOT_OPEN_FILE,
	ODBPP_COULD_NOT_READ_STEPS,
	ODBPP_COULD_NOT_READ_LAYERS,
	ODBPP_DATABASE_EMPTY,
	ODBPP_NO_SELECTED,
	ODBPP_NOT_ENOUGH_SPACE,
	ODBPP_NO_LICENSE,
	GDSPP_NOT_LOADED,
	GDSPP_NOT_PROCESSED,
	GDSPP_NO_LICENSE,
};


enum PPState
{
	IDLE=100,
	STARTED,
	INITIALIZING,
	RASTERIZING,
	CALCULATING,
	SAVING_PREVIEW,
	STRIPE_WRITING,
	FINALIZING,
	OPC,
	ABORTING,
};

// --------------- ODB DEFINES ---------------------------------------------------------------------------------------

enum OdbContext {
	BOARD=1,
	MISC
};

	
enum OdbType {
	SIGNALLYR=1,
	POWER_GROUND,
	DIELECTRIC,
	MIXED,
	SOLDER_MASK,
	SOLDER_PASTE,
	SILK_SCREEN,
	DRILL,
	ROUT,
	DOCUMENT,
	COMPONENT,
	MASK,
	CONDUCTIVE_PASTE,
	OTHERLAYERTYPE,
	OTHERSM,
	OTHERSS,
};

enum OdbPolarity {
	POSITIVE = 1,
	NEGATIVE
};

// --------------- CORE FUNCTIONS -------------------------------------------------------------------------------------


/**********************************************************************************************************************
 ** InitGlobalLogger - Turn on global logging. All pre-processing actions will be logged to a separate global log file
 **					   in addition to the "per job" logs. File is named PreProGlobal.log and will be located in same
 **					   directory as PreProcessApi.dll. If this function is not called, no global log will be made.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int InitGlobalLogger();

/**********************************************************************************************************************
 ** Init - Initialize and prepare for processing. Will reset all parameters to default except dll_dir from SetDllPath.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int Init();										

/**********************************************************************************************************************
 ** InitGerber - Same as Init but with input and output as parameters.
 **
 ** input_file - full or relative (to bin folder) path of input Gerber file
 ** output_dir - full or relative (to bin folder) path of output job directory
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int InitGerber(const char* input_file, const char* output_dir);

/**********************************************************************************************************************
 ** InitGDS - Initialize and prepare GDSII/OASIS file for processing. NB! Require separate license dongle.
 **
 ** input_file - full or relative (to bin folder) path of input GDSII/OASIS file
 ** output_dir - full or relative (to bin folder) path of output job directory
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int InitGDS(const char* input_file, const char* output_dir, const char* work_dir);

/**********************************************************************************************************************
 ** GDSConvert - Convert initialized GDSII/OASIS file to gerber prepared for rasterization.
 ** 
 ** nesting_level - should normally be kept as default, 0, and the tool will look for repeating objects and find the 
 **					optimal setting. For special designs, e.g. tests design with no repeating objects, this parameter
 **					could be set to 1 to avoid using time looking for this.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int GDSConvert(int nesting_level=0);

 /**********************************************************************************************************************
 ** InitODB - Initialize and prepare ODB database for processing. Same as Init but with input and output as parameters.
 **
 ** input_file - full or relative (to bin folder) path of input ODB file
 ** output_dir - full or relative (to bin folder) path of output job directory
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int InitODB(const char* input_file, const char* output_dir, const char* work_dir);

 /**********************************************************************************************************************
 ** ODBGetStepCount - Get step count in ODB database.
 **
 ** count - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetStepCount(int *count);

 /**********************************************************************************************************************
 ** ODBGetLayerCount - Get layer count in ODB database.
 **
 ** count - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerCount(int *count);

 /**********************************************************************************************************************
 ** ODBGetStepName - Get name of step.
 **
 ** step - step index number. Starts at 0. If OdbGetStepCount indicates 3 steps, valid indexes are 0, 1, 2.
 ** buf - pointer to string buffer. If status code is 0 a zero terminated string is written to pointer address.
 ** bufsz - size of string buffer.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetStepName(int step, char *buf, int bufsz);

 /**********************************************************************************************************************
 ** ODBGetStepDcodeInfo - Get step total Dcode information.
 **
 ** step - step index number. Starts at 0. If OdbGetStepCount indicates 3 steps, valid indexes are 0, 1, 2.
 ** dcode_start - pointer to return argument.
 ** dcode_count - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetStepDcodeInfo(int step, int* dcode_start, int* dcode_count);

 /**********************************************************************************************************************
 ** ODBGetLayerName - Get name of layer.
 **
 ** layer - layer index number. Starts at 0. If OdbGetLayerCount indicates 3 layers, valid indexes are 0, 1, 2.
 ** buf - pointer to string buffer. If status code is 0 a zero terminated string is written to pointer address.
 ** bufsz - size of string buffer.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerName(int layer, char *buf, int bufsz);

 /**********************************************************************************************************************
 ** ODBGetLayerDcodeInfo - Get layer Dcode information.
 **
 ** layer - layer index number. Starts at 0. If OdbGetLayerCount indicates 3 layers, valid indexes are 0, 1, 2.
 ** dcode_start - pointer to return argument.
 ** dcode_count - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerDcodeInfo(int layer, int *dcode_start, int* dcode_count);

 /**********************************************************************************************************************
 ** ODBGetLayerType - Get layer type.
 **
 ** layer - layer index number. Starts at 0. If OdbGetLayerCount indicates 3 layers, valid indexes are 0, 1, 2.
 ** type - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerType(int layer, int* type);

 /**********************************************************************************************************************
 ** ODBGetLayerContext - Get layer context.
 **
 ** layer - layer index number. Starts at 0. If OdbGetLayerCount indicates 3 layers, valid indexes are 0, 1, 2.
 ** context - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerContext(int layer, int* context);

 /**********************************************************************************************************************
 ** ODBGetLayerPolarity - Get layer polarity.
 **
 ** layer - layer index number. Starts at 0. If OdbGetLayerCount indicates 3 layers, valid indexes are 0, 1, 2.
 ** polarity - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetLayerPolarity(int layer, int* polarity);

 /**********************************************************************************************************************
 ** ODBSetActiveLayerAndStep - Set layer and step to be active when preprocessing.
 ** NOTE: Set desired work directory with SetWorkDir before using this function. Some temporary files are created.
 **
 **	step - selected step to use for other preprocessing functions.
 **	layer - selected layer to use for other preprocessing functions.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBSetActiveLayerAndStep(int step, int layer);

 /**********************************************************************************************************************
 ** ODBGetActiveLayerAndStep - Get currently selected layer and step.
 **
 **	step - pointer to return argument.
 **	layer - pointer to return argument.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
 PP_API int ODBGetActiveLayerAndStep(int* step, int* layer);

/**********************************************************************************************************************
 ** StartPreprocessing - Start processing the input and generate job output.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int StartProcessing();		

/**********************************************************************************************************************
 ** StartPreprocessingAsync - Start processing the input and generate job output. Process is started in an asynchronous
 **							  thread and function call returns immediately after thread is started. To be able to know
 **							  when processing is done, you have to poll for result status PPResult::ALREADYRUNNING with 
 **							  GetProcessResult(). With async processing you get the option to poll for current state and 
 **							  progress also.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int StartProcessingAsync();

/**********************************************************************************************************************
 ** GetProcessResult - Used for polling result/status when StartProcessingAsync is called. Returns the result status of
 **					   the async pre-processing if completed. If pre-processing thread is still running, 
 **					   PPResult::ALREADYRUNNING will be returned and you should continue polling. 
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int GetProcessResult();

/**********************************************************************************************************************
 ** GetProcessState - Used to check current state of the processing during polling when StartProcessingAsync is called.
 **					  GetProcessResult should always be called before this in the polling sequence.
 **
 ** Return: PPState
 **********************************************************************************************************************/
  PP_API int GetProcessState();

/**********************************************************************************************************************
 ** GetProcessProgress - Used to check progress of the processing during polling when StartProcessingAsync is called.
 **					     GetProcessResult should always be called before this in the polling sequence. The progress is
 **						 is just an estimate given in % and might take shorter or longer time than indicated.
 **
 ** Return: Processing progress given in %. Start = 0, Finished = 100.
 **********************************************************************************************************************/
  PP_API int GetProcessProgress();

/**********************************************************************************************************************
 ** GetErrMsg - Can be called after a failed processing for more detailed error information.
 **
 ** str	- string buffer owned by the caller, error information will be stored to this buffer
 ** len - length of the string buffer
 **
 ** Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **			Text will be returned but truncated to buffer size.
 **********************************************************************************************************************/
  PP_API int GetErrMsg(char* str, int len);				

/**********************************************************************************************************************
 ** UpdateFiducials - Update fiducial coordinates for an existing pre-processed job configuration, rlt_settings.xml.
 **					  Existing rlt_settings.xml will be renamed to rlt_settings_old.xml.
 **
 ** gerber		- full or relative (to bin folder) path to gerber file where fiducial coordinates should be extracted
 **				  from
 ** job_folder	- full or relative (to bin folder) path to existing job folder where fiducials should be updated
 ** global_fid	- global fiducial dcode value to update job with
 ** local_fid	- local fiducial dcode value to update job with
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int UpdateFiducials(const char* gerber, const char* job_folder, int global_fid, int local_fid);
  
/**********************************************************************************************************************
 ** PreAnalyze - "Dry-run" gerber analyze/convert to be able to ask for actual image width and height in mm.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int PreAnalyze();

/**********************************************************************************************************************
 ** GetImageWidth - Possible to call after PreAnalyze to be able to get the actual width of gerber window.
 **
 ** Return: width of the image in mm.
 **********************************************************************************************************************/
  PP_API double GetImageWidth();
  
/**********************************************************************************************************************
 ** GetImageHeight - Possible to call after PreAnalyze to be able to get the actual height of gerber window.
 **
 ** Return: height of the image in mm.
 **********************************************************************************************************************/
  PP_API double GetImageHeight();
 
/**********************************************************************************************************************
 ** GetPPVersion - Read out the bundle version of the PreProcessApi dll. If the version is not part of a bundle, the
 **				   the version string returned will be 9999.9999.9999.9999. If failed to identify the file the bundle
 **				   version file lls2_bundle_version.txt, then X.X.X.X is returned.
 **
 ** str	- string buffer owned by the caller, version information will be stored to this buffer
 ** len - length of the string buffer
 **
 ** Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API int GetPPVersion(char* str, int len);

/**********************************************************************************************************************
 ** GetPPModuleVersion - Read out the current PreProcess API DLL module version.
 **
 ** str	- string buffer owned by the caller, version information will be stored to this buffer
 ** len - length of the string buffer
 **
 ** Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API int GetPPModuleVersion(char* str, int len);

/**********************************************************************************************************************
 ** GetResultPath - Read out path to pre-processed result job folder after a pre-processing is done. 
 **
 ** str	- string buffer owned by the caller, full path to result folder will be stored to this buffer
 ** len - length of the string buffer
 **
 ** Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API int GetResultPath(char* str, int len);

/**********************************************************************************************************************
 ** SetDllPath - Specify explicitly the directory that should be "current directory" for PreProcessAPI.dll. This is the
 **				 directory where all dependency files should be located. Log/settings files will also be saved here.
 **				 This can be usefult if PreProcessAPI is called from another process with a different location.
 **
 ** dll_dir - full or relative (to callers current directory) path to where PreProcessApi.dll and it's dependencies
 **			  is located.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int SetDllPath(const char* dll_dir);

/**********************************************************************************************************************
 ** SaveParameters - Save job parameter settings to file to be able to load them again later. Paneldata are not saved.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int SaveParameters();

/**********************************************************************************************************************
 ** LoadParameters - Load job parameter settings from file if previously saved. If no previous saved file found a new
 **					 one is created with default values.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int LoadParameters();


  PP_API void AbortProcessing();



// --------------- PANEL DATA -----------------------------------------------------------------------------------------


/**********************************************************************************************************************
 ** AddStaticPanelData - Add additional gerber file containing logo, QR code etc to be pasted in the image at given
 **						 location in original gerber with given orientation parameters.
 **
 **	logo		- full or relative (to bin folder) path to gerber file containing the logo to be pasted in the image.
 ** dcode		- D-code value (in original gerber) where the logo should be placed.
 ** rotation	- counter clockwise rotation of the logo relative to gerber orientation, given in degrees. Valid values
 **				  are 0, 90, 180 and 270. Rotation is applied before mirror/flip.
 ** inverse		- specify if logo colors (black/white) should be inverted (negative).
 ** flipH		- specify if logo should be flipped/mirrored horizontal.
 ** flipY		- specify if logo should be flipped/mirrored vertically.
 ** scaleX		- specify scaling factor for the logo in X. 1=no scaling.
 ** scaleY		- specify scaling factor for the logo in Y. 1=no scaling.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int AddStaticPanelData(const char* logo, int dcode, int rotation, bool inverse, bool flipH, bool flipV,
								   double scaleX, double scaleY);
  
/**********************************************************************************************************************
 ** AddDynamicPanelData - Specifies a frame where dynamic panel data can be placed on the panel (content/text specified 
 **						  in Luria before print). Frame is specified to be located with lower left corner at given
 **						  position. Max number of dynamic panel data fields are 400 (note that there can be many fields
 **						  added with one call of this function if the d-code have multiple locations).
 **
 **	type		- 0=Text, 1=QR-code, 2=ECC200, 3=ECC200-8x32. How to display dynamic data.
 **	content		- 0=BoardSerialNumber, 1=ScalingInformation, 2=GeneralText, 3=DateTime, 4=ScaleInfo-X, 5=ScaleInfo-Y. 
 **				  What content to be displayed as dynamic data.
 ** dcode		- D-code value (in original gerber) where the dynamic panel data should be placed.
 ** rotation	- counter clockwise rotation of the dynamic panel data relative to gerber orientation, given in degrees.
 **				- Rotation will be around lower left corner. Content added in luria later will follow the rotation of
 **				  this frame.
 ** sizeX		- with of the frame where it is allowed to add dynamic panel data in luria before print, given in mm
 ** sizeY		- height of the frame where it is allowed to add dynamic panel data in luria bhefore print, given in mm
 ** inverse 	- inverse the projection of the paneldata, relative to global inverse setting on design
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int AddDynamicPanelData(int type, int content, int dcode, int rotation, double sizeX, double sizeY, bool inverse);


/**********************************************************************************************************************
 ** AddDynamicPanelDataId - Specifies a frame where dynamic panel data can be placed on the panel (content/text specified
 **						  in Luria before print). Frame is specified to be located with lower left corner at positions
 **						  registered to this ID. Max number of dynamic panel data fields are 400 (note that there can be 
 **						  many fields added with one call of this function if the d-code have multiple locations).
 **
 **	type		- 0=Text, 1=BMP-file, 2=ECC200, 3=ECC200-8x32. How to display dynamic data.
 **	content		- 0=BoardSerialNumber, 1=ScalingInformation, 2=GeneralText, 3=DateTime, 4=ScaleInfo-X, 5=ScaleInfo-Y.
 **				  What content to be displayed as dynamic data.
 ** id			- ID for group of coordinates sharing the same properties. Similar to DCODE except coordinates is added
 **				  manually instead of parsed from gerber file.
 ** rotation	- counter clockwise rotation of the dynamic panel data relative to gerber orientation, given in degrees.
 **				- Rotation will be around lower left corner. Content added in luria later will follow the rotation of
 **				  this frame.
 ** sizeX		- with of the frame where it is allowed to add dynamic panel data in luria before print, given in mm
 ** sizeY		- height of the frame where it is allowed to add dynamic panel data in luria bhefore print, given in mm
 ** inverse 	- inverse the projection of the paneldata, relative to global inverse setting on design
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int AddDynamicPanelDataId(int type, int content, int id, int rotation, double sizeX, double sizeY, bool inverse);


/**********************************************************************************************************************
 ** AddDynamicPanelDataPos - Adds a paneldata position to an already defined ID. Coordinates in millimeter and is
 **						  relative to the original gerber origin and coordinate system. It is possible to add multiple
 **						  positions to the same ID.
 **
 **	id			- ID must already be defined by above function AddDynamicPanelDataId.
 ** x_pos		- X coordinate of paneldata position, given in mm.
 ** y_pos		- Y coordinate of paneldata position, given in mm.
 **
 ** Return: PPResult
 **********************************************************************************************************************/
  PP_API int AddDynamicPanelDataPos(int id, double x_pos, double y_pos);





// --------------- SET/GET PARAMETER SETTINGS -------------------------------------------------------------------------
// (NB: Init must be called before set/get parameter functions!)


/**********************************************************************************************************************
 ** InputFile - Path of gerber file to be processed.
 **
 ** Set:
 **		val - full or relative (to bin folder) path of input Gerber file
 **
 ** Get:
 **		str - string buffer owned by the caller, input file path will be stored to this buffer
 **		len - length of the string buffer
 **
 **		Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API void SetInputFile(const char* val);
  PP_API int GetInputFile(char* str, int len);

  /**********************************************************************************************************************
  ** InputFileODB - Path of ODB file to be processed.
  **
  ** Set:
  **		val - full or relative (to bin folder) path of input ODB file
  **
  ** Get:
  **		str - string buffer owned by the caller, input file path will be stored to this buffer
  **		len - length of the string buffer
  **
  **		Return: 0 if info text is longer than callers string buffer size (len param).
  **********************************************************************************************************************/
  PP_API void SetInputFileODB(const char* val); // TODO
  PP_API int GetInputFileODB(char* str, int len); // TODO


/**********************************************************************************************************************
 ** OutputDir - Path of directory to place the result output folder.
 **
 ** Set:
 **		val - full or relative (to bin folder) path of output directory
 **
 ** Get:
 **		str - string buffer owned by the caller, output directory path will be stored to this buffer
 **		len - length of the string buffer
 **
 **		Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API void SetOutputDir(const char* val);
  PP_API int GetOutputDir(char* str, int len);

/**********************************************************************************************************************
 ** WorkDir - Path of temporary work folder.
 **
 ** Set:
 **		val - full or relative (to bin folder) path of work directory
 **
 ** Get:
 **		str - string buffer owned by the caller, work directory path will be stored to this buffer
 **		len - length of the string buffer
 **
 **		Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API void SetWorkDir(const char* val);
  PP_API int GetWorkDir(char* str, int len);

/**********************************************************************************************************************
 ** Suffix - Suffix added to the job name in addition to gerber file name.
 **
 ** Set:
 **		val - set the suffix text that should be added to job name
 **
 ** Get:
 **		str - string buffer owned by the caller, suffix text will be stored to this buffer
 **		len - length of the string buffer
 **
 **		Return: PPResult. TEXT_TOO_LONG_ERROR if info text is longer than callers string buffer size (len parameter).
 **********************************************************************************************************************/
  PP_API void SetSuffix(const char* val);
  PP_API int GetSuffix(char* str, int len);

/**********************************************************************************************************************
 ** Inverse - If image colors (black/white) should be inverted (negative)
 **
 ** Set:
 **		val - Bool, true=inverse colors
 **
 ** Is:
 **		Return: 0=false, 1=true (inverted colors)
 **********************************************************************************************************************/
  PP_API void SetInverse(bool val);
  PP_API int IsInverse();

/**********************************************************************************************************************
 ** InverseFrame - Reverse colors (black/white) of ImageFrame and FadeIn/FadeOut areas only. 
 **				   If used in combination with Inverse, then only gerber design artwork will be inverted and frame kept
 **				   (double inverse = no inverse).
 **
 ** Set:
 **	val - Bool, true=reverse colors of frame
 ** ** Is:
 **		Return: 0=false, 1=true (frame colors reverted)
 **********************************************************************************************************************/
  PP_API void SetInverseFrame(bool val);
  PP_API int IsInverseFrame();

/**********************************************************************************************************************
 ** RotationCCW - Counter clockwise rotation of the image relative to gerber orientation, 
 **				  valid values: 0, 90, 180, 270. Rotation is applied before mirror/flip.
 **
 ** Set:
 **		val - set rotation value in degrees
 **
 ** Get:
 **		Return: rotation value in degrees
 **********************************************************************************************************************/
  PP_API void SetRotationCCW(int val);
  PP_API int GetRotationCCW();

/**********************************************************************************************************************
 ** Mirror - If image should be flipped/mirrored in horizontal/vertical direction. Mirror is applied after rotation.
 **
 ** Set:
 **		val - Bool, true=mirrored in h/v
 **
 ** Is:
 **		Return: 0=false, 1=true (mirrored in h/v)
 **********************************************************************************************************************/
  PP_API void SetMirrorH(bool val);
  PP_API void SetMirrorV(bool val);
  PP_API int IsMirrorH();
  PP_API int IsMirrorV();

/**********************************************************************************************************************
 ** Erosion - Exposure size compensation of artwork elements in x/y-direction. Positive value shrinks all elements and 
 **			  negative value grows all elements.
 **
 ** Set:
 **		val - specify how much artwork elements should grow/shrink in x/y, given in mm.
 **
 ** Get:
 **		Return: how much to grow/shrink in x/y, given in mm.
 **********************************************************************************************************************/
  PP_API void SetErosionX(double val);
  PP_API void SetErosionY(double val);
  PP_API double GetErosionX();
  PP_API double GetErosionY();

/**********************************************************************************************************************
 ** Offset - Offsets image in x/y direction, same as adding a black frame to left/bottom of image.
 **
 ** Set:
 **		val - specify x/y-offset value, given in mm.
 **
 ** Get:
 **		Return: x/y-offset value, given in mm.
 **********************************************************************************************************************/
  PP_API void SetOffsetX(double val);					// Offsets image in x-direction, given in mm.
  PP_API void SetOffsetY(double val);					// Offsets image in y-direction, given in mm.
  PP_API double GetOffsetX();
  PP_API double GetOffsetY();

/**********************************************************************************************************************
 ** ImageFrame - Black frame surrounding the image as well as overscan area for each stripe to avoid loss of image data
 **				 and gaps after registration adjustments.
 **				 Factor must be given in in range 0-5, where 0-ULTRA-LOW equals 128 native pixels, 1-MIN equals 384 
 **				 native pixels, 2-LOW equals 768 native pixels, 3-MEDIUM equals 1152 native pixels, 4-HIGH equals 1536 
 **				 native pixels and 5-MAX equals 1920 native pixels. The given ImageFrame value is sum of both sides, 
 **				 meaning that the actual frame size is ImageFrame divided by 2.
 **
 ** Set:
 **		val - specify frame/overlap factor, value 0-5 (factor of 384, except 0).
 **
 ** Get:
 **		Return: frame/overlap factor, value 0-5 (factor of 384, except 0).
 **********************************************************************************************************************/
  PP_API void SetImageFrame(int val);
  PP_API int GetImageFrame();

/**********************************************************************************************************************
 ** Fiducial - D-code value for global/local fiducial
 **
 ** Set:
 **		val - specify D-code value for global/local fiducial.
 **
 ** Get:
 **		Return: D-code value for global/local fiducial
 **********************************************************************************************************************/
  PP_API void	SetGlobalFiducial(int val);
  PP_API void	SetLocalFiducial(int val);
  PP_API int GetGlobalFiducial();
  PP_API int GetLocalFiducial();
  
/**********************************************************************************************************************
 ** MakePreview - Make a preview bitmap of the image with reduced resolution, so it can be opened in standard image 
 **				  viewers. NB: Enabling this might increases pre-processing time by up to 50%!
 **
 ** Set:
 **		val - Bool, true=make preview image
 **
 ** Is:
 **		Return: 0=false, 1=true (make preview image)
 **********************************************************************************************************************/
  PP_API void SetMakePreview(bool val);
  PP_API int IsMakePreview();

/**********************************************************************************************************************
 ** NegativeYDirection - For table where y-axis is moving in negative y direction, this setting must be enabled. This 
 **						 job will then not be able to print on a table with with positive direction on y axis.
 **
 ** Set:
 **		val - Bool, true=table prints in negative y direction
 **
 ** Is:
 **		Return: 0=false, 1=true (negative y direction)
 **********************************************************************************************************************/
  PP_API void SetNegativeYDirection(bool val);
  PP_API int IsNegativeYDirection();

/**********************************************************************************************************************
 ** PhotoheadNumber - Number of photoheads in the system.
 **
 ** Set:
 **		val - set the number of photoheads
 **
 ** Get:
 **		Return: number of photoheads
 **********************************************************************************************************************/
  PP_API void SetPhotoheadNumber(int val);
  PP_API int GetPhotoheadNumber();

/**********************************************************************************************************************
 ** PhotoheadPitch - Pitch on the system (number of stripes for each head before jump)
 **
 ** Set:
 **		val - set the pitch
 **
 ** Get:
 **		Return: pitch value
 **********************************************************************************************************************/
  PP_API void SetPhotoheadPitch(int val);
  PP_API int GetPhotoheadPitch();

/**********************************************************************************************************************
 ** ProductID - Product ID defines the photohead configuration by means of lens magnification and DMD.
 **				The id consists of 6 digits: LLLLDD
 **				LLLL = Lens magnification factor x 1000
 **				DD = DMD type
 **					Valid DMD types:
 **					01 = 1920x1080, 10.8 um pixel size
 **					02 = 2560x1600, 7.56 um pixel size
 **
 ** Set:
 **		val - Product id: 
 **				200001=LLS50
 **				235002=LLS30
 **				100001=LLS25
 **				100002=LLS15
 **				050001=LLS10
 **				050002=LLS06
 **				025002=LLS04
 **				016702=LLS2500
 **
 ** Get:
 **		Return: Product id
 **********************************************************************************************************************/
  PP_API void SetProductID(int val);
  PP_API int GetProductID();

/**********************************************************************************************************************
 ** PhotoheadRotated - If photohead is mounted rotated or default. Should be set to true if machine have photoheads
 **					   mounted 180 deg rotated vs default (see documentation for default definition).
 **
 ** Set:
 **		val - Bool, true=photoheads is rotated
 **
 ** Is:
 **		Return: 0=false, 1=true (photoheads is rotated)
 **********************************************************************************************************************/
  PP_API void SetPhotoheadRotated(bool val);
  PP_API int IsPhotoheadRotated();

/**********************************************************************************************************************
 ** FixedWindow - Custom window in gerber that is target for pre-processing.
 **				  Window is specified by lower left corner (LLX/LLY) and upper right corner (URX/URY) of the window. 
 **
 ** Set:
 **		val - coordinates (X/Y) of lower left and upper right corner of fixed window in gerber to pre-process, given in 
 **			  mm. If all set to zero, autowindow will be used -> as smal window as possible but all elements must be 
 **			  included.
 **
 ** Get:
 **		Return: lower left and upper right coordinates (X/Y) of the fixed window, given in mm.
 **********************************************************************************************************************/
  PP_API void SetFixedWindow(double LLx, double LLy, double URx, double URy);
  PP_API double GetWindowLLX();
  PP_API double GetWindowLLY();
  PP_API double GetWindowURX();
  PP_API double GetWindowURY();

/**********************************************************************************************************************
 ** NominalPhotoheadDist - Nominal distance from PH #1 to the given photohead. Should be based on mechanical design 
 **						   value and/or measured distance. Must be set after PhotoheadNumber, PhotoheadPitch and
 **						   LensMagnification is set.
 **
 ** Set:
 **		val		  - specify nominal distance between given photohead and the first photohead, given in mm.
 **					Default distance is 0.2 mm less than theoretical distance (dmd_width x pitch) accumulated for all 
 **					heads.
 **		ph_number - given photohead number to specify nominal distance distance to (from #1, to this #2-8).
 **
 **		Return: 1(true)=success, 0(false)=failed (i.e.invalid ph_number input, ph_num must be 2 - 8).	
 **
 ** Get:
 **		ph_number - given photohead number where nominal distance is specified to (from #1 to this #2-8).
 **
 **		Return: nominal distance from photohead #1 to given photohead (ph_number), given in mm.
 **********************************************************************************************************************/
  PP_API int SetNominalPhotoheadDist(double val, int ph_number);
  PP_API double GetNominalPhotoheadDist(int ph_number);

/**********************************************************************************************************************
 ** TableLimitX - Max X position of the right-most photohead to avoid jump motion outside table limits.
 **
 ** Set:
 **		val - specify table motion limit position in X, given in mm. Default=800.
 **
 ** Get:
 **		Return: table motion limit position in X, given in mm.
 **********************************************************************************************************************/
  PP_API void SetTableLimitX(double val);
  PP_API double	GetTableLimitX();

/**********************************************************************************************************************
 ** WhiteLines - White Lines is set to make sure a given number of native pixel lines will overwrite the ImageFrame area
 **				 at top and bottom of each stripe, and set to be white (light on).
 **
 ** Set:
 **		val - set the number of white lines
 **
 ** Get:
 **		Return: the number of white lines
 **********************************************************************************************************************/
  PP_API void SetWhiteLines(int val);
  PP_API int GetWhiteLines();

/**********************************************************************************************************************
 ** ExtraLongStripes - Enable to support 1-buffer solution when printing to enable print extra large jobs in y direction.
 **
 ** Set:
 **		val - Bool, true=long stripe support enabled
 **
 ** Is:
 **		Return: 0=false, 1=true (extra long stipe support)
 **********************************************************************************************************************/
  PP_API void SetExtraLongStripes(bool val);
  PP_API int IsExtraLongStripes();





// --------------------------------------------------------------------------------------------------------------------
// --------------- ADVANCED OPTIONS (normally default values should be kept)-------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


/**********************************************************************************************************************
** Spx - Sub-pixel level of the system, typically 36. Should never be changed unless there are a special need for this.
**
** Set:
**		val - specify sub-pixel level of the system. Valid values: 1, 4, 9, 16, 25, 36. Default=36.
**
** Get:
**		Return: sub-pixel level of the system.
**********************************************************************************************************************/
  PP_API void SetSpx(int val);			// Set sub-pixel level, valid values: 1, 4, 9, 16, 25, 36.
  PP_API int GetSpx();


/**********************************************************************************************************************
 ** Threads - Number of CPU threads that can be ran simultaneously. Should not surpass number of logical cores in CPU.
 **
 ** Set:
 **		val - specify number of CPU threads. Default=number of logical cores on the machines CPU.
 **
 ** Get:
 **		Return: number of CPU threads.
 **********************************************************************************************************************/
  PP_API void SetThreads(int val);
  PP_API int GetThreads();


/**********************************************************************************************************************
 ** ShapeCompensation - Size compensation of G36/G37 shapes for maximal accuracy in rasterization. Requires "cleaner"
 **						gerber file (some cleaning/fixing will be done) and might take longer time to process in some
 **						cases. Disable to accept more "dirty" gerbers (many duplicate points and folded polygons), but
 **						you might loose 1 sub-pixel (1.8 micrometers on LLS25 lens) accuracy on those shapes, and
 **						should be unnoticeable on most LLS systems, thus it should be disabled in most cases.
 **
 ** Set:
 **		val - Bool, true=Enable size compensation for G36/G37 shapes, Default=false.
 **
 ** Is:
 **		Return: 0=false, 1=true (Size compensation for G36/G37 shapes is enabled).
 **********************************************************************************************************************/
  PP_API void SetShapeCompensation(bool val);
  PP_API int IsShapeCompensation();

/**********************************************************************************************************************
 ** GbrFixRetryCount - Can be useful if you need ShapeCompensation enabled and it fails to fix/clean gerber because of
 **					too many duplicate points and/or folded polygons. Should not be used unless it fails with 0 and
 **					you really need ShapeCompensation enabled, and if so you should start with retry count = 2, and
 **					increase if it still fails. ErrorAsWarning option must not be set.
 **
 ** Set:
 **		val - specify number of retries to clean/fix gerber for shape compensation. Default=0.
 **
 ** Get:
 **		Return: number of retries to to clean/fix gerber for shape compensation.
 **********************************************************************************************************************/
  PP_API void SetGbrFixRetryCount(int val);
  PP_API int GetGbrFixRetryCount();

/**********************************************************************************************************************
 ** ErrorAsWarning - Default=false. Ignore all errors and just print them as warning in converter log. NOT recommended
 **					to be enabled. It will "force" converting the gerber regardless if output might be erroneous.
 **
 ** Set:
 **		val - Bool, true=errors treated as warnings
 **
 ** Is:
 **		Return: 0=false, 1=true (errors treated as warnings)
 **********************************************************************************************************************/
  PP_API void SetErrorAsWarning(bool val);
  PP_API int IsErrorAsWarning();

/**********************************************************************************************************************
 ** ErrorTreshold - Number of errors in gerber to accept before abortion. NOT recommended to set higher than 0.
 **					It will "force" converting the gerber regardless if output might be erroneous.
 **
 ** Set:
 **		val - specify number of errors to accept before abortion, default=0.
 **
 ** Get:
 **		Return: number of errors to accept before abortion.
 **********************************************************************************************************************/
  PP_API void SetErrorThreshold(int val);
  PP_API int GetErrorThreshold();

/**********************************************************************************************************************
 ** ArcResolution - Arc (and circles) can not be directly rasterized, thus it must break up an arc into a series of 
 **					segments. The ArcResolution parameter determines how many degrees of arc to approximate as a 
 **					segment. Default Value is 3 degrees which would generate 121 (120 + 1) vertices per circle. A value
 **					optimized for faster processing could be e.g. 9 degrees which would generate 41 (40 + 1) vertices
 **					per circle, but at the cost of coarser arcs.
 **					Note that when you set higher ArcResolution, large circles will be more poorly approximated than small 
 **					circles. This error can be corrected by using ChordError option.
 **
 ** Set:
 **		val - specify the angle to determine the number of segments to be used when approximate the border line of a
 **			  circle or an arc, in degrees. Default=3. Speed optimized=9.
 **
 ** Get:
 **		Return: the angle used in segmentation to determin arc resolution, in degrees.
 **********************************************************************************************************************/
  PP_API void SetArcResolution(int val);
  PP_API int GetArcResolution();

/**********************************************************************************************************************
 ** ChordError - A different approach to controlling how arcs are segmented. This parameters is a distance (not an angle) 
 **				 and represents the maximum error between the ideal arc and the segment approximation. Because the error 
 **				 is constant, is uses less segments on small radius arcs than for large radius arcs, unlikne the option 
 **				 ArcResolution. Chord error will override the arc resolution control and can help cut down the 
 **				 pre-processing time without sacrificing the smoothness of large arcs. 
 **				 Suggested optimization setting is:
 **				 0.001 mm ChordError in combination with 9 deg ArcResolution
 **
 ** Set:
 **		val - specify the max allowed error between ideal arc and the segment approximation, in mm.
 **
 ** Get:
 **		Return: the distance between ideal arc and the segment approximation, in mm.
 **********************************************************************************************************************/
  PP_API void SetChordError(double val);
  PP_API double	GetChordError();


/**********************************************************************************************************************
 ** IgnoreODBLayerStepInSuffix - When using ODB++ as input the final result folder will be named as the ODB++ input
 **			and with the selected step and layer as suffix. With this option set to true, this suffix will not be added.
 **
 ** Set:
 **		val - specify if ODB layer and step should be added as suffix to result folder and stripe names
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
  PP_API void SetIgnoreODBLayerStepInSuffix(bool val);
  PP_API int IsIgnoreODBLayerStepInSuffix();
  


// --------------------------------------------------------------------------------------------------------------------
// --------------- ADVANCED PREVIEW PARAMETERS ------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


/**********************************************************************************************************************
 ** AdvPreview - Enable advanced preview to get a downscaled version of the rasterized image for preview purposes. This
 **				 is helpful to visualize the location of paneldata fields and the stripe distribution with overlap areas.
 **
 ** Set:
 **		val - true=Enable advanced preview (default), false=Disable fast mode.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetAdvPreview(bool val);
PP_API int IsAdvPreview();

/**********************************************************************************************************************
 ** PreviewFastMode - Enable fast mode to auto select scaling factors for optimized speed and to get a reasonable image
 **					  size as output that is possible to view in normal image viewer software. If this is enabled, the
 **					  parameters Post-/Pre-DrawFactor will be ignored and should be set to zero.

 **					  It is recommended to enable fast mode.
 **
 ** Set:
 **		val - true=Enable fast mode (default), false=Disable fast mode.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewFastMode(bool val);
PP_API int IsPreviewFastMode();

/**********************************************************************************************************************
 ** PreviewShowPaneldata - Enable show paneldata to enable visualization of all defined paneldata fields in the preview
 **						   image as a separate color. This is very helpful to be able to see and verify the defined
 **						   paneldata locations, field sizes and easier identify if any of the paneldata limitations are 
 **						   broken.
 **
 ** Set:
 **		val - true=Enable show paneldata (default), false=Disable show paneldata.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewShowPaneldata(bool val);
PP_API int IsPreviewShowPaneldata();

/**********************************************************************************************************************
 ** PreviewShowLines - Enable show lines to visualize the distribution of stripes including overlap, in a separate color.
 **
 ** Set:
 **		val - true=Enable show lines (default), false=Disable show lines.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewShowLines(bool val);
PP_API int IsPreviewShowLines();

/**********************************************************************************************************************
 ** PreviewVerticalFlip - Enable vertical flip to flip the image vertically before saving the preview.
 **
 ** Set:
 **		val - true=Enable flip vertically (default), false=Disable flip vertically.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewVerticalFlip(bool val);
PP_API int IsPreviewVerticalFlip();

/**********************************************************************************************************************
 ** PreviewDrawFilledRectangles - Enable draw filled rectangles to show the paneldata fields as color filled rectangles.
 **								  If disabled it will only show the lines around paneldata fields.
 **
 ** Set:
 **		val - true=Enable draw filled rectangles (default), false=Disable draw filled rectangles.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewDrawFilledRectangles(bool val);
PP_API int IsPreviewDrawFilledRectangles();

/**********************************************************************************************************************
 ** PreviewCollisionDetection - Enable collision detection to show the paneldata fields in red color if they collide
 **								with artwork design and green if no collision.
 **
 **								LIMITATION! This will only work for paneldata with rotations 0, 90, 180 and 270 degrees.
 **								It will also slow down the preview process.
 **
 ** Set:
 **		val - true=Enable collision detection, false=Disable collision detection (default).
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewCollisionDetection(bool val);
PP_API int IsPreviewCollisionDetection();

/**********************************************************************************************************************
 ** PreviewSaveAsPng - Enable save as png to save the privew image as png instead of bmp to save disk space.
 **					   
 **				       NB! This will be slower.
 **
 ** Set:
 **		val - true=Enable save as png, false=Disable save as png (default).
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
PP_API void SetPreviewSaveAsPng(bool val);
PP_API int IsPreviewSaveAsPng();

/**********************************************************************************************************************
 ** PreviewPreDrawFactor - Pre Draw Factor divide input dimensions in image by factor before drawing.
 **						   
 **						   Recommended to set to 0 and enable FastPreview.
 **
 ** Set:
 **		val - specify the factor downscale input image before draw. Default=0. 
 **
 ** Get:
 **		Return: downscale factor before draw.
 **********************************************************************************************************************/
PP_API int GetPreviewPreDrawFactor();
PP_API void SetPreviewPreDrawFactor(int val);

/**********************************************************************************************************************
 ** PreviewPostDrawFactor - Post Draw Factor divide input dimensions in image by factor after drawing.
 **
 **						   Recommended to set to 0 and enable FastPreview.
 **
 ** Set:
 **		val - specify the factor downscale input image after draw. Default=0.
 **
 ** Get:
 **		Return: downscale factor after draw.
 **********************************************************************************************************************/
PP_API int GetPreviewPostDrawFactor();
PP_API void SetPreviewPostDrawFactor(int val);

/**********************************************************************************************************************
 ** PreviewDrawLineWidth - Draw Line Width specifies the thickknes of the drew lines for paneldata and stripe
 **						   distribution visualization.
 **
 ** Set:
 **		val - specify draw line thickness. Default=3.
 **
 ** Get:
 **		Return: draw line thickness.
 **********************************************************************************************************************/
PP_API int GetPreviewDrawLineWidth();
PP_API void SetPreviewDrawLineWidth(int val);

/**********************************************************************************************************************
 ** PreviewMaxMemory - Max Memory available on the computer for image processing. Large images with small downscale
 **					   factor require more memory. Should be set to amount of RAM on the computer or a little lower.
 **					   Unit is Mega Byte.
 **
 ** Set:
 **		val - Max memory. Default=256000 (MB).
 **
 ** Get:
 **		Return: max memory.
 **********************************************************************************************************************/
PP_API int GetPreviewMaxMemory();
PP_API void SetPreviewMaxMemory(int val);



// --------------------------------------------------------------------------------------------------------------------
// --------------- OPTICAL PROXIMITY CORRECTION (OPC) PARAMETERS ------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// * See OPC chapter in "Interface specification LuxBeam LS RLT V2" document in Bundle doc folder for further 
//   illustrations of the effects of OPC parameters

/**********************************************************************************************************************
 ** OpcEnabled - Enable or Disable the Optimal Proximity Correction (OPC) feature. This feature manipulates the features
 **				 in the design to compensate for unwanted optical effects on exposure. Note that this is quite time
 **				 consuming process and require high end CUDA compliant Nvidia GPU.
 **
 ** Set:
 **		val - set to true to enable OPC
 **
 ** Get:
 **		Return: returns true if OPCS is enabled
 **********************************************************************************************************************/
/**********************************************************************************************************************
 ** EdgeBlending - Enable edge blending to get more smooth overlaps between stripes.
 **
 ** Set:
 **		val - false=Disable edge blending (default), true=Enable edge blending.
 **
 ** Is:
 **		Return: 0=false, 1=true
 **********************************************************************************************************************/
  PP_API void SetEdgeBlending(bool val);
  PP_API int IsEdgeBlending();

  
  PP_API void SetOpcEnabled(bool val);
  PP_API int IsOpcEnabled();

/**********************************************************************************************************************
 ** OpcCuringTreshold - a parameter used to describe how hard or how easy a photo resist can be cured by UV light. The 
 **						value can be any integer between 0 to 255, with 0 meaning no exposure and 255 meaning full 
 **						exposure. Over-exposure may happen when stray light, for example with exposure power of 100, 
 **						presents at an unexposed area. In this case, a photo resist with curing threshold < 100 would be 
 **						cured unexpectedly, causing over-exposure.
 **
 **						When over-exposure is encountered during real photo lithography process, use a lower value for 
 **						curing_threshold in OPC setting. By doing this, we are telling the OPC process that the photo 
 **						resist being used has a lower curing threshold than the current setting. Therefore more 
 **						correction will be added to the over-exposed area.
 **
 ** Set:
 **		val - specify the curing threshold value. Typically in the range from 50 to 128. Default 80.
 **
 ** Get:
 **		Return: the curing threshold value.
 **********************************************************************************************************************/
  PP_API void SetOpcCuringThreshold(int val);
  PP_API int GetOpcCuringThreshold();

/**********************************************************************************************************************
 ** OpcRangeLow - In reality, there is no perfect photo resist that has a strict curing threshold. Therefore two
 **				  parameters, range_low and range_high, are introduced to confine a reasonable range within which the
 **				  curing threshold can be adjusted. These two parameters also allow for separate mapping for under- and
 **				  over- exposed areas.
 **
 **				  For under-exposed area, only pixels with exposure level below 'curing threshold ?range_low' will be 
 **				  corrected. Lower range_low value leads to stronger correction on under-exposed area. Vice versa. 
 **				  Default value for range_low is 25. Since over-exposure is a more common problem than under-exposure, 
 **			      range_low can then be kept default for most cases.
 **
 ** Set:
 **		val - specify the range low value.
 **
 ** Get:
 **		Return: the range low value.
 **********************************************************************************************************************/
  PP_API void SetOpcRangeLow(int val);
  PP_API int GetOpcRangeLow();

/**********************************************************************************************************************
 ** OpcRangeHigh - In reality, there is no perfect photo resist that has a strict curing threshold. Therefore two 
 **				   parameters, range_low and range_high, are introduced to confine a reasonable range within which the 
 **				   curing threshold can be adjusted. These two parameters also allow for separate mapping for under- and 
 **				   over- exposed areas.
 **				   
 **				   For over-exposed area, only pixels with exposure level over 'curing threshold + range_high' will be
 **				   corrected. Lower range_high value leads to stronger correction for over-exposed area. Vice versa.
 **				   Default value for range_high should be 25, but is here set to 15 to clearly show the effects of it. 
 **				   Since over-exposure is a more common problem than under-exposure, range_high is normally the value to 
 **				   adjust. 
 **
 ** Set:
 **		val - specify the range high value.
 **
 ** Get:
 **		Return: the range high value.
 **********************************************************************************************************************/
  PP_API void SetOpcRangeHigh(int val);
  PP_API int GetOpcRangeHigh();

/**********************************************************************************************************************
 ** OpcSigma - A Gaussian blur is performed during OPC process, and sigma is the standard deviation in the Gaussian 
 **			   distribution equation. Larger value means stronger blur and smaller value gives sharper image.
 **			   Sigma turns to have more effect on the corners than on line width.
 **			   Default value for sigma is 2.
 **
 ** Set:
 **		val - specify Gaussian sigma value for OPC
 **
 ** Get:
 **		Return: sigma value.
 **********************************************************************************************************************/
  PP_API void SetOpcGausSigma(int val);
  PP_API int GetOpcGausSigma();

#endif