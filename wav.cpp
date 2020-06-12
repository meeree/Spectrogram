#include <cstdio>
void ReadWav (void* output, const char* filename)
{
    static size_t header_size = 44;

    FILE * fl = fopen(filename, "r");

    // Get size of file 
    fseek(fl, 0, SEEK_END);
    size_t sz = ftell(fl);
    rewind(fl);

    // Allocate output 
    output = malloc(output, sz * 

    fread(
    fclose(fl);
    ms_wavStream.seekg(/SLOW));
    ms_wavStream.read(bytes,4);
double channel = ((bytes[1]<<8)|bytes[0])/32768.0;
}
