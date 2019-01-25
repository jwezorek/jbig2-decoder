jbig2-decoder
=======
Command line utility for decoding JBig2 (.jb2) files built using source code from PDFium (https://opensource.google.com/projects/pdfium).

usage is like

1. **`jbig2-decoder.exe input.jb2`**  
defaults to a png formatted image file "input.png"
2. **`jbig2-decoder.exe -o test.png input.jb2`**  
optionally specify an output file name with format implied by extension
3. **`jbig2-decoder.exe -f png input.jb2`**  
optionally explicitely specify an output format (can be png, bmp, or jpg, currently)
4. **`jbig2-decoder.exe global_stream_fname page_stream_fname`**  
use a 'global' and 'page' stream (extracted from a pdf, say)
     
Currently I just have a solution file for Visual Studio 2017 and its written to C++17.
PDFium is permissively licensed, BSD, so so is this. Also uses getopt_pp which is boost-style licensed, and stb-image-write.h which is public domain
