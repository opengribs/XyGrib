#ifndef USE_OPENJPEG
 void dummy_openjpeg(void) {}
#else   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "grib2.h"
#include "openjpeg.h"

#define GRIB_SUCCESS          0
#define GRIB_DECODING_ERROR  (-3)
#define GRIB_ENCODING_ERROR  (-14) /* match openjpeg in grib_api */

/* opj_* Helper code from https://groups.google.com/forum/#!topic/openjpeg/8cebr0u7JgY */
typedef struct
{
  OPJ_UINT8* pData;
  OPJ_SIZE_T dataSize;
  OPJ_SIZE_T offset;
  // j2k_encode_helper *helper;
} opj_memory_stream;


#if 0
// debug output
static void openjpeg_warning(const char *msg, void *client_data)
{
    fprintf(stderr,"OPENJPEG: %s",msg);
}

static void openjpeg_info(const char *msg, void *client_data)
{
  fprintf(stderr,"OPENJPEG INFO: %s",msg);
}
#endif

static void openjpeg_error(const char *msg, void *client_data)
{
    fprintf(stderr,"OPENJPEG ERROR: %s",msg);
}


static OPJ_SIZE_T opj_memory_stream_read(void *buffer, OPJ_SIZE_T nb_bytes, void * p_user_data)
{
  opj_memory_stream* mstream = (opj_memory_stream*) p_user_data; //Our data.
  OPJ_SIZE_T nb_bytes_read = nb_bytes;

  //Check if the current offset is outside our data buffer.
  if (mstream->offset >= mstream->dataSize)
    return (OPJ_SIZE_T) -1;

  //Check if we are reading more than we have.
  if (nb_bytes > (mstream->dataSize - mstream->offset))
    nb_bytes_read = mstream->dataSize - mstream->offset;

  memcpy(buffer, &(mstream->pData[mstream->offset]), nb_bytes_read);
  mstream->offset += nb_bytes_read;
  return nb_bytes_read;
}

static OPJ_SIZE_T opj_memory_stream_write(void *buffer, OPJ_SIZE_T nb_bytes, void *user_data)
{
  opj_memory_stream* mstream = (opj_memory_stream*) user_data; // our data
  OPJ_SIZE_T nb_bytes_write = nb_bytes;

  if (mstream->offset >= mstream->dataSize)
    return (OPJ_SIZE_T)-1;
  if (nb_bytes > (mstream->dataSize - mstream->offset))
    nb_bytes_write = mstream->dataSize - mstream->offset;

  memcpy(&(mstream->pData[mstream->offset]), buffer, nb_bytes_write);
  mstream->offset += nb_bytes_write;
  return nb_bytes_write;
}

static OPJ_OFF_T opj_memory_stream_skip(OPJ_OFF_T nb_bytes, void *user_data)
{
  opj_memory_stream* mstream = (opj_memory_stream*) user_data;
  OPJ_SIZE_T l_nb_bytes;

  if (nb_bytes < 0)
    return -1;
  l_nb_bytes = (OPJ_SIZE_T) nb_bytes;
  if (l_nb_bytes > mstream->dataSize - mstream->offset)
    nb_bytes = mstream->dataSize - mstream->offset;
  mstream->offset += l_nb_bytes;
  return l_nb_bytes;
}

static OPJ_BOOL opj_memory_stream_seek(OPJ_OFF_T nb_bytes, void * user_data)
{
  opj_memory_stream* mstream = (opj_memory_stream*) user_data;

  if (nb_bytes < 0)
    return OPJ_FALSE;
  if (nb_bytes >(OPJ_OFF_T) mstream->dataSize)
    return OPJ_FALSE;
  mstream->offset = (OPJ_SIZE_T) nb_bytes;
  return OPJ_TRUE;
}

static void opj_memory_stream_do_nothing(void * p_user_data)
{
  OPJ_ARG_NOT_USED(p_user_data);
}

// Create a stream to use memory as the input or output
static opj_stream_t* opj_stream_create_default_memory_stream(opj_memory_stream* memoryStream, OPJ_BOOL is_read_stream)
{
  opj_stream_t* stream;

  if (!(stream = opj_stream_default_create(is_read_stream)))
    return (NULL);
  if (is_read_stream)
    opj_stream_set_read_function(stream, opj_memory_stream_read);
  else
    opj_stream_set_write_function(stream, opj_memory_stream_write);

  opj_stream_set_seek_function(stream, opj_memory_stream_seek);
  opj_stream_set_skip_function(stream, opj_memory_stream_skip);
  opj_stream_set_user_data(stream, memoryStream, opj_memory_stream_do_nothing);
  opj_stream_set_user_data_length(stream, memoryStream->dataSize);
  return stream;
}

int dec_jpeg2000(char *injpc,g2int bufsize,g2int *outfld)
/*$$$  SUBPROGRAM DOCUMENTATION BLOCK
*                .      .    .                                       .
* SUBPROGRAM:    dec_jpeg2000      Decodes JPEG2000 code stream
*   PRGMMR: McKinstry              DATE: 2016-08-29
*
* ABSTRACT: This Function decodes a JPEG2000 code stream specified in the
*   JPEG2000 Part-1 standard (i.e., ISO/IEC 15444-1) using Openpeg
*   Software version 2.1
*   Openjpeg is available at http://www.openjpeg.org/
*
* PROGRAM HISTORY LOG:
* 2016-08-29 Alastair McKinstry
*
* USAGE:     int dec_jpeg2000(char *injpc,g2int bufsize,g2int *outfld)
*
*   INPUT ARGUMENTS:
*      injpc - Input JPEG2000 code stream.
*    bufsize - Length (in bytes) of the input JPEG2000 code stream.
*
*   OUTPUT ARGUMENTS:
*     outfld - Output matrix of grayscale image values.
*
*   RETURN VALUES :
*          0 = Successful decode
*         -3 = Error decode jpeg2000 code stream.
*         -5 = decoded image had multiple color components.
*              Only grayscale is expected.
*
* REMARKS:
*
* Requires OpenJPEG software, 2.1 or later.
*
* ATTRIBUTES:
*   LANGUAGE: C
*
*$$$*/

{
    int err = GRIB_SUCCESS;
    g2int i;
    unsigned long mask;
    size_t n_vals, count;

    opj_dparameters_t parameters = {0,};    /* decompression parameters */
    opj_codec_t *codec = NULL;
    opj_image_t *image = NULL;
    opj_stream_t *stream = NULL;
    opj_image_comp_t comp = {0,};
    opj_image_cmptparm_t cmptparm = {0,};
    opj_memory_stream mstream;
    
    /* set decoding parameters to default values */
    opj_set_default_decoder_parameters(&parameters);
    parameters.decod_format = 1; /* J */

    /* get a decoder handle */
    codec = opj_create_decompress(OPJ_CODEC_J2K);

#if 0
    opj_set_info_handler(codec, openjpeg_info, NULL);
    opj_set_warning_handler(codec, openjpeg_warning, NULL);
#endif
    opj_set_info_handler(codec, NULL, NULL);
    opj_set_warning_handler(codec, NULL, NULL);
    opj_set_error_handler(codec, openjpeg_error,NULL);

    /* initialize our memory stream */
    mstream.pData = injpc;
    mstream.dataSize = bufsize;
    mstream.offset = 0;
    /* open a byte stream from memory stream */
    stream = opj_stream_create_default_memory_stream( &mstream, OPJ_STREAM_READ);
    
    /* setup the decoder decoding parameters using user parameters */
    if (!opj_setup_decoder(codec, &parameters)) { 
	err = GRIB_DECODING_ERROR;
	goto cleanup;
    }

    if  (!opj_read_header(stream, codec, &image)) {
      err = GRIB_DECODING_ERROR;
      goto cleanup;
    }
    if (!opj_decode(codec, stream, image)) {
      err = GRIB_DECODING_ERROR;
      goto cleanup;
    }

    // sanity checks
    //if ( !(*n_vals <= image->comps[0].w * image->comps[0].h) ) {
    //  err = GRIB_DECODING_ERROR;
    //  goto cleanup;
    //}
    if ( (image->numcomps != 1) || !(image->x1 * image->y1) ) {
      err = GRIB_DECODING_ERROR;
      goto cleanup;
    }    
    assert(image->comps[0].sgnd == 0);
    assert(comp.prec <= sizeof(image->comps[0].data[0])*8 - 1); /* BR: -1 because I don't know what happens if the sign bit is set */
    
    assert(image->comps[0].prec < sizeof(mask)*8-1);
    
    mask = (1 << image->comps[0].prec) - 1;
 
    count = image->comps[0].w * image->comps[0].h;
    for(i = 0; i <count ; i++)
      outfld[i] = image->comps[0].data[i] & mask;
    
 cleanup:
    /* close the byte stream */
    if( codec && stream ) opj_end_decompress(codec,stream);
    if (codec)   opj_destroy_codec(codec);
    if (stream) opj_stream_destroy(stream);
    if (image) opj_image_destroy(image);
    
    return err;


}

#endif   /* USE_OPENJPEG */
