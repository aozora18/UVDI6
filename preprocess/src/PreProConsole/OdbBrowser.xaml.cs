using Parago.Windows;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;

namespace PreProConsole
{
    public partial class OdbBrowser : Window
    {

        private const string PRE_PRO_DLL = "PreProcessApi.dll";

        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int InitODB(string input_file, string output_dir, string work_dir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int InitODBScan(string input_file);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetStepCount(ref int count);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerCount(ref int count);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetStepName(int step, StringBuilder buf, int bufsz);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetStepDcodeInfo(int step, ref int dcode_start, ref int dcode_count);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerName(int layer, StringBuilder buf, int bufsz);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerDcodeInfo(int layer, ref int dcode_start, ref int dcode_count);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerType(int layer, ref int type);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerContext(int layer, ref int context);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetLayerPolarity(int layer, ref int polarity);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBSetActiveLayerAndStep(int step, int layer);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ODBGetActiveLayerAndStep(ref int step, ref int layer);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetErrMsg(StringBuilder rntStr, int len);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetWorkDir(string workDir);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetOdbStep(string step);
        [DllImport(PRE_PRO_DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetOdbLayer(string layer);

        private bool odb_selected = false;
        private bool is_ready_to_browse = false;
        private bool is_fast_mode = false;

        // For PreProcessing
        public OdbBrowser(String file, String output_dir, String work_dir)
        {
            InitializeComponent();
            List<OdbStep> items = new List<OdbStep>();
            List<OdbLayer> layers = new List<OdbLayer>();

            int err = InitODB(file, output_dir, work_dir);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not initialize ODB database file", err);
                return;
            }
            int stepsInFile = 0;
            err = ODBGetStepCount(ref stepsInFile);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not get step count", err);
                return;
            }
            for (int i = 0; i < stepsInFile; i++)
            {
                var buf = new StringBuilder(1024);
                err = ODBGetStepName(i, buf, 1024);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get step name", err);
                    return;
                }
                var stepName = buf.ToString();
                int dCodeStart = 0, dcodeCount = 0;
                err = ODBGetStepDcodeInfo(i, ref dCodeStart, ref dcodeCount);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get step Dcode info", err);
                    return;
                }
                var step = new OdbStep();
                step.Name = stepName;
                step.DcodeStart = dCodeStart;
                step.DcodeCount = dcodeCount;
                step.Index = i;
                items.Add(step);
            }
            stepsListView.ItemsSource = items;

            // Get layers
            int layersInFile = 0;
            err = ODBGetLayerCount(ref layersInFile);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not get layer count", err);
                return;
            }
            for (int i = 0; i < layersInFile; i++)
            {
                var buf = new StringBuilder(1024);
                err = ODBGetLayerName(i, buf, 1024);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get layer name", err);
                    return;
                }
                var layerName = buf.ToString();

                int dcodeStart = 0, dcodeCount = 0;
                err = ODBGetLayerDcodeInfo(i, ref dcodeStart, ref dcodeCount);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get layer Dcode info", err);
                    return;
                }
                int context = 0;
                err = ODBGetLayerContext(i, ref context);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get layer context", err);
                    return;
                }
                int type = 0;
                err = ODBGetLayerType(i, ref type);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get layer type", err);
                    return;
                }
                int polarity = 0;
                err = ODBGetLayerPolarity(i, ref polarity);

                var layer = new OdbLayer();
                layer.Name = layerName;
                layer.DcodeStart = dcodeStart;
                layer.DcodeCount = dcodeCount;
                layer.Index = i;
                layer.Context = context;
                layer.Type = type;
                layer.Polarity = polarity;
                layers.Add(layer);
            }
            layersListView.ItemsSource = layers;
            is_ready_to_browse = true;
        }

        // For PreProFast
        public OdbBrowser(String file)
        {
            is_fast_mode = true;

            InitializeComponent();
            List<OdbStep> items = new List<OdbStep>();
            List<OdbLayer> layers = new List<OdbLayer>();

            int err = InitODBScan(file);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not initialize ODB database file", err);
                return;
            }
            int stepsInFile = 0;
            err = ODBGetStepCount(ref stepsInFile);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not get step count", err);
                return;
            }
            for (int i = 0; i < stepsInFile; i++)
            {
                var buf = new StringBuilder(1024);
                err = ODBGetStepName(i, buf, 1024);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get step name", err);
                    return;
                }
                var stepName = buf.ToString();
                
                var step = new OdbStep();
                step.Name = stepName;
                step.Index = i;
                items.Add(step);
            }
            stepsListView.ItemsSource = items;

            // Get layers
            int layersInFile = 0;
            err = ODBGetLayerCount(ref layersInFile);
            if (err != 0)
            {
                ShowODBErrorMessage("Could not get layer count", err);
                return;
            }
            for (int i = 0; i < layersInFile; i++)
            {
                var buf = new StringBuilder(1024);
                err = ODBGetLayerName(i, buf, 1024);
                if (err != 0)
                {
                    ShowODBErrorMessage("Could not get layer name", err);
                    return;
                }
                var layerName = buf.ToString();

                var layer = new OdbLayer();
                layer.Name = layerName;
                layer.Index = i;
                layers.Add(layer);
            }
            layersListView.ItemsSource = layers;
            is_ready_to_browse = true;
        }

        public bool IsReadyToBrowse()
        {
            return is_ready_to_browse;
        }

        private void ShowODBErrorMessage(string msg, int err)
        {
            var s = "ODB++ processing failed!\n\n" + msg;
            var img = MessageBoxImage.Information;
            var buf = new StringBuilder(1024);

            if (err == 65546)
            {
                MessageBox.Show(Application.Current.MainWindow, s + ":\r\n" + "Could not find valid ODB license", "ODB", MessageBoxButton.OK, img);
                return;
            }
            GetErrMsg(buf, buf.MaxCapacity);
            MessageBox.Show(Application.Current.MainWindow, s + ":\r\n" + buf.ToString(), "ODB", MessageBoxButton.OK, img);
        }

        public class OdbStep
        {
            public string Name { get; set; }
            public int DcodeStart { get; set; }
            public int DcodeCount { get; set; }
            public int Index { get; set; }
        }

        public class OdbLayer
        {
            public string Name { get; set; }
            public int DcodeStart { get; set; }
            public int DcodeCount { get; set; }
            public int Index { get; set; }
            public int Context { get; set; }
            public int Type { get; set; }
            public int Polarity { get; set; }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            
            var err = 0;

            if (is_fast_mode)
            {
                var selectedLayer = (OdbLayer)layersListView.SelectedItem;
                var selectedStep = (OdbStep)stepsListView.SelectedValue;

                if (selectedLayer == null || selectedStep == null)
                {
                    MessageBox.Show(Application.Current.MainWindow, "Please select layer and step", "ODBScan", MessageBoxButton.OK, MessageBoxImage.Exclamation);
                    return;
                }

                SetOdbStep(selectedStep.Name);
                SetOdbLayer(selectedLayer.Name);

                this.Close();
                odb_selected = true;
            }
            else
            {

                var selectedLayer = layersListView.SelectedIndex;
                var selectedStep = stepsListView.SelectedIndex;

                // progress bar while converting GDSII/OASIS 
                ProgressDialogResult result2 = ProgressDialog.Execute(MainWindow.GetWindow(this), "Converting ODB++ to Gerber...", () =>
                {
                    err = ODBSetActiveLayerAndStep(selectedStep, selectedLayer);
                }, new ProgressDialogSettings(false, false, true));

                if (err != 0)
                {
                    ShowODBErrorMessage("Could not set active layer and step", err);
                    return;
                }
                this.Close();
                odb_selected = true;
            }
        }

        public bool odbSelected()
        {
            return odb_selected;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

    }
}
