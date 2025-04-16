GERBER EXTRACT v1.0 (Mar 13, 2012)
(c) 2012 Artwork Conversion Software Inc. 
417 Ingalls St. Santa Cruz CA 95060
T: +1 (831) 424-6163
E: info@artwork.com
W: http://www.artwork.com

Frequently Asked Questions:

01. How to run GerberExtract (Win64) ?
-A: Install the GerberExtract-Win64 package in <install_dir>. From <install_dir> run gbrxtrakapp64.exe with the appropriate command line options

02. What supporting files need to be present alongside GerberExtact binary ?
-A: acs.key, acsbool64.dll, zlibwapi.dll, colfill.pat and gdsfont.shx must be present in the same directory as gbrxtrakapp64.exe

03. Which platforms are supported by GerberExtract ?
-A: At present, only Windows 64bit is available. Upon request, Windows 32bit, Red Hat Linux E4 64,32 bit and Solaris 8 64,32 bit can be made available

04. What does GerberExtract do ?
-A: Extracts data from an OASIS/GDSII file to Gerber

05. How to extract the entire default top cell from an OASIS/GDSII file ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file>

06. How to extract a specific cell (TEST) from an OASIS/GDSII file ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -cell:TEST

07. How to extract a specific set of layers (1,2:2,3) from an OASIS/GDSII file ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -layers:1,2:2,3

08. How to extract a specific window from an OASIS/GDSII file ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -window:<minx>,<miny>,<maxx>,<maxy>

09. How does GerberExtract attempt to reduce the output Gerber file size ?
-A: GerberExtract uses pattern recognition techniques to find repeating patterns that have the same pitch in X and Y directions. Each of these patterns can be then represented as SR (Step and Repeat) records thereby acheiving data compaction

10. How to extract Gerber without compaction (SR records) ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -nocompress

11. How to generate log file at a specific localtion <log_file> ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -log:<log_file>

12. How to run GerberExtract silently in the background ?
-A: Run: <install_dir>\gbrxtrakapp64.exe <input_file> <output_file> -silent. All messages to stdout will be suppressed

13. What is the units of the Gerber output ?
-A: The output Gerber units are always MM with precision 3,5

14. What is the polarity of the Gerber output ?
-A: The output Gerber polarity is always positive (IPPOS)


