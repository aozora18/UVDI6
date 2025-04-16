using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Windows;

namespace PreProConsole
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class CoordsInput : Window
    {
        private List<double[]> paneldataCoordsList;

        public CoordsInput()
        {
            paneldataCoordsList = new List<double[]>();
            InitializeComponent();
        }

        public List<double[]> GetPaneldataCoords()
        {
            return paneldataCoordsList;
        }

        private void OKButton_Click(object sender, RoutedEventArgs e)
        {
            var paneldataCoordsStr = PaneldataCoordsTextBox.Text;
            paneldataCoordsList = ParsePaneldataCoordsText(paneldataCoordsStr);
            if (paneldataCoordsList.Count == 0)
                DialogResult = false;
            else
                DialogResult = true;
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        private List<double[]> ParsePaneldataCoordsText(string s)
        {
            var points = new List<double[]>();

            foreach (var tokens in from line in s.Split('\n').ToList() where line.Contains(';') select line.Split(';'))
            {
                for (var i = 0; i < tokens.Length; i++)
                {
                    tokens[i] = tokens[i].Trim('\r', ' ', '\t', 'x', 'X', 'y', 'Y', ':');
                    tokens[i] = tokens[i].Replace('.', ',');
                    tokens[i] = tokens[i].Replace(',', Convert.ToChar(CultureInfo.InvariantCulture.NumberFormat.NumberDecimalSeparator));
                }
                double x, y;
                if (Double.TryParse(tokens[0], NumberStyles.Float, CultureInfo.InvariantCulture, out x) &&
                    Double.TryParse(tokens[1], NumberStyles.Float, CultureInfo.InvariantCulture, out y))
                {
                    points.Add(new[] { x, y });
                }
            }
            return points;
        }
    }
}
