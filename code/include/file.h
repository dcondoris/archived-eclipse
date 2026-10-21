#ifndef FILE_H
#define FILE_H

void openFile (const char * filename, long * file_descriptor, long flags, long mode);
void readFile (long buffer_size, char * buffer, long * file_descriptor, long * bytesRead);
void writeFile(long file_descriptor, const char * buffer, long buffer_size, long * bytesWritten);
void closeFile (long file_descriptor);

#endif