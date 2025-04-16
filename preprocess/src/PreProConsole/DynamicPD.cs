using System.Collections.Generic;

namespace PreProConsole
{
    public class DynamicPD
    {
        public int Dcode { get; set; }
        public int Rotation { get; set; }
        public double SizeX { get; set; }
        public double SizeY { get; set; }
        public int Type { get; set; }
        public int Content { get; set; }
        public bool Inverse { get; set; }
        public bool IsCoords { get; set; }

        public List<double[]> coords;

    }
}
