//
// Created by danie on 12-12-2022.
//

#include "zipping.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paths.h"

/* CHUNK is the size of the memory chunk used by the zlib routines. */

#define CHUNK 0x4000

/* The following macro calls a zlib routine and checks the return
   value. If the return value ("status") is not OK, it prints an error
   message and exits the program. Zlib's error statuses are all less
   than zero. */

#define CALL_ZLIB(x) {                                                  \
        int status;                                                     \
        status = x;                                                     \
        if (status < 0) {                                               \
            fprintf (stderr,                                            \
                     "%s:%d: %s returned a bad status of %d.\n",        \
                     __FILE__, __LINE__, #x, status);                   \
            exit (EXIT_FAILURE);                                        \
        }                                                               \
    }

/* These are parameters to deflateInit2. See
   http://zlib.net/manual.html for the exact meanings. */

#define windowBits 15
#define GZIP_ENCODING 16

static void strm_init (z_stream * strm)
{
    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;
    CALL_ZLIB (deflateInit2(strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                             windowBits | GZIP_ENCODING, 8,
                             Z_DEFAULT_STRATEGY));
}

void try_zip()
{
    unsigned char out[CHUNK];
    z_stream strm;
    strm_init (& strm);
    char* uncompressed_file = calloc((strlen(outPutCsvFile) + strlen("output.csv")+1), sizeof(char));
    strcpy(uncompressed_file, outPutCsvFile);
    strcat(uncompressed_file, "output.csv");
    char* compressed_file = calloc((strlen(outPutCsvFile) + strlen("output_compressed.bin")+1), sizeof(char));
    strcpy(compressed_file, outPutCsvFile);
    strcat(compressed_file, "output_compressed.zip");
    FILE *stream = fopen(uncompressed_file, "r");
    FILE *output = fopen(compressed_file, "w+");

    char line[1024];
    while(fgets(line, 1024, stream)){
        int have;
        strm.next_in = (unsigned char *) line;
        strm.avail_in = strlen (line);
        strm.avail_out = CHUNK;
        strm.next_out = out;
        CALL_ZLIB (deflate (& strm, Z_FINISH));
        have = CHUNK - strm.avail_out;
        fprintf(output, "c", out);
    }
    deflateEnd(&strm);

}