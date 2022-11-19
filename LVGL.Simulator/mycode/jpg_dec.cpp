#include "jpg_dec.h"
#include <string.h>
#include <stdlib.h>
#include "LVGL.Simulator/mycode/tjpg/tjpgd.h"

typedef struct {
    lv_fs_file_t* fp;               /* Input stream */
    uint8_t* fbuf;          /* Output frame buffer */
    unsigned int wfbuf;     /* Width of the frame buffer [pix] */
} IODEV;

size_t in_func(    /* Returns number of bytes read (zero on error) */
    JDEC* jd,       /* Decompression object */
    uint8_t* buff,  /* Pointer to the read buffer (null to remove data) */
    size_t nbyte    /* Number of bytes to read/remove */
)
{
    IODEV* dev = (IODEV*)jd->device;   /* Session identifier (5th argument of jd_prepare function) */


    if (buff) { /* Raad data from imput stream */
        return lv_fs_read(dev->fp, buff, nbyte, 0);
    }
    else {    /* Remove data from input stream */
        return (lv_fs_seek(dev->fp, nbyte, LV_FS_SEEK_CUR) == LV_FS_RES_OK) ? nbyte : 0;
    }
}

void jpg_display()
{
    lv_fs_file_t fp;
    JRESULT res;      /* Result code of TJpgDec API */
    JDEC jdec;        /* Decompression object */
    void* work;       /* Pointer to the work area */
    size_t sz_work = 3500; /* Size of work area */
    IODEV devid;      /* Session identifier */


    /* Initialize input stream */
    //if (argc < 2) return -1;
    //devid.fp = fopen(argv[1], "rb");
    lv_fs_res_t t = lv_fs_open(&fp, "C:\Users\pzh\Desktop\1.jpg", LV_FS_MODE_RD);
    if (t != LV_FS_RES_OK) return;

    /* Prepare to decompress */
    work = (void*)malloc(sz_work);
    res = jd_prepare(&jdec, in_func, work, sz_work, &devid);
    if (res == JDR_OK) {
        /* It is ready to dcompress and image info is available here */
        //printf("Image size is %u x %u.\n%u bytes of work ares is used.\n", jdec.width, jdec.height, sz_work - jdec.sz_pool);

        /* Initialize output device */
        devid.fbuf = (uint8_t*)malloc(N_BPP * jdec.width * jdec.height); /* Create frame buffer for output image */
        devid.wfbuf = jdec.width;

        res = jd_decomp(&jdec, out_func, 0);   /* Start to decompress with 1/1 scaling */
        if (res == JDR_OK) {
            /* Decompression succeeded. You have the decompressed image in the frame buffer here. */
            printf("\rDecompression succeeded.\n");

        }
        else {
            printf("jd_decomp() failed (rc=%d)\n", res);
        }

        free(devid.fbuf);    /* Discard frame buffer */

    }
    else {
        printf("jd_prepare() failed (rc=%d)\n", res);
    }

    free(work);             /* Discard work area */

    fclose(devid.fp);       /* Close the JPEG file */

    return res;
}
