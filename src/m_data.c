#include <stdlib.h>
#include <string.h>

#include <tea.h>

#include "miniz.h"

#include "m_data.h"
#include "fs.h"

#define CLASS_NAME  DATA_CLASS_NAME

static void data_free(void* ud)
{
    Data* self = (Data*)ud;
    free(self->data);
}

static Data* new_data(tea_State* T)
{
    Data* self = (Data*)tea_new_udata(T, sizeof(*self), CLASS_NAME);
    tea_set_finalizer(T, data_free);
    memset(self, 0, sizeof(*self));
    return self;
}

static void data_fromFile(tea_State* T)
{
    const char* filename = tea_check_string(T, 0);
    Data* self = new_data(T);
    size_t len;
    void* data = fs_read(filename, &len);
    if(!data)
    {
        tea_error(T, "could not open file '%s'", filename);
    }
    self->data = data;
    self->len = len;
}

static void data_fromString(tea_State* T)
{
    size_t len;
    const char* data = tea_check_lstring(T, 0, &len);
    Data* self = new_data(T);
    self->data = malloc(len);
    if(!self->data)
    {
        tea_error(T, "out of memory");
    }
    memcpy(self->data, data, len);
    self->len = len;
}

static void data_getLength(tea_State* T)
{
    Data* self = (Data*)tea_check_udata(T, 0, CLASS_NAME);
    tea_push_integer(T, self->len);
}

static void data_tostring(tea_State* T)
{
    Data* self = (Data*)tea_check_udata(T, 0, CLASS_NAME);
    tea_push_lstring(T, self->data, self->len);
}

static const tea_Methods reg[] = {
    { "fromFile", "static", data_fromFile, 1, 0 },
    { "fromString", "static", data_fromString, 1, 0 },
    { "getLength", "method", data_getLength, 1, 0 },
    { "tostring", "method", data_tostring, 1, 0 },
    { NULL, NULL }
};

void micro_open_data(tea_State* T)
{
    tea_create_class(T, "Data", reg);
    tea_push_value(T, -1);
    tea_set_key(T, TEA_REGISTRY_INDEX, CLASS_NAME);
}

static void data_compress(tea_State *T)
{
    size_t source_len;
    const unsigned char* source = (const unsigned char*)tea_check_lstring(T, 0, &source_len);
    
    // Get initial compression buffer size estimate
    // According to miniz docs, worst case compression can expand data by ~1.1x
    mz_ulong compressed_len = (mz_ulong)(source_len * 1.1) + 12;
    unsigned char* compressed = NULL;
    int status;
    
    compressed = (unsigned char*)malloc(compressed_len);
    if(!compressed)
    {
        tea_error(T, "out of memory");
    }
    
    // Attempt compression
    status = mz_compress(compressed, &compressed_len, source, source_len);
    
    if(status != MZ_OK)
    {
        free(compressed);
        tea_error(T, mz_error(status));
    }
    
    tea_push_lstring(T, (const char*)compressed, compressed_len);
    free(compressed);
}

static void data_decompress(tea_State* T)
{
    size_t compressed_len;
    const unsigned char* compressed = (const unsigned char*)tea_check_lstring(T, 0, &compressed_len);
    
    // Get the decompressed size
    mz_ulong decompressed_len = compressed_len * 2;  // Initial estimate
    unsigned char* decompressed = NULL;
    int status;
    
    do
    {
        // Allocate or reallocate buffer
        unsigned char* new_buffer = (unsigned char*)realloc(decompressed, decompressed_len);
        if(!new_buffer)
        {
            free(decompressed);
            tea_error(T, "out of memory");
        }
        decompressed = new_buffer;
        
        // Try to decompress
        status = mz_uncompress(decompressed, &decompressed_len, compressed, compressed_len);
        
        if(status == MZ_BUF_ERROR)
        {
            // Buffer too small, double it and try again
            decompressed_len *= 2;
        }
    }
    while(status == MZ_BUF_ERROR);
    
    if(status != MZ_OK)
    {
        free(decompressed);
        tea_error(T, mz_error(status));
    }
    
    tea_push_lstring(T, (const char*)decompressed, decompressed_len);
    free(decompressed);
}

static const tea_Reg mod[] = {
    { "compress", data_compress, 1, 0 },
    { "decompress", data_decompress, 1, 0 },
    { NULL, NULL }
};

void micro_open_data_(tea_State* T)
{
    tea_create_submodule(T, "data", mod);
}