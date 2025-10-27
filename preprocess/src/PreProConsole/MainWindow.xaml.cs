/**********************************************************************************************************************
 ** PreProConsole - LLS RLT system by VISITECH AS
 ** 
 ** The purpose of this GUI application is to provide a simple example of how the PreProcessApi.dll can be utilized
 ** from a C# user interface application, and is not made with production environment requirements in mind.
 ** 
 ** All source code from LuriaConsole are free to reuse and modify for VISITECH LLS customers, at their own risk. Note
 ** that we will not prioritize improvements/bugs in PreProConsole, as it is expected that our LLS RLT customers make
 ** their own user interface.
 **********************************************************************************************************************/

using MahApps.Metro.Controls;
using Parago.Windows;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Threading;
using System.Collections.Generic;
using System.IO.Packaging;
using System.Linq.Expressions;
using System.Xml.Linq;

namespace PreProConsole
{
    public partial class MainWindow : MetroWindow
    {
        private const string PRE_PRO_DLL = "PreProcessApi.dll";

        #region PreProcessApi DLL interface functions
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int InitGlobalLogger();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int SetDllPath(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int Init();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int InitGerber(string inputFile, string outputDir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPPVersion(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPPModuleVersion(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetProductName(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int SetInputFile(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int SetOutputDir(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int StartProcessing();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int StartProcessingAsync();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void AbortProcessing();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetProcessResult();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetProcessState();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetProcessProgress();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int PreAnalyze();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int SaveParameters();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int LoadParameters();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetImageWidth();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetImageHeight();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetInverse(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsInverse();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetInverseFrame(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsInverseFrame();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetRotationCCW(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetRotationCCW();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetMirrorH(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsMirrorH();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetMirrorV(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsMirrorV();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetErosionX(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetErosionX();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetErosionY(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetErosionY();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOffsetX(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetOffsetX();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOffsetY(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetOffsetY();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetImageFrame(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetImageFrame();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetResultPath(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetGlobalFiducial(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetLocalFiducial(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetGlobalFiducial();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetLocalFiducial();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetErrMsg(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetInputFile(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOutputDir(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetSuffix(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int AddStaticPanelData(string logoGerber, int dcode, int rotation, bool inverse, bool flipH, bool flipV, double scaleX, double scaleY);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int AddDynamicPanelData(int type, int content, int dcode, int rotation, double sizeX, double sizeY, bool inverse);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int AddDynamicPanelDataId(int type, int content, int id, int rotation, double sizeX, double sizeY, bool inverse);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int AddDynamicPanelDataPos(int id, double x_pos, double y_pos);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsMakePreview();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetMakePreview(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPhotoheadPitch();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPhotoheadPitch(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPhotoheadNumber();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPhotoheadNumber(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetProductID();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetProductID(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetSuffix(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPhotoheadRotated();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPhotoheadRotated(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetFixedWindow(double lLx, double lLy, double uRx, double uRy);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetWindowLLX();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetWindowLLY();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetWindowURX();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetWindowURY();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetWorkDir(string workDir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetWorkDir(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetNominalPhotoheadDist(int ph_num);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetNominalPhotoheadDist(double val, int ph_num);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetTableLimitX();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetTableLimitX(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int UpdateFiducials(string gerber, string job_folder, int global_fid, int local_fid);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetNegativeYDirection(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsNegativeYDirection();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcEnabled(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsOpcEnabled();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcCuringThreshold(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcCuringThreshold();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcRangeLow(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcRangeLow();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcRangeHigh(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcRangeHigh();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcGausSigma(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcGausSigma();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcGausKernelRadius(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcGausKernelRadius();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOpcImageDeltaDim(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOpcImageDeltaDim();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetArcResolution();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetArcResolution(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetEdgeBlending(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsEdgeBlending();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetMTC(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsMTC();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetAdvPreview(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsAdvPreview();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewFastMode(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewFastMode();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewShowPaneldata(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewShowPaneldata();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewShowLines(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewShowLines();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewVerticalFlip(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewVerticalFlip();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewCollisionDetection(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewCollisionDetection();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewDrawFilledRectangles(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewDrawFilledRectangles();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewSaveAsPng(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsPreviewSaveAsPng();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPreviewPreDrawFactor();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewPreDrawFactor(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPreviewPostDrawFactor();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewPostDrawFactor(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPreviewDrawLineWidth();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewDrawLineWidth(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetPreviewMaxMemory();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetPreviewMaxMemory(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetWhiteLines();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetWhiteLines(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetSmoothing(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsSmoothing();
        #endregion

        #region GDSII support functions
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int InitGDS(string inputFile, string outputDir, string workDir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GDSConvert(int nestingLevel = 0);
        #endregion

        #region PreProFast functions
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int InitJob(string inputFile, string outputDir, string workDir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOdbLayer(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOdbLayer(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetOdbStep(string val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetOdbStep(StringBuilder rntStr, int len);
        #endregion

        #region Advanced functions
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetThreads(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetThreads();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetSpx(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetSpx();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetErrorThreshold();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetErrorThreshold(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsErrorAsWarning();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetErrorAsWarning(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsShapeCompensation();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetShapeCompensation(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetGbrFixRetryCount();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetGbrFixRetryCount(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetChordError(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetChordError();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetIgnoreODBLayerStepInSuffix(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsIgnoreODBLayerStepInSuffix();
        #endregion

        #region Debug functions (Only available in developer mode, not part of interface documentation)
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsDevMode();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetBlackTop(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetBlackBottom(double val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetBlackBottom();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern double GetBlackTop();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsBlackRestTop();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetBlackRestTop(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsBlackRestBottom();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetBlackRestBottom(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetRam(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetRam();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsMakeBin();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetMakeBin(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsRegDebug();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetRegDebug(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsKeepTiff();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetKeepTiff(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsSaveBitmap();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetSaveBitmap(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsKeepTmp();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetKeepTmp(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsSilent();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetSilent(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int IsBmpFrameDisabled();
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetBmpFrameDisabled(bool val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern void SetImageFrameOverride(int val);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)] private static extern int GetImageFrameOverride();
        #endregion

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)] static extern bool SetDllDirectory(string lpPathName);

        NumberFormatInfo nfi = new CultureInfo("en-US", false).NumberFormat;

        private readonly BackgroundWorker processing_worker = new BackgroundWorker();
        private readonly BackgroundWorker analyzing_worker = new BackgroundWorker();
        private readonly BackgroundWorker async_worker = new BackgroundWorker();
        private readonly BackgroundWorker updateFids_worker = new BackgroundWorker();
        private string _status;
        private readonly StringBuilder _strBuffer;
        private const int BufferLen = 1024;
        private bool dll_failed = false;
        private bool odb_selected = false;
        private bool gds_selected = false;
        private bool fast_mode = false;

        private int[] ProductIDMap;

        public ObservableCollection<string> LogoListStr { get; set; }
        public ObservableCollection<Logo> LogoList { get; set; }
        public ObservableCollection<string> DynamicPDListStr { get; set; }
        public ObservableCollection<DynamicPD> DynamicPDList { get; set; }
        public ObservableCollection<string> DPDTypeList { get; set; }
        public ObservableCollection<string> DPDContentList { get; set; }
        public ObservableCollection<int> NumberOfPhotoheadsList { get; set; }
        public ObservableCollection<int> PhotoheadPitchList { get; set; }
        public ObservableCollection<string> ImageFrameList { get; set; }
        public ObservableCollection<string> ProductIDList { get; set; }

        public int Progress
        {
            get { return (int)GetValue(ProgressProperty); }
            set { SetValue(ProgressProperty, value); }
        }
        public static DependencyProperty ProgressProperty = DependencyProperty.Register("Progress", typeof(int), typeof(MainWindow));

        public MainWindow()
        {
            Progress = 0;
            nfi.NumberGroupSeparator = "";
            _strBuffer = new StringBuilder(BufferLen);
            LogoListStr = new ObservableCollection<string>();
            LogoList = new ObservableCollection<Logo>();
            DynamicPDListStr = new ObservableCollection<string>();
            DynamicPDList = new ObservableCollection<DynamicPD>();
            DPDTypeList = new ObservableCollection<string>(new[] { "0 - Text", "1 - BMP-file", "2 - ECC200", "3 - ECC200-8x32" });
            DPDContentList = new ObservableCollection<string>(new[] { "0 - BoardSerialNumber", "1 - ScaleInfo", "2 - GeneralText", "3 - DateTime", "4 - ScaleInfo-X", "5 - ScaleInfo-Y" });
            NumberOfPhotoheadsList = new ObservableCollection<int>(new[] { 1, 2, 3, 4, 5, 6, 7, 8 });
            PhotoheadPitchList = new ObservableCollection<int>(Enumerable.Range(1, 50).ToList());
            ImageFrameList = new ObservableCollection<string>(new[] { "0 - ultra-low", "1 - min", "2 - low", "3 - medium", "4 - high", "5 - max" });
            ProductIDList = new ObservableCollection<string>(new[] { "LLS50", "LLS30", "LLS25", "LLS15", "LLS10", "LLS06", "LLS04", "LLS2500" });
            ProductIDMap = new[] { 200001, 235002, 100001, 117502, 50001, 50002, 25002, 16702 };


            // Subscribe to events
            processing_worker.DoWork += processing_worker_DoWork;
            processing_worker.RunWorkerCompleted += processing_worker_RunWorkerCompleted;

            async_worker.DoWork += async_worker_DoWork;
            async_worker.RunWorkerCompleted += async_worker_RunWorkerCompleted;

            analyzing_worker.DoWork += analyzing_worker_DoWork;
            analyzing_worker.RunWorkerCompleted += analyzing_worker_RunWorkerCompleted;

            RltReset();
            InitializeComponent();
        }

        private void processing_worker_DoWork(object sender, DoWorkEventArgs e)
        {
            _status = GetResultMsg(StartProcessing());
        }

        private void async_worker_DoWork(object sender, DoWorkEventArgs e)
        {
            _status = GetResultMsg(StartProcessingAsync());

            while (true)
            {
                var status = GetProcessResult();
                var progress = GetProcessProgress();

                Action action = delegate
                {
                    Status.Text = GetStateMsg(GetProcessState());
                    Progress = progress;
                };
                System.Windows.Application.Current.Dispatcher.Invoke(DispatcherPriority.Normal, action);

                if (GetResultMsg(status) != "ALREADY_RUNNING")
                {
                    _status = GetResultMsg(status);
                    break;
                }
                Thread.Sleep(500);
            }
        }

        private void analyzing_worker_DoWork(object sender, DoWorkEventArgs e)
        {
            _status = GetResultMsg(PreAnalyze());
        }

        private void processing_worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            OpenButton.IsEnabled = true;
            ClearButton.IsEnabled = true;
            PreAnalyzeButton.IsEnabled = true;
            PhotoheadPitchCB.IsEnabled = true;
            NumberOfPhotoheadsCB.IsEnabled = true;
            AsyncButton.IsEnabled = true;
            UpdateFidsButton.IsEnabled = true;
            Progress = 100;

            Status.Text = _status;
            if (GetResultPath(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetResultPath exception", "Result Path string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            string s = "PreProcessing completed successfully.\nResult location: " + _strBuffer;
            var img = MessageBoxImage.Information;
            if (_status != "SUCCESS")
            {
                img = MessageBoxImage.Error;
                if (GetErrMsg(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                s = "PreProcessing failed!\n\nStatus: " + _status + "\n\n" + _strBuffer;
            }
            MessageBox.Show(Application.Current.MainWindow, s, "Preprocess result: " + _status, MessageBoxButton.OK, img);
        }

        private void async_worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            OpenButton.IsEnabled = true;
            ClearButton.IsEnabled = true;
            PreAnalyzeButton.IsEnabled = true;
            PhotoheadPitchCB.IsEnabled = true;
            NumberOfPhotoheadsCB.IsEnabled = true;
            AsyncButton.IsEnabled = true;
            UpdateFidsButton.IsEnabled = true;

            Status.Text = _status;
            if (GetResultPath(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetResultPath exception", "Result Path string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            string s = "PreProcessing completed successfully.\nResult location: " + _strBuffer;
            var img = MessageBoxImage.Information;

            if (_status == "ABORTED")
            {
                img = MessageBoxImage.Warning;
                if (GetErrMsg(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                s = "PreProcessing aborted!\n\nStatus: " + _status + "\n\n" + _strBuffer;
            }
            else if (_status != "SUCCESS")
            {
                img = MessageBoxImage.Error;
                if (GetErrMsg(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                s = "PreProcessing failed!\n\nStatus: " + _status + "\n\n" + _strBuffer;
            }
            else
            {
                
                StringBuilder outLocate = new StringBuilder(BufferLen);

                if (GetResultPath(outLocate, BufferLen) != 0)
                    return;
                

                if(FiducialValidate(outLocate.ToString()) == false)
                {
                    //확인바람.
                }

            }
            MessageBox.Show(Application.Current.MainWindow, s, "Preprocess result: " + _status, MessageBoxButton.OK, img);
        }

        bool FiducialValidate(string path)
        {

            string fPath = $@"{path}\rlt_settings.xml";
            var doc = XDocument.Load(fPath);

            var localFids = doc.Descendants("Local")
                .Where(e => e.Attribute("d-code")?.Value != "0")
                .Elements("fid")
                .Select(fid => new
                {
                    GbrX = double.Parse( fid.Element("gbr")?.Attribute("x")?.Value),
                    GbrY = double.Parse(fid.Element("gbr")?.Attribute("y")?.Value)
                }).ToList();

            var globalFids = doc.Descendants("Global")
               .Where(e => e.Attribute("d-code")?.Value != "0")
               .Elements("fid")
               .Select(fid => new
               {
                   GbrX =  double.Parse(fid.Element("gbr")?.Attribute("x")?.Value),
                   GbrY = double.Parse(fid.Element("gbr")?.Attribute("y")?.Value)
               }).ToList();

            var mapping = new Dictionary<double, List<double>>();

            globalFids.ForEach(v =>
             {
                 
                 double key = Math.Floor(v.GbrX * 100.0) / 100.0;
                 double val = Math.Floor(v.GbrY * 100.0) / 100.0;

                 if (!mapping.TryGetValue(key, out var list))
                 {
                     list = new List<double>();
                     mapping[key] = list;
                 }
                 list.Add(val);
             });

            int col = mapping.Count;
            int row = 0;

            foreach (var kv in mapping)
            {
                if (kv.Value.Count > row)
                    row = kv.Value.Count;
            }

            if (row * col != globalFids.Count)
            {
                MessageBox.Show(@"global fiducial validate error, check fiducial position");
            }
            mapping.Clear();

            localFids.ForEach(v =>
            {

                double key = Math.Floor(v.GbrX * 100.0) / 100.0;
                double val = Math.Floor(v.GbrY * 100.0) / 100.0;

                if (!mapping.TryGetValue(key, out var list))
                {
                    list = new List<double>();
                    mapping[key] = list;
                }
                list.Add(val);
            });

            col = mapping.Count;
            row = 0;

            foreach (var kv in mapping)
            {
                if (kv.Value.Count > row)
                    row = kv.Value.Count;
            }

            if (row * col != localFids.Count)
            {
                MessageBox.Show(@"local fiducial validate error, check fiducial position");
            }

            return true;
        }



        private void analyzing_worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            OpenButton.IsEnabled = true;
            ClearButton.IsEnabled = true;
            PreAnalyzeButton.IsEnabled = true;
            PhotoheadPitchCB.IsEnabled = true;
            NumberOfPhotoheadsCB.IsEnabled = true;
            AsyncButton.IsEnabled = true;
            UpdateFidsButton.IsEnabled = true;

            Status.Text = _status;
            if (GetResultPath(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetResultPath exception", "Result Path string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            string s = "PreAnalyzing completed successfully.\nWidth: " + string.Format(nfi, "{0:N2}", GetImageWidth()) + " mm\nHeight: " + string.Format(nfi, "{0:N2}", GetImageHeight()) + " mm";
            var img = MessageBoxImage.Information;
            if (_status == "FAILED")
            {
                img = MessageBoxImage.Error;
                if (GetErrMsg(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                s = "PreAnalyzing failed!\n\nStatus: " + _status + "\n\n" + _strBuffer;
            }
            MessageBox.Show(Application.Current.MainWindow, s, "PreAnalyze: " + _status, MessageBoxButton.OK, img);
        }

        private void RltReset()
        {
            try
            {
                if (dll_failed)
                    ChangePreProAPIPath();

                if (GetProductName(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "GetProductName exception", "Version string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                string product_name = _strBuffer.ToString();
                if (product_name == "PreProFast")
                    fast_mode = true;
                if (GetPPVersion(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "GetPPVersion exception", "Version string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                string bundle_version = _strBuffer.ToString();
                if (GetPPModuleVersion(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "GetPPModuleVersion exception", "Version string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                string module_version = _strBuffer.ToString();

                Title = "LLS2 Bundle v" + bundle_version + " (PreProcessConsole v" + Assembly.GetExecutingAssembly().GetName().Version + ", PreProcessAPI v" + module_version + ")";

                InitializeComponent();
                InitGlobalLogger();
                Init();
                UpdateParameters();
                DataContext = this;
                dll_failed = false;

                if (fast_mode)
                {
                    Title = "(FAST-MODE) LLS2 Bundle v" + bundle_version + " (PreProcessConsole v" + Assembly.GetExecutingAssembly().GetName().Version + ", PreProcessAPI v" + module_version + ")";
                    ProductNameTextBox.Text = "PreProFast Console";
                }
            }
            catch (DllNotFoundException)
            {
                var res = MessageBox.Show("ERROR: Unnable to call PreProcessAPI function.\nSpecify correct DLL path.",
                                        "Invalid DLL path", MessageBoxButton.OKCancel, MessageBoxImage.Error);
                if (res == MessageBoxResult.OK)
                {
                    dll_failed = true;
                    RltReset();
                }
            }
            catch (Exception e)
            {
                MessageBox.Show("ERROR: Failed to initiate:\n" + e.Message,
                                        "Init failed", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void UpdateParameters()
        {
            if (IsDevMode() == 0)
                DebugTab.Visibility = Visibility.Collapsed;
            else
                DebugTab.Visibility = Visibility.Visible;

            if (GetInputFile(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetInputFile exception", "Input File string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            GerberInput.Text = _strBuffer.ToString();

            if (GetOutputDir(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetOutputDir exception", "Output Dir string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            OutputDir.Text = _strBuffer.ToString();

            Spx.Text = GetSpx().ToString();
            Rotation.Text = GetRotationCCW().ToString();
            ErosionX.Text = string.Format(nfi, "{0:N4}", GetErosionX());
            ErosionY.Text = string.Format(nfi, "{0:N4}", GetErosionY());
            InverseCheckBox.IsChecked = IsInverse() != 0;
            InverseFrameCheckBox.IsChecked = IsInverseFrame() != 0;
            FlipHCheckBox.IsChecked = IsMirrorH() != 0;
            FlipVCheckBox.IsChecked = IsMirrorV() != 0;
            Ram.Text = GetRam().ToString();
            Threads.Text = GetThreads().ToString();
            BlackBottom.Text = string.Format(nfi, "{0:N2}", GetBlackBottom());
            BlackTop.Text = string.Format(nfi, "{0:N2}", GetBlackTop());
            BlackRestTop.IsChecked = IsBlackRestTop() != 0;
            BlackRestBottom.IsChecked = IsBlackRestBottom() != 0;
            GlobalFiducial.Text = GetGlobalFiducial().ToString();
            LocalFiducial.Text = GetLocalFiducial().ToString();

            MakePreviewCheckBox.IsChecked = IsMakePreview() != 0;
            NegativeYDirectionCheckBox.IsChecked = IsNegativeYDirection() != 0;
            EdgeBlendingCheckBox.IsChecked = IsEdgeBlending() != 0;
            MTCCheckBox.IsChecked = IsMTC() != 0;
            NumberOfPhotoheadsCB.SelectedValue = GetPhotoheadNumber();
            PhotoheadPitchCB.SelectedValue = GetPhotoheadPitch();
            ProductIDComboBox.SelectedIndex = Array.FindIndex(ProductIDMap, id => id == GetProductID());
            MakeBinCheckBox.IsChecked = IsMakeBin() != 0;
            PhotoheadRotatedCheckBox.IsChecked = IsPhotoheadRotated() != 0;
            RegDebugCheckBox.IsChecked = IsRegDebug() != 0;
            KeepTiffCheckBox.IsChecked = IsKeepTiff() != 0;
            SaveBitmapCheckBox.IsChecked = IsSaveBitmap() != 0;
            KeepTmpCheckBox.IsChecked = IsKeepTmp() != 0;
            SilentCheckBox.IsChecked = IsSilent() != 0;
            ErrorAsWarningCheckBox.IsChecked = IsErrorAsWarning() != 0;
            IgnoreODBLayerStepInSuffixCheckBox.IsChecked = IsIgnoreODBLayerStepInSuffix() != 0;
            ErrorThresholdEdit.Text = GetErrorThreshold().ToString();
            if (GetWorkDir(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetWorkDir exception", "Work Dir string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            WorkDir.Text = _strBuffer.ToString();
            ChordErrorEdit.Text = string.Format(nfi, "{0:N4}", GetChordError());
            DisableBmpFrameCheckBox.IsChecked = IsBmpFrameDisabled() != 0;
            TableLimitXEdit.Text = string.Format(nfi, "{0:N2}", GetTableLimitX());
            ShapeCompensationCheckBox.IsChecked = IsShapeCompensation() != 0;
            GbrFixRetryCountEdit.Text = GetGbrFixRetryCount().ToString();
            ArcResolutionEdit.Text = GetArcResolution().ToString();
            ImageFrameOverrideEdit.Text = GetImageFrameOverride().ToString();
            WhiteLinesEdit.Text = GetWhiteLines().ToString();
            SmoothingCheckBox.IsChecked = IsSmoothing() != 0;

            ImageFrameComboBox.SelectedIndex = GetImageFrame();

            AutoWindowRadioButton.IsChecked = true;
            UpdateGerberWindowSelection();

            //Preview
            EnableAdvPreviewCheckBox.IsChecked = IsAdvPreview() != 0;
            FastModeCheckBox.IsChecked = IsPreviewFastMode() != 0;
            ShowPaneldataCheckBox.IsChecked = IsPreviewShowPaneldata() != 0;
            ShowLinesCheckBox.IsChecked = IsPreviewShowLines() != 0;
            VerticalFlipCheckBox.IsChecked = IsPreviewVerticalFlip() != 0;
            CollisionDetectionCheckBox.IsChecked = IsPreviewCollisionDetection() != 0;
            DrawFilledRectanglesCheckBox.IsChecked = IsPreviewDrawFilledRectangles() != 0;
            SaveAsPngCheckBox.IsChecked = IsPreviewSaveAsPng() != 0;
            PreDrawFactorEdit.Text = GetPreviewPreDrawFactor().ToString();
            PostDrawFactorEdit.Text = GetPreviewPostDrawFactor().ToString();
            DrawLineWidthEdit.Text = GetPreviewDrawLineWidth().ToString();
            MaxMemoryEdit.Text = GetPreviewMaxMemory().ToString();

            // Static panel data
            LogoListStr.Clear();
            LogoList.Clear();
            RotationSPDEdit.Text = "0";
            LogoDcodeEdit.Text = String.Empty;
            ScaleXSPDEdit.Text = string.Format(nfi, "{0:N2}", 1.0);
            ScaleYSPDEdit.Text = string.Format(nfi, "{0:N2}", 1.0); ;
            InverseSPDCheckBox.IsChecked = false;
            FlipXSPDCheckBox.IsChecked = false;
            FlipYSPDCheckBox.IsChecked = false;

            // Dynamic panel data
            DynamicPDListStr.Clear();
            DynamicPDList.Clear();
            DcodeDPDEdit.Text = String.Empty;
            RotationDPDEdit.Text = "0";
            SizeXDPDEdit.Text = string.Format(nfi, "{0:N2}", 200.0); ;
            SizeYDPDEdit.Text = string.Format(nfi, "{0:N2}", 100.0); ;

            if (GetSuffix(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetSuffix exception", "Suffix string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            SuffixEdit.Text = _strBuffer.ToString();

            UpdateNomPhDist(GetPhotoheadNumber());

            if (GdsNestingLevelEdit.Text == String.Empty)
                GdsNestingLevelEdit.Text = "0";

            // OPC params
            EnableOPCCheckBox.IsChecked = IsOpcEnabled() != 0;
            CuringThreshold.Text = GetOpcCuringThreshold().ToString();
            RangeHigh.Text = GetOpcRangeHigh().ToString();
            RangeLow.Text = GetOpcRangeLow().ToString();
            GausKernelRadius.Text = GetOpcGausKernelRadius().ToString();
            GausSigma.Text = GetOpcGausSigma().ToString();
            ImageDeltaDim.Text = GetOpcImageDeltaDim().ToString();

            if(fast_mode)
            {
                if (GetOdbStep(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "GetOdbStep exception", "OdbStep string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                OdbStepEdit.Text = _strBuffer.ToString();

                if (GetOdbLayer(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "GetOdbLayer exception", "OdbLayer string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                OdbLayerEdit.Text = _strBuffer.ToString();

                odb_selected = false;
                gds_selected = false;
                GdsButton.IsEnabled = false;
                GerberButton.IsEnabled = false;
                OdbButton.IsEnabled = false;
                PreConvertButton.IsEnabled = false;
                OdbGrid.IsEnabled = true;
                GerberButton.Visibility = Visibility.Collapsed;
                GdsButton.Visibility = Visibility.Collapsed;
                OdbButton.Visibility = Visibility.Collapsed;
                PreAnalyzeButton.Visibility = Visibility.Collapsed;
                PreConvertButton.Visibility = Visibility.Collapsed;
                PreviewTab.Visibility = Visibility.Collapsed;
                StaticPaneldataTab.Visibility = Visibility.Collapsed;
                OPCTab.Visibility = Visibility.Collapsed;
                UpdateFidsButton.Visibility = Visibility.Collapsed;
                OdbGrid.Visibility = Visibility.Visible;


                FlipHCheckBox.IsEnabled = false;
                FlipHCheckBox.Visibility = Visibility.Collapsed;
                FlipVCheckBox.IsEnabled = false;
                FlipVCheckBox.Visibility = Visibility.Collapsed;
                MakePreviewCheckBox.IsEnabled = false;
                MakePreviewCheckBox.Visibility = Visibility.Collapsed;
                OffsetX.IsEnabled = false;
                OffX.Visibility = Visibility.Collapsed;
                OffsetY.IsEnabled = false;
                OffY.Visibility = Visibility.Collapsed;
                GlobalFiducial.IsEnabled = false;
                GlobalFiducialSP.Visibility = Visibility.Collapsed;
                LocalFiducial.IsEnabled = false;
                LocalFiducialSP.Visibility = Visibility.Collapsed;
                Rotation.IsEnabled = false;
                RotationSP.Visibility = Visibility.Collapsed;
                AddDynamicPDDcodeButton.IsEnabled = false;
                AddDynamicPDDcodeButton.Visibility = Visibility.Collapsed;
            }
            else
            {
                OdbGrid.IsEnabled = false;
                OdbGrid.Visibility = Visibility.Collapsed;
                BufferSizeSP.IsEnabled = false;
                BufferSizeSP.Visibility = Visibility.Collapsed;
            }

            Status.Text = "READY";
        }

        private bool RltSetValues()
        {
            try
            {
                double val_d;

                if (fast_mode)
                {
                    InitJob(GerberInput.Text, OutputDir.Text, WorkDir.Text);
                    
                    SetOdbStep(OdbStepEdit.Text);
                    SetOdbLayer(OdbLayerEdit.Text);
                }
                else if (odb_selected == false && gds_selected == false)
                {
                    InitGerber(GerberInput.Text, OutputDir.Text);
                    SetWorkDir(WorkDir.Text);
                }

                SetRotationCCW(Int32.Parse(Rotation.Text));
                if (Double.TryParse(ErosionX.Text, NumberStyles.Float, nfi, out val_d))
                    SetErosionX(val_d);
                else
                {
                    MessageBox.Show("ErosionX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
                if (Double.TryParse(ErosionY.Text, NumberStyles.Float, nfi, out val_d))
                    SetErosionY(val_d);
                else
                {
                    MessageBox.Show("ErosionY input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
                SetInverse(InverseCheckBox.IsChecked != null && (Boolean)InverseCheckBox.IsChecked);
                SetInverseFrame(InverseFrameCheckBox.IsChecked != null && (Boolean)InverseFrameCheckBox.IsChecked);
                SetMirrorH(FlipHCheckBox.IsChecked != null && (Boolean)FlipHCheckBox.IsChecked);
                SetMirrorV(FlipVCheckBox.IsChecked != null && (Boolean)FlipVCheckBox.IsChecked);
                SetGlobalFiducial(Int32.Parse(GlobalFiducial.Text));
                SetLocalFiducial(Int32.Parse(LocalFiducial.Text));
                SetMakePreview(MakePreviewCheckBox.IsChecked != null && (Boolean)MakePreviewCheckBox.IsChecked);
                SetNegativeYDirection(NegativeYDirectionCheckBox.IsChecked != null && (Boolean)NegativeYDirectionCheckBox.IsChecked);
                SetEdgeBlending(EdgeBlendingCheckBox.IsChecked != null && (Boolean)EdgeBlendingCheckBox.IsChecked);
                SetMTC(MTCCheckBox.IsChecked != null && (Boolean)MTCCheckBox.IsChecked);
                SetPhotoheadRotated(PhotoheadRotatedCheckBox.IsChecked != null && (Boolean)PhotoheadRotatedCheckBox.IsChecked);
                SetSuffix(SuffixEdit.Text);
                if (Double.TryParse(TableLimitXEdit.Text, NumberStyles.Float, nfi, out val_d))
                    SetTableLimitX(val_d);
                else
                {
                    MessageBox.Show("TableLimitX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
                if (!SetGerberWindowSelection())
                    return false;
                SetPhotoheadNumber((int)NumberOfPhotoheadsCB.SelectedValue);
                SetPhotoheadPitch((int)PhotoheadPitchCB.SelectedValue);
                SetProductID(ProductIDMap[ProductIDComboBox.SelectedIndex]);
                if (!SaveNomPhDist())
                    return false;
                SetImageFrame(ImageFrameComboBox.SelectedIndex);
                SetWhiteLines(Int32.Parse(WhiteLinesEdit.Text));
                SetSmoothing(SmoothingCheckBox.IsChecked != null && (Boolean)SmoothingCheckBox.IsChecked);

                //Preview
                SetAdvPreview(EnableAdvPreviewCheckBox.IsChecked != null && (Boolean)EnableAdvPreviewCheckBox.IsChecked);
                SetPreviewFastMode(FastModeCheckBox.IsChecked != null && (Boolean)FastModeCheckBox.IsChecked);
                SetPreviewShowPaneldata(ShowPaneldataCheckBox.IsChecked != null && (Boolean)ShowPaneldataCheckBox.IsChecked);
                SetPreviewShowLines(ShowLinesCheckBox.IsChecked != null && (Boolean)ShowLinesCheckBox.IsChecked);
                SetPreviewVerticalFlip(VerticalFlipCheckBox.IsChecked != null && (Boolean)VerticalFlipCheckBox.IsChecked);
                SetPreviewCollisionDetection(CollisionDetectionCheckBox.IsChecked != null && (Boolean)CollisionDetectionCheckBox.IsChecked);
                SetPreviewDrawFilledRectangles(DrawFilledRectanglesCheckBox.IsChecked != null && (Boolean)DrawFilledRectanglesCheckBox.IsChecked);
                SetPreviewSaveAsPng(SaveAsPngCheckBox.IsChecked != null && (Boolean)SaveAsPngCheckBox.IsChecked);

                SetPreviewPreDrawFactor(Int32.Parse(PreDrawFactorEdit.Text));
                SetPreviewPostDrawFactor(Int32.Parse(PostDrawFactorEdit.Text));
                SetPreviewDrawLineWidth(Int32.Parse(DrawLineWidthEdit.Text));
                SetPreviewMaxMemory(Int32.Parse(MaxMemoryEdit.Text));


                // Static Panel Data
                foreach (var l in LogoList)
                {
                    if (AddStaticPanelData(l.GerberFile, l.Dcode, l.Rotation, l.Inverse, l.FlipH, l.FlipV, l.ScaleX, l.ScaleY) != 0)
                    {
                        var img = MessageBoxImage.Error;
                        if (GetErrMsg(_strBuffer, BufferLen) != 0)
                            MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                        string s = "Failed to add panel data!\n\n" + _strBuffer;
                        MessageBox.Show(Application.Current.MainWindow, s, "PanelData", MessageBoxButton.OK, img);
                        return false;
                    }
                }

                // Dynamic Panel Data
                foreach (var d in DynamicPDList)
                {
                    if (!d.IsCoords)
                    {
                        if (AddDynamicPanelData(d.Type, d.Content, d.Dcode, d.Rotation, d.SizeX, d.SizeY, d.Inverse) != 0)
                        {
                            var img = MessageBoxImage.Error;
                            if (GetErrMsg(_strBuffer, BufferLen) != 0)
                                MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                            string s = "Failed to add panel data!\n\n" + _strBuffer;
                            MessageBox.Show(Application.Current.MainWindow, s, "PanelData", MessageBoxButton.OK, img);
                            return false;
                        }
                    }
                    else if (d.coords.Count > 0)
                    {
                        if (AddDynamicPanelDataId(d.Type, d.Content, d.Dcode, d.Rotation, d.SizeX, d.SizeY, d.Inverse) != 0)
                        {
                            var img = MessageBoxImage.Error;
                            if (GetErrMsg(_strBuffer, BufferLen) != 0)
                                MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                            string s = "Failed to add panel data!\n\n" + _strBuffer;
                            MessageBox.Show(Application.Current.MainWindow, s, "PanelData", MessageBoxButton.OK, img);
                            return false;
                        }
                        foreach (var c in d.coords)
                        {
                            AddDynamicPanelDataPos(d.Dcode, c[0], c[1]);
                        }
                    }
                }


                // OPC params
                SetOpcEnabled(EnableOPCCheckBox.IsChecked != null && (Boolean)EnableOPCCheckBox.IsChecked);
                SetOpcCuringThreshold(Int32.Parse(CuringThreshold.Text));
                SetOpcRangeHigh(Int32.Parse(RangeHigh.Text));
                SetOpcRangeLow(Int32.Parse(RangeLow.Text));
                SetOpcGausKernelRadius(Int32.Parse(GausKernelRadius.Text));
                SetOpcGausSigma(Int32.Parse(GausSigma.Text));
                SetOpcImageDeltaDim(Int32.Parse(ImageDeltaDim.Text));


                // Advanced functions that should normally not be changed
                SetSpx(Int32.Parse(Spx.Text));
                SetThreads(Int32.Parse(Threads.Text));
                SetErrorAsWarning(ErrorAsWarningCheckBox.IsChecked != null && (Boolean)ErrorAsWarningCheckBox.IsChecked);
                SetIgnoreODBLayerStepInSuffix(IgnoreODBLayerStepInSuffixCheckBox.IsChecked != null && (Boolean)IgnoreODBLayerStepInSuffixCheckBox.IsChecked);
                SetErrorThreshold(Int32.Parse(ErrorThresholdEdit.Text));
                SetShapeCompensation(ShapeCompensationCheckBox.IsChecked != null && (Boolean)ShapeCompensationCheckBox.IsChecked);
                SetGbrFixRetryCount(Int32.Parse(GbrFixRetryCountEdit.Text));
                SetArcResolution(Int32.Parse(ArcResolutionEdit.Text));
                if (Double.TryParse(ChordErrorEdit.Text, NumberStyles.Float, nfi, out val_d))
                    SetChordError(val_d);
                else
                {
                    MessageBox.Show("ChordError input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }


                // Only for internal debug use, require developer mode dongle (not customers use)
                if (IsDevMode() == 1)
                {
                    SetRam(Int32.Parse(Ram.Text));
                    if (Double.TryParse(BlackBottom.Text, NumberStyles.Float, nfi, out val_d))
                        SetBlackBottom(val_d);
                    else
                    {
                        MessageBox.Show("BlackBottom input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                        return false;
                    }
                    if (Double.TryParse(BlackTop.Text, NumberStyles.Float, nfi, out val_d))
                        SetBlackTop(val_d);
                    else
                    {
                        MessageBox.Show("BlackTop input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                        return false;
                    }
                    SetBlackRestTop(BlackRestTop.IsChecked != null && (Boolean)BlackRestTop.IsChecked);
                    SetBlackRestBottom(BlackRestBottom.IsChecked != null && (Boolean)BlackRestBottom.IsChecked);
                    SetMakeBin(MakeBinCheckBox.IsChecked != null && (Boolean)MakeBinCheckBox.IsChecked);
                    SetRegDebug(RegDebugCheckBox.IsChecked != null && (Boolean)RegDebugCheckBox.IsChecked);
                    SetKeepTiff(KeepTiffCheckBox.IsChecked != null && (Boolean)KeepTiffCheckBox.IsChecked);
                    SetSaveBitmap(SaveBitmapCheckBox.IsChecked != null && (Boolean)SaveBitmapCheckBox.IsChecked);
                    SetKeepTmp(KeepTmpCheckBox.IsChecked != null && (Boolean)KeepTmpCheckBox.IsChecked);
                    SetSilent(SilentCheckBox.IsChecked != null && (Boolean)SilentCheckBox.IsChecked);
                    SetBmpFrameDisabled(DisableBmpFrameCheckBox.IsChecked != null && (Boolean)DisableBmpFrameCheckBox.IsChecked);
                    SetImageFrameOverride(Int32.Parse(ImageFrameOverrideEdit.Text));
                }


            }
            catch (Exception)
            {
                MessageBox.Show("ERROR: Invalid input format in one or more text boxes.\nPlease check your input and try again.",
                                        "Invalid parameter format", MessageBoxButton.OK, MessageBoxImage.Error);
                return false;
            }
            return true;
        }

        private void RunPreprocessing()
        {
            if (!RltSetValues())
            {
                Status.Text = "FAILED";
                return;
            }
            Status.Text = "PROCESSING";
            OpenButton.IsEnabled = false;
            ClearButton.IsEnabled = false;
            PreAnalyzeButton.IsEnabled = false;
            PhotoheadPitchCB.IsEnabled = false;
            NumberOfPhotoheadsCB.IsEnabled = false;
            AsyncButton.IsEnabled = false;
            UpdateFidsButton.IsEnabled = false;
            Progress = 0;
            processing_worker.RunWorkerAsync();
        }

        private void RunAsyncPreprocessing()
        {
            if (!RltSetValues())
            {
                Status.Text = "FAILED";
                return;
            }
            Status.Text = "PROCESSING";
            OpenButton.IsEnabled = false;
            ClearButton.IsEnabled = false;
            PreAnalyzeButton.IsEnabled = false;
            PhotoheadPitchCB.IsEnabled = false;
            NumberOfPhotoheadsCB.IsEnabled = false;
            AsyncButton.IsEnabled = false;
            UpdateFidsButton.IsEnabled = false;
            async_worker.RunWorkerAsync();
        }

        private void RunPreAnalyzing()
        {
            if (!RltSetValues())
            {
                Status.Text = "FAILED";
                return;
            }
            Status.Text = "ANALYZING";
            OpenButton.IsEnabled = false;
            ClearButton.IsEnabled = false;
            PreAnalyzeButton.IsEnabled = false;
            PhotoheadPitchCB.IsEnabled = false;
            NumberOfPhotoheadsCB.IsEnabled = false;
            AsyncButton.IsEnabled = false;
            UpdateFidsButton.IsEnabled = false;
            analyzing_worker.RunWorkerAsync();
        }

        private void RunUpdateFiducials()
        {
            string xml_file = "";
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.Title = "Select xml file of the job where fiducials will be updated";
            openFileDialog.Filter = "Job Configuration|*.xml|All Files|*.*";
            if (openFileDialog.ShowDialog() == true)
            {
                xml_file = openFileDialog.FileName;

                var job_path = Path.GetDirectoryName(xml_file);
                _status = GetResultMsg(UpdateFiducials(GerberInput.Text, job_path, Int32.Parse(GlobalFiducial.Text), Int32.Parse(LocalFiducial.Text)));
                Status.Text = _status;
            }
            else
            {
                _status = "FAILED";
            }

            string s = "Updating fiducials completed successfully.";
            var img = MessageBoxImage.Information;

            if (_status != "SUCCESS")
            {
                img = MessageBoxImage.Error;
                if (GetErrMsg(_strBuffer, BufferLen) != 0)
                    MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                s = "Updating fiducials failed!\n\nStatus: " + _status + "\n\n" + _strBuffer;
            }
            MessageBox.Show(Application.Current.MainWindow, s, "UpdateFiducials: " + _status, MessageBoxButton.OK, img);
        }


        private void OnGerberBrowsClick(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
                GerberInput.Text = openFileDialog.FileName;

            if (OdbButton.IsChecked == true)
            {
                var browser = new OdbBrowser(openFileDialog.FileName, OutputDir.Text, WorkDir.Text);
                if (browser.IsReadyToBrowse() == true)
                {
                    browser.ShowDialog();
                    odb_selected = browser.odbSelected();
                }
            }
        }

        private void OnOutputBrowsClick(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.ShowDialog();
            OutputDir.Text = dialog.SelectedPath;
        }

        private void OnProcessClick(object sender, RoutedEventArgs e)
        {
            RunPreprocessing();
        }

        private void OnAsyncClick(object sender, RoutedEventArgs e)
        {
            RunAsyncPreprocessing();
        }

        private void OnClearClick(object sender, RoutedEventArgs e)
        {
            RltReset();
        }

        private void OnOpenClick(object sender, RoutedEventArgs e)
        {
            if (GetResultPath(_strBuffer, BufferLen) != 0)
            {
                MessageBox.Show(Application.Current.MainWindow, "GetResultPath exception", "Result Path string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
            Process.Start("explorer.exe", _strBuffer.ToString());
        }


        bool SetGerberWindowSelection()
        {
            if (AutoWindowRadioButton.IsChecked == true)
            {
                double val_d;
                if (Double.TryParse(OffsetX.Text, NumberStyles.Float, nfi, out val_d))
                    SetOffsetX(val_d);
                else
                {
                    MessageBox.Show("OffsetX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }

                if (Double.TryParse(OffsetY.Text, NumberStyles.Float, nfi, out val_d))
                    SetOffsetY(val_d);
                else
                {
                    MessageBox.Show("OffsetY input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            else
            {
                try
                {
                    SetFixedWindow(Double.Parse(WindowLLx.Text, nfi),
                                   Double.Parse(WindowLLy.Text, nfi),
                                   Double.Parse(WindowURx.Text, nfi),
                                   Double.Parse(WindowURy.Text, nfi));
                }
                catch (Exception)
                {
                    MessageBox.Show("Fixed Window input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            return true;
        }

        private void OnAutoWindowChecked(object sender, RoutedEventArgs e)
        {
            Llx.Visibility = Visibility.Collapsed;
            Lly.Visibility = Visibility.Collapsed;
            Urx.Visibility = Visibility.Collapsed;
            Ury.Visibility = Visibility.Collapsed;
            SetFixedWindow(0.0, 0.0, 0.0, 0.0);

            OffX.Visibility = Visibility.Visible;
            OffY.Visibility = Visibility.Visible;

            UpdateGerberWindowSelection();
        }

        private void OnFixedWindowChecked(object sender, RoutedEventArgs e)
        {
            Llx.Visibility = Visibility.Visible;
            Lly.Visibility = Visibility.Visible;
            Urx.Visibility = Visibility.Visible;
            Ury.Visibility = Visibility.Visible;
            OffX.Visibility = Visibility.Collapsed;
            OffY.Visibility = Visibility.Collapsed;
            UpdateGerberWindowSelection();
        }

        void UpdateGerberWindowSelection()
        {
            if (AutoWindowRadioButton.IsChecked == true)
            {
                OffsetX.Text = string.Format(nfi, "{0:N4}", GetOffsetX());
                OffsetY.Text = string.Format(nfi, "{0:N4}", GetOffsetY());
            }
            else
            {
                WindowLLx.Text = string.Format(nfi, "{0:N4}", GetWindowLLX());
                WindowLLy.Text = string.Format(nfi, "{0:N4}", GetWindowLLY());
                WindowURx.Text = string.Format(nfi, "{0:N4}", GetWindowURX());
                WindowURy.Text = string.Format(nfi, "{0:N4}", GetWindowURY());
            }
        }

        private void OnWorkBrowsClick(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.ShowDialog();
            WorkDir.Text = dialog.SelectedPath;
        }

        private void OnLogoBrowsClick(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
                LogoFileEdit.Text = openFileDialog.FileName;
        }

        private void OnLogoAddClick(object sender, RoutedEventArgs e)
        {
            try
            {
                double val_d;
                var l = new Logo();
                l.GerberFile = LogoFileEdit.Text;
                l.Dcode = Int32.Parse(LogoDcodeEdit.Text);
                l.Rotation = Int32.Parse(RotationSPDEdit.Text);
                l.Inverse = (Boolean)InverseSPDCheckBox.IsChecked;
                l.FlipH = (Boolean)FlipXSPDCheckBox.IsChecked;
                l.FlipV = (Boolean)FlipYSPDCheckBox.IsChecked;
                if (Double.TryParse(ScaleXSPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    l.ScaleX = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                if (Double.TryParse(ScaleYSPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    l.ScaleY = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleY input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                LogoList.Add(l);

                LogoListStr.Add("Dcode=" + LogoDcodeEdit.Text + ", File=" + LogoFileEdit.Text + ", Rotation=" + RotationSPDEdit.Text +
                    ", ScaleX=" + ScaleXSPDEdit.Text + ", ScaleY=" + ScaleYSPDEdit.Text + ", Inverse=" + InverseSPDCheckBox.IsChecked +
                    ", FlipH=" + FlipXSPDCheckBox.IsChecked + ", FlipV=" + FlipYSPDCheckBox.IsChecked);
            }
            catch (Exception)
            {
                MessageBox.Show("Missing or invalid dynamic paneldata fields.", "Missing fields", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void OnClearLogoListClick(object sender, RoutedEventArgs e)
        {
            LogoListStr.Clear();
            LogoList.Clear();
        }


        private void OnLogoRemoveClick(object sender, RoutedEventArgs e)
        {
            try
            {
                LogoList.RemoveAt(LogoListView.SelectedIndex);
                LogoListStr.RemoveAt(LogoListView.SelectedIndex);
            }
            catch (Exception)
            {
                MessageBox.Show("No static paneldata selected for removal.", "No selection", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void OnDynamicPDAddClick(object sender, RoutedEventArgs e)
        {
            try
            {
                double val_d;
                var d = new DynamicPD();
                d.IsCoords = false;
                d.Dcode = Int32.Parse(DcodeDPDEdit.Text);
                d.Type = TypeDPDComboBox.SelectedIndex;
                d.Content = ContentPDComboBox.SelectedIndex;
                d.Rotation = Int32.Parse(RotationDPDEdit.Text);
                if (Double.TryParse(SizeXDPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    d.SizeX = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                if (Double.TryParse(SizeYDPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    d.SizeY = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleY input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }
                d.Inverse = (InverseDPDCheckBox.IsChecked != null && (Boolean)InverseDPDCheckBox.IsChecked);

                DynamicPDList.Add(d);

                DynamicPDListStr.Add("Dcode=" + DcodeDPDEdit.Text +
                    ", Content=" + ContentPDComboBox.SelectedIndex +
                    ", Type=" + TypeDPDComboBox.SelectedIndex +
                    ", Rotation=" + RotationDPDEdit.Text +
                    ", SizeX=" + SizeXDPDEdit.Text + ", SizeY=" + SizeYDPDEdit.Text +
                    ", Inverse=" + d.Inverse.ToString());
            }
            catch (Exception)
            {
                MessageBox.Show("Missing or invalid dynamic paneldata fields.", "Missing fields", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void OnDynamicPDAddCoordsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                double val_d;
                var d = new DynamicPD();
                d.IsCoords = true;
                d.Dcode = Int32.Parse(DcodeDPDEdit.Text);
                d.Type = TypeDPDComboBox.SelectedIndex;
                d.Content = ContentPDComboBox.SelectedIndex;
                d.Rotation = Int32.Parse(RotationDPDEdit.Text);
                if (Double.TryParse(SizeXDPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    d.SizeX = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleX input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                if (Double.TryParse(SizeYDPDEdit.Text, NumberStyles.Float, nfi, out val_d))
                    d.SizeY = val_d;
                else
                {
                    MessageBox.Show("PanelData ScaleY input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }
                d.Inverse = (InverseDPDCheckBox.IsChecked != null && (Boolean)InverseDPDCheckBox.IsChecked);


                var dialog = new CoordsInput
                {

                };
                if (dialog.ShowDialog() == true)
                {
                    d.coords = dialog.GetPaneldataCoords();
                    MessageBox.Show("Successfully added " + d.coords.Count.ToString() + " coords to the paneldata ID.", 
                        "Paneldata ID coords added", MessageBoxButton.OK, MessageBoxImage.Information);
                }
                else
                {
                    MessageBox.Show("Failed to add any coordinates! Try again and check the format.",
                        "Failed to add Paneldata ID coords", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }


                DynamicPDList.Add(d);

                DynamicPDListStr.Add("ID=" + DcodeDPDEdit.Text +
                    ", Content=" + ContentPDComboBox.SelectedIndex +
                    ", Type=" + TypeDPDComboBox.SelectedIndex +
                    ", Rotation=" + RotationDPDEdit.Text +
                    ", SizeX=" + SizeXDPDEdit.Text + ", SizeY=" + SizeYDPDEdit.Text +
                    ", Inverse=" + d.Inverse.ToString());
            }
            catch (Exception)
            {
                MessageBox.Show("Missing or invalid dynamic paneldata fields.", "Missing fields", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void OnDynamicPDRemoveClick(object sender, RoutedEventArgs e)
        {
            try
            {
                DynamicPDList.RemoveAt(DynamicPDListView.SelectedIndex);
                DynamicPDListStr.RemoveAt(DynamicPDListView.SelectedIndex);
            }
            catch (Exception)
            {
                MessageBox.Show("No dynamic paneldata selected for removal.", "No selection", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void OnClearDynamicPDListClick(object sender, RoutedEventArgs e)
        {
            DynamicPDList.Clear();
            DynamicPDListStr.Clear();
        }

        private void OnPreAnalyzeClick(object sender, RoutedEventArgs e)
        {
            RunPreAnalyzing();
        }

        private void NumPHSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            int ph_no = (int)NumberOfPhotoheadsCB.SelectedValue;
            UpdateNomPhDist(ph_no);
        }

        private bool SaveNomPhDist()
        {
            int ph_num = GetPhotoheadNumber();
            double val_d;

            if (ph_num >= 2)
            {
                if (Double.TryParse(NominalPhotoheadDist2Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 2);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #2 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 3)
            {
                if (Double.TryParse(NominalPhotoheadDist3Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 3);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #3 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 4)
            {
                if (Double.TryParse(NominalPhotoheadDist4Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 4);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #4 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 5)
            {
                if (Double.TryParse(NominalPhotoheadDist5Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 5);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #5 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 6)
            {
                if (Double.TryParse(NominalPhotoheadDist6Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 6);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #6 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 7)
            {
                if (Double.TryParse(NominalPhotoheadDist7Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 7);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #7 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }
            if (ph_num >= 8)
            {
                if (Double.TryParse(NominalPhotoheadDist8Edit.Text, NumberStyles.Float, nfi, out val_d))
                    SetNominalPhotoheadDist(val_d, 8);
                else
                {
                    MessageBox.Show("NominalPhotoheadDistance #8 input format is invalid.", "Invalid input format", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return false;
                }
            }

            return true;
        }

        private void UpdateNomPhDist(int ph_no)
        {
            int ph_num = ph_no;

            if (ph_num >= 2)
            {
                NominalPhotoheadDist2Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist2Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(2);
                if (dist > 0)
                    NominalPhotoheadDist2Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist2Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist2Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist2Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 3)
            {
                NominalPhotoheadDist3Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist3Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(3);
                if (dist > 0)
                    NominalPhotoheadDist3Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist3Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist3Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist3Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 4)
            {
                NominalPhotoheadDist4Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist4Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(4);
                if (dist > 0)
                    NominalPhotoheadDist4Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist4Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist4Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist4Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 5)
            {
                NominalPhotoheadDist5Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist5Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(5);
                if (dist > 0)
                    NominalPhotoheadDist5Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist5Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist5Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist5Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 6)
            {
                NominalPhotoheadDist6Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist6Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(6);
                if (dist > 0)
                    NominalPhotoheadDist6Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist6Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist6Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist6Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 7)
            {
                NominalPhotoheadDist7Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist7Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(7);
                if (dist > 0)
                    NominalPhotoheadDist7Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist7Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist7Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist7Edit.Visibility = Visibility.Collapsed;
            }
            if (ph_num >= 8)
            {
                NominalPhotoheadDist8Label.Visibility = Visibility.Visible;
                NominalPhotoheadDist8Edit.Visibility = Visibility.Visible;
                var dist = GetNominalPhotoheadDist(8);
                if (dist > 0)
                    NominalPhotoheadDist8Edit.Text = string.Format(nfi, "{0:N2}", dist);
                else
                    NominalPhotoheadDist8Edit.Text = String.Empty;
            }
            else
            {
                NominalPhotoheadDist8Label.Visibility = Visibility.Collapsed;
                NominalPhotoheadDist8Edit.Visibility = Visibility.Collapsed;
            }
        }


        private string GetStateMsg(int id)
        {
            string stateMsg = "";

            switch (id)
            {
                case 100:
                    stateMsg += "Idle"; break;
                case 101:
                    stateMsg += "Starting..."; break;
                case 102:
                    stateMsg += "Initializing..."; break;
                case 103:
                    stateMsg += "Rasterizing..."; break;
                case 104:
                    stateMsg += "Calculating..."; break;
                case 105:
                    stateMsg += "MakingPreview..."; break;
                case 106:
                    stateMsg += "WritingStripes..."; break;
                case 107:
                    stateMsg += "Finalizing..."; break;
                case 108:
                    stateMsg += "OPC..."; break;
                case 109:
                    stateMsg += "Aborting..."; break;
                default:
                    stateMsg += "UndefinedStateId"; break;
            }
            return stateMsg;
        }

        private string GetResultMsg(int id)
        {
            string resultMsg = "";

            switch (id)
            {
                case 0:
                    resultMsg += "SUCCESS"; break;
                case 1:
                    resultMsg += "INVALID_INPUT"; break;
                case 2:
                    resultMsg += "CONVERSION_FAIL"; break;
                case 3:
                    resultMsg += "PIXEL_CALC_FAIL"; break;
                case 4:
                    resultMsg += "FIDUCIAL_FAIL"; break;
                case 5:
                    resultMsg += "PANEL_DATA_FAIL"; break;
                case 6:
                    resultMsg += "SAVE_PREVIEW_FAIL"; break;
                case 7:
                    resultMsg += "STRIPE_SPLIT_FAIL"; break;
                case 8:
                    resultMsg += "SAVE_TO_FILE_FAIL"; break;
                case 9:
                    resultMsg += "RESULT_FOLDER_FAIL"; break;
                case 10:
                    resultMsg += "LOGGING_FAILURE"; break;
                case 11:
                    resultMsg += "LICENSE_ERROR"; break;
                case 12:
                    resultMsg += "INIT_ERROR"; break;
                case 13:
                    resultMsg += "ALREADY_RUNNING"; break;
                case 14:
                    resultMsg += "EXIT_ERROR"; break;
                case 15:
                    resultMsg += "DLL_PATH_ERROR"; break;
                case 16:
                    resultMsg += "LOAD_FROM_FILE_ERROR"; break;
                case 17:
                    resultMsg += "TEXT_TOO_LONG_ERROR"; break;
                case 18:
                    resultMsg += "INTERNAL_ERROR"; break;
                case 19:
                    resultMsg += "OPC_FAIL"; break;
                case 20:
                    resultMsg += "ABORTED"; break;
                default:
                    resultMsg += "UndefinedResultId"; break;
            }
            return resultMsg;
        }

        private void OnUpdateFidsClick(object sender, RoutedEventArgs e)
        {
            RunUpdateFiducials();
        }



        private void OnLoadPanelDataClick(object sender, RoutedEventArgs e)
        {
            string pd_file;
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                pd_file = openFileDialog.FileName;
                string[] content = System.IO.File.ReadAllLines(pd_file);
                ParseAndSavePanelData(content);
            }
        }


        // Example of expected content of a paneldata text input line:
        // dcode=123|text_size=500|text_string=this is a text string|serial_string=serial####number|serial_symbol=#|serial_start=0|flip_h=false|flipV=false|
        // is_coords=true|id_coords=3486,85:-1134,78;55,4:87.0
        private void ParseAndSavePanelData(string[] lines)
        {
            string warning_msg = "";

            foreach (string line in lines)
            {
                var d = new DynamicPD();
                string[] parameters = line.Split('|');

                d.IsCoords = false;

                // if line does not start with 'dcode', go to next line
                if (!parameters[0].StartsWith("dcode="))
                {
                    warning_msg += "Failed to identify dcode keyword in start of line. Go to next line.\n";
                    continue;
                }

                // Search all parameters in line
                foreach (var param in parameters)
                {
                    // set dcode
                    if (param.StartsWith("dcode="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Dcode = val;
                        else
                        {
                            warning_msg += "Failed to set Dcode. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set type
                    if (param.StartsWith("type="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Type = val;
                        else
                        {
                            warning_msg += "Failed to set Type. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set content
                    if (param.StartsWith("content="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Content = val;
                        else
                        {
                            warning_msg += "Failed to set Content. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set rotation
                    if (param.StartsWith("rotation="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Rotation = val;
                        else
                        {
                            warning_msg += "Failed to set Rotation. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set sizeX
                    if (param.StartsWith("size_x="))
                    {
                        double val;
                        if (Double.TryParse(param.Split('=')[1], out val))
                            d.SizeX = val;
                        else
                        {
                            warning_msg += "Failed to set SizeX. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set sizeY
                    if (param.StartsWith("size_y="))
                    {
                        double val;
                        if (Double.TryParse(param.Split('=')[1], out val))
                            d.SizeY = val;
                        else
                        {
                            warning_msg += "Failed to set SizeY. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set inverse
                    if (param.StartsWith("inverse="))
                    {
                        bool val;
                        if (Boolean.TryParse(param.Split('=')[1], out val))
                            d.Inverse = val;
                        else
                        {
                            warning_msg += "Failed to set Inverse. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set dcode/id coords type
                    if (param.StartsWith("is_coords="))
                    {
                        bool val;
                        if (Boolean.TryParse(param.Split('=')[1], out val))
                            d.IsCoords = val;
                        else
                        {
                            warning_msg += "Failed to set IsCoords. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set manual id coords input
                    if (param.StartsWith("id_coords="))
                    {
                        var coords_str = param.Split('=')[1].Split(';');
                        
                        foreach(var c in coords_str)
                        {
                            double x, y;
                            if (Double.TryParse(c.Split(':')[0], out x) && Double.TryParse(c.Split(':')[1], out y))
                            {
                                if (d.coords == null)
                                    d.coords = new List<double[]>();
                                d.coords.Add(new[] { x, y });
                            }
                            else
                            {
                                warning_msg += "Failed to add paneldata id coords from file. Input format is invalid, skipping line.\n";
                            }
                        }
                    }
                }

                DynamicPDList.Add(d);

                DynamicPDListStr.Add("Dcode=" + d.Dcode.ToString() +
                    ", Content=" + d.Content.ToString() +
                    ", Type=" + d.Type.ToString() +
                    ", Rotation=" + d.Rotation.ToString() +
                    ", SizeX=" + d.SizeX.ToString() + ", SizeY=" + d.SizeY.ToString() +
                    ", Inverse=" + d.Inverse.ToString());
            }

            if (warning_msg != "")
                MessageBox.Show(warning_msg, "Invalid format in input file", MessageBoxButton.OK, MessageBoxImage.Warning);

        }

        private void ChangePreProAPIPath()
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.ShowDialog();
            string apiDir = dialog.SelectedPath; ;

            // Specify where this application will search for dependency DLLs if located in another folder than PreProcessAPI.dll
            SetDllDirectory(apiDir);

            // Specify to PreProcessAPI where it should look for dependencies and place setting/log files etc.
            SetDllPath(apiDir);
        }

        private void OnLoadClick(object sender, RoutedEventArgs e)
        {
            LoadParameters();
            UpdateParameters();
            UpdateNomPhDist(GetPhotoheadNumber());
        }

        private void OnSaveClick(object sender, RoutedEventArgs e)
        {
            RltSetValues();
            SaveParameters();
        }

        private void PhPitchSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {

        }

        private void ProductIDChangedChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {

        }


        private void OnLoadConvolutionMatrixClick(object sender, RoutedEventArgs e)
        {
            string sim_file;
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                sim_file = openFileDialog.FileName;
                string[] content = System.IO.File.ReadAllLines(sim_file);
                ParseAndSaveSimFile(content);
            }
        }


        private void ParseAndSaveSimFile(string[] lines)
        {
            string warning_msg = "";

            foreach (string line in lines)
            {
                var d = new DynamicPD();
                string[] parameters = line.Split('|');

                // if line does not start with 'dcode', go to next line
                if (!parameters[0].StartsWith("dcode="))
                {
                    warning_msg += "Failed to identify dcode keyword in start of line. Go to next line.\n";
                    continue;
                }

                // Search all parameters in line
                foreach (var param in parameters)
                {
                    // set dcode
                    if (param.StartsWith("dcode="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Dcode = val;
                        else
                        {
                            warning_msg += "Failed to set Dcode. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set type
                    if (param.StartsWith("type="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Type = val;
                        else
                        {
                            warning_msg += "Failed to set Type. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set content
                    if (param.StartsWith("content="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Content = val;
                        else
                        {
                            warning_msg += "Failed to set Content. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set rotation
                    if (param.StartsWith("rotation="))
                    {
                        int val;
                        if (Int32.TryParse(param.Split('=')[1], out val))
                            d.Rotation = val;
                        else
                        {
                            warning_msg += "Failed to set Rotation. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set sizeX
                    if (param.StartsWith("size_x="))
                    {
                        double val;
                        if (Double.TryParse(param.Split('=')[1], out val))
                            d.SizeX = val;
                        else
                        {
                            warning_msg += "Failed to set SizeX. Input format is invalid, skipping line.\n";
                        }
                    }

                    // set sizeY
                    if (param.StartsWith("size_y="))
                    {
                        double val;
                        if (Double.TryParse(param.Split('=')[1], out val))
                            d.SizeY = val;
                        else
                        {
                            warning_msg += "Failed to set SizeY. Input format is invalid, skipping line.\n";
                        }
                    }
                }

                DynamicPDList.Add(d);

                DynamicPDListStr.Add("Dcode=" + d.Dcode.ToString() +
                    ", Content=" + d.Content.ToString() +
                    ", Type=" + d.Type.ToString() +
                    ", Rotation=" + d.Rotation.ToString() +
                    ", SizeX=" + d.SizeX.ToString() + ", SizeY=" + d.SizeY.ToString());
            }

            if (warning_msg != "")
                MessageBox.Show(warning_msg, "Invalid format in input file", MessageBoxButton.OK, MessageBoxImage.Warning);


        }


        private void OnPrepareClick(object sender, RoutedEventArgs e)
        {
            if (OdbButton.IsChecked == true)
            {
                var browser = new OdbBrowser(GerberInput.Text, OutputDir.Text, WorkDir.Text);
                if (browser.IsReadyToBrowse() == true)
                {
                    browser.ShowDialog();
                    odb_selected = browser.odbSelected();
                }
            }
            else if (GdsButton.IsChecked == true)
            {
                gds_selected = false;
                int err = InitGDS(GerberInput.Text, OutputDir.Text, WorkDir.Text);
                if (err == 0x1000D)
                {
                    var err_msg = "Failed to validate GDSII license.\nPlease make sure hardware dongle is inserted and make sure you have a valid license.";
                    MessageBox.Show(err_msg, "License error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
                else if (err != 0)
                {
                    if (GetErrMsg(_strBuffer, BufferLen) != 0)
                        MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                    var err_msg = "Could not initiate GDSII file.\n\n" + _strBuffer;
                    MessageBox.Show(err_msg, "GDSII error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                int gdsNestingLevel = 0;
                if (GdsNestingLevelEdit.Text != String.Empty)
                    gdsNestingLevel = Int32.Parse(GdsNestingLevelEdit.Text);

                // progress bar while converting GDSII/OASIS 
                ProgressDialogResult result2 = ProgressDialog.Execute(MainWindow.GetWindow(this), "Converting GDSII to Gerber...", () =>
                {
                    err = GDSConvert(gdsNestingLevel);
                }, new ProgressDialogSettings(false, false, true));

                if (err != 0)
                {
                    if (GetErrMsg(_strBuffer, BufferLen) != 0)
                        MessageBox.Show(Application.Current.MainWindow, "ErrorMessage exception", "Error message string is larger than string buffer, will be truncated!", MessageBoxButton.OK, MessageBoxImage.Warning);
                    var err_msg = "Failed to convert GDSII file.\n\n" + _strBuffer;
                    MessageBox.Show(err_msg, "GDSII error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                gds_selected = true;
            }
            else
            {
                MessageBox.Show("ODB++/GDSII preproccesing not enabled. Please select correct radio button \"ODB++\" or \"GDSII\".", "ODB/GSII error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
        }

        private void OdbButton_Checked(object sender, RoutedEventArgs e)
        {
            PreConvertButton.IsEnabled = true;
            gds_selected = false;
        }

        private void GerberButton_Checked(object sender, RoutedEventArgs e)
        {
            if (PreConvertButton != null)
            {
                PreConvertButton.IsEnabled = false;
            }
            odb_selected = false;
            gds_selected = false;
        }

        private void GdsButton_Checked(object sender, RoutedEventArgs e)
        {
            PreConvertButton.IsEnabled = true;
            odb_selected = false;

        }

        private void OnAbortClick(object sender, RoutedEventArgs e)
        {
            _status = "ABORTING";
            AbortProcessing();
        }

        private void OnODBScannerClick(object sender, RoutedEventArgs e)
        {
            var browser = new OdbBrowser(GerberInput.Text);
            if (browser.IsReadyToBrowse() == true)
            {
                browser.ShowDialog();
                odb_selected = browser.odbSelected();
            }

            if (GetOdbStep(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetOdbStep exception", "OdbStep string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            OdbStepEdit.Text = _strBuffer.ToString();

            if (GetOdbLayer(_strBuffer, BufferLen) != 0)
                MessageBox.Show(Application.Current.MainWindow, "GetOdbLayer exception", "OdbLayer string is larger than string buffer!", MessageBoxButton.OK, MessageBoxImage.Warning);
            OdbLayerEdit.Text = _strBuffer.ToString();
        }
    }
}
