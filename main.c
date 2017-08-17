#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <bcm_host.h>



//If you want this code to be interrrupt driven, uncomment this next line.  Otherwise, you get a while(1) with a usleep inside
//#define FREEPLAY_INTERRUPT_DRIVEN
//interrupt-driven code seems to introduce a bit of video lag

//If you want this code to do the screen scaling for the Freeplay Zero/CM3, uncomment this next line.  Otherwise use overscan in config.txt
#define FREEPLAY_SCALE_TO_VIEWPORT
//scaling code seems to introduce a bit of video lag

//If you want this code to use Mutexes (shouldn't be necessary), uncomment this next line
//#define FREEPLAY_USE_MUTEX

//If you want to include a low-battery indicator, uncomment this next line.
#define FREEPLAY_INCLUDE_BATT_INDICATOR

#if defined(FREEPLAY_INCLUDE_BATT_INDICATOR) && !defined(FREEPLAY_SCALE_TO_VIEWPORT)
#error FREEPLAY_INCLUDE_BATT_INDICATOR requires FREEPLAY_SCALE_TO_VIEWPORT
#endif

#ifndef FREEPLAY_INTERRUPT_DRIVEN
#define FREEPLAY_MS_SLEEP 25
#endif

#ifdef FREEPLAY_USE_MUTEX
#include <pthread.h>
#endif

#ifdef FREEPLAY_INCLUDE_BATT_INDICATOR
#include <wiringPi.h>

// Generated by   : ImageConverter 565 Online
// Generated from : battery_low_32x32_white_bkg.png
// Time generated : Tue, 25 Jul 17
// Image Size     : 32x32 pixels
// Memory usage   : 2048 bytes
const unsigned short battery_low_32x32_white_bkg[1024] ={
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0010 (16) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0020 (32) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0030 (48) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0040 (64) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0050 (80) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0060 (96) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0070 (112) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0080 (128) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD75, 0xAD55, 0xAD54,   // 0x0090 (144) pixels
    0xAD75, 0xAD54, 0xA534, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x00A0 (160) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD54, 0xA534, 0xAD55, 0xAD34, 0xAD75, 0xC638, 0xFFDF, 0xFFFF,   // 0x00B0 (176) pixels
    0xE73C, 0xE73C, 0xCE58, 0xAD34, 0xA534, 0xA514, 0xA534, 0xA534, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x00C0 (192) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xA514, 0xE73C, 0xE75C, 0xE73C, 0xE73C, 0xE73C, 0xEF5D, 0xF7BE,   // 0x00D0 (208) pixels
    0xEF7D, 0xEF7D, 0xF79E, 0xF7BE, 0xFFDF, 0xFFDF, 0xFFDF, 0xA514, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x00E0 (224) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xA514, 0xA534, 0xAD75, 0xAD75, 0xAD55, 0xAD55, 0xAD75, 0xA514,   // 0x00F0 (240) pixels
    0xAD75, 0xA534, 0xA514, 0xAD54, 0xAD54, 0xAD54, 0xAD54, 0xAD34, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0100 (256) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xC638, 0xC638, 0xC638, 0xC638, 0xC638,   // 0x0110 (272) pixels
    0xC638, 0xC638, 0xC638, 0xC638, 0xC638, 0xC638, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0120 (288) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0130 (304) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0140 (320) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0150 (336) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0160 (352) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0170 (368) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0180 (384) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0190 (400) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x01A0 (416) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x01B0 (432) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x01C0 (448) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x01D0 (464) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x01E0 (480) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x01F0 (496) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0200 (512) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0210 (528) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0220 (544) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0230 (560) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0240 (576) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0250 (592) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0260 (608) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0270 (624) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0280 (640) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xAD55, 0xC638, 0xC638, 0xDEDB, 0xCE79, 0xC618, 0xBDF7, 0xBDF7,   // 0x0290 (656) pixels
    0xBDF8, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0xC638, 0xAD55, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x02A0 (672) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8924, 0x8924, 0x8924, 0xE659, 0xD575, 0xC471, 0xB38D, 0xA2AA,   // 0x02B0 (688) pixels
    0x91A6, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x02C0 (704) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8924, 0xB8C3, 0xC124, 0xF659, 0xED96, 0xECF2, 0xEC70, 0xEBED,   // 0x02D0 (720) pixels
    0xE34A, 0xDA87, 0xDA26, 0xD1C6, 0xC965, 0xC124, 0xB8C3, 0x8924, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x02E0 (736) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8924, 0xB8C3, 0xC124, 0xF659, 0xED96, 0xECF2, 0xEC70, 0xEBED,   // 0x02F0 (752) pixels
    0xE34A, 0xDA87, 0xDA26, 0xD1C6, 0xC965, 0xC124, 0xB8C3, 0x8924, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0300 (768) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x8924, 0xB8C3, 0xC124, 0xF659, 0xED96, 0xECF2, 0xEC70, 0xEBED,   // 0x0310 (784) pixels
    0xE34A, 0xDA87, 0xDA26, 0xD1C6, 0xC965, 0xC124, 0xB8C3, 0x8924, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0320 (800) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFDF, 0x8924, 0x8924, 0x8924, 0xE659, 0xD575, 0xC471, 0xB38D, 0xA2AA,   // 0x0330 (816) pixels
    0x91A6, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0x8924, 0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0340 (832) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xF7BE, 0xF79E, 0xEF7D, 0xAD55, 0xAD34, 0xA514, 0xA534, 0xAD75, 0xAD34, 0xA514, 0xAD54,   // 0x0350 (848) pixels
    0xAD34, 0xAD54, 0xAD34, 0xAD75, 0xAD75, 0xAD54, 0xA534, 0xA514, 0xEF7D, 0xF79E, 0xF7BE, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0360 (864) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xF79E, 0xEF5D, 0xE71C, 0xDEFB, 0xAD34, 0xE75C, 0xE75C, 0xEF7D, 0xEF7D, 0xF79D, 0xFFDE, 0xF7BE,   // 0x0370 (880) pixels
    0xD6DA, 0xEF7D, 0xF7BE, 0xFFDF, 0xFFFF, 0xF7BE, 0xFFDF, 0xAD75, 0xDEFB, 0xE71C, 0xEF5D, 0xF79E, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0380 (896) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xF7BE, 0xEF7D, 0xE71C, 0xDEDB, 0xD6BA, 0xAD54, 0xDF1B, 0xDF1B, 0xE71B, 0xE73C, 0xE71B, 0xE73C, 0xE73C,   // 0x0390 (912) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFDF, 0xFFFF, 0xA514, 0xD6BA, 0xDEDB, 0xE71C, 0xEF7D, 0xF7BE, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x03A0 (928) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xF79E, 0xEF5D, 0xE71C, 0xDEFB, 0xAD34, 0xAD34, 0xAD54, 0xA534, 0xAD54, 0xA514, 0xAD55, 0xA534,   // 0x03B0 (944) pixels
    0xAD75, 0xAD54, 0xAD54, 0xA534, 0xA534, 0xAD54, 0xA514, 0xAD54, 0xDEFB, 0xE71C, 0xEF5D, 0xF79E, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x03C0 (960) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xF7BE, 0xF79E, 0xEF7D, 0xEF7D, 0xEF7D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D,   // 0x03D0 (976) pixels
    0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF5D, 0xEF7D, 0xEF7D, 0xEF7D, 0xF79E, 0xF7BE, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x03E0 (992) pixels
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFDF, 0xFFDF, 0xF7BE, 0xF7BE, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E,   // 0x03F0 (1008) pixels
    0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF79E, 0xF7BE, 0xF7BE, 0xFFDF, 0xFFDF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x0400 (1024) pixels
};


unsigned char batt_low = 0;
unsigned int batt_low_counter = 0;
#endif


DISPMANX_DISPLAY_HANDLE_T display;
DISPMANX_MODEINFO_T display_info;
DISPMANX_RESOURCE_HANDLE_T screen_resource;
VC_IMAGE_TRANSFORM_T transform;
uint32_t image_prt;
VC_RECT_T rect1;
int ret;
int fbfd = 0;
char *fbp = 0;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

#ifdef FREEPLAY_USE_MUTEX
pthread_mutex_t g_mtxCritSection;
#endif


#ifdef FREEPLAY_SCALE_TO_VIEWPORT
#define FREEPLAY_SCALED_W  (16*19)  //304 (but seemingly needs to be in multiples of 16)
#define FREEPLAY_SCALED_H 203       //GBA viewport should be 3:2
#define FREEPLAY_SCALED_OFFSET_X 16
#define FREEPLAY_SCALED_OFFSET_Y 8

void *image_scaled = NULL;  //calloc( 1, scaled_w * scaled_h * vinfo.bits_per_pixel / 8 );
void *image_fb_temp = NULL;  //calloc( 1, vinfo.yres * vinfo.xres * vinfo.bits_per_pixel / 8 );

static inline void rotate90_16bpp(uint16_t *src, register uint16_t *dst, int src_width, int src_height)
{
    int i, j;
    
    for(i=0; i<src_width; i++)
    {
        for(j=0; j<src_height; j++)
        {
            *(dst+((src_height*i)+j)) = *(src+((src_height-1-j)*src_width) + i);
        }
    }
}

static inline void copy_16bpp_offset(uint16_t *src, register uint16_t *dst, int src_width, int src_height, int dst_offset_x, int dst_offset_y, int dst_width, int dst_height)
{
    int src_x, src_y;
    uint16_t *dst_ptr;
    
    
    
    for(src_y=0; src_y<src_height; src_y++)
    {
        dst_ptr = dst + (dst_width * dst_offset_y) + (dst_width * src_y) + dst_offset_x;
        
        /*for(src_x=0; src_x<src_width; src_x++)
         {
         *(dst_ptr + src_x) = *(src+(src_y*src_width) + src_x);
         }*/
        memcpy(dst_ptr,src+(src_y*src_width),src_width*2);
    }
}


static inline void copy_16bpp_offset_and_rotate(uint16_t *src, register uint16_t *dst, int src_width, int src_height, int dst_offset_x, int dst_offset_y, int dst_width, int dst_height)
{
    int src_x, src_y;
    uint16_t *dst_ptr;
    
    
    
    for(src_y=0; src_y<src_height; src_y++)
    {
        dst_ptr = dst + (dst_width * dst_offset_y) + (dst_width * src_y) + dst_offset_x;
        
        for(src_x=0; src_x<src_width; src_x++)
        {
            //            *(dst_ptr + src_x) = *(src+(src_y*src_width) + src_x);
            *(dst_ptr+((dst_height*src_y)+src_x)) = *(src+((src_height-1-src_x)*src_width) + src_y);
        }
        //memcpy(dst_ptr,src+(src_y*src_width),src_width*2);
    }
}

#ifdef FREEPLAY_INTERRUPT_DRIVEN
inline
#endif
void copy_screen_scale_to_viewport(DISPMANX_UPDATE_HANDLE_T handle, void* arg) {
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_lock(&g_mtxCritSection);
#endif
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
#endif
    
    //snapshot the display
    ret = vc_dispmanx_snapshot(display, screen_resource, 0);
    
    //read data (and scale) from snapshot into image_scaled
    vc_dispmanx_resource_read_data(screen_resource, &rect1, image_scaled, FREEPLAY_SCALED_W * vinfo.bits_per_pixel / 8);
    
    
    //no rotation means that the LCD hardware/driver is handling the rotation
    
    //xres is the width of the LCD in landscape mode (320)
    //yres is the height of the LCD in landscape mode (240)
    
    //no rotation, so just push the data to the proper area of the LCD (fbp)
    copy_16bpp_offset((uint16_t *)image_scaled, (uint16_t *)fbp,           FREEPLAY_SCALED_W, FREEPLAY_SCALED_H, FREEPLAY_SCALED_OFFSET_X, FREEPLAY_SCALED_OFFSET_Y, vinfo.xres, vinfo.yres);
    
    
#ifdef FREEPLAY_INCLUDE_BATT_INDICATOR
    if(batt_low)
    {
        if(batt_low_counter < 60)
        {
            int y,x;
            
            for(y = 0; y < 32; y++)
            {
                //32x32
                memcpy((uint16_t *)fbp + (320*(y+FREEPLAY_SCALED_OFFSET_Y)+(320-32)), battery_low_32x32_white_bkg + (32*y), 32*2);
            }
        }
        
        batt_low_counter++;
        
        if(batt_low_counter >= 120)
            batt_low_counter = 0;
        
    }
#endif
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport, NULL);
#endif
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_unlock(&g_mtxCritSection);
#endif
}

void copy_screen_scale_to_viewport_and_rotate(DISPMANX_UPDATE_HANDLE_T handle, void* arg)
{
    if(!image_fb_temp)
        return;
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_lock(&g_mtxCritSection);
#endif
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
#endif
    
    //snapshot the display
    ret = vc_dispmanx_snapshot(display, screen_resource, 0);
    
    //read data (and scale) from snapshot into image_scaled
    vc_dispmanx_resource_read_data(screen_resource, &rect1, image_scaled, FREEPLAY_SCALED_W * vinfo.bits_per_pixel / 8);
    
    //this mode is where we are rotating the image here in software
    
    //in rotation mode, xres is the width of the LCD in portrait mode (240)
    //in rotation mode, yres is the height of the LCD in portrait mode (320)
    
    //copy from scaled image to image_fb_temp
    copy_16bpp_offset((uint16_t *)image_scaled, (uint16_t *)image_fb_temp, FREEPLAY_SCALED_W, FREEPLAY_SCALED_H, FREEPLAY_SCALED_OFFSET_X, FREEPLAY_SCALED_OFFSET_Y, vinfo.yres, vinfo.xres);
    
    //rotate image_fb_temp to the LCD (fbp)
    rotate90_16bpp((uint16_t *)image_fb_temp, (uint16_t *)fbp, vinfo.yres, vinfo.xres);
    
    //copy_16bpp_offset_and_rotate((uint16_t *)image_scaled, (uint16_t *)fbp, FREEPLAY_SCALED_W, FREEPLAY_SCALED_H, FREEPLAY_SCALED_OFFSET_X, FREEPLAY_SCALED_OFFSET_Y, vinfo.yres, vinfo.xres);
    
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport_and_rotate, NULL);
#endif
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_unlock(&g_mtxCritSection);
#endif
    
}

#else

#ifdef FREEPLAY_INTERRUPT_DRIVEN
inline
#endif
void copy_screen(DISPMANX_UPDATE_HANDLE_T handle, void* arg) {
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_lock(&g_mtxCritSection);
#endif
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
#endif
    
    vc_dispmanx_snapshot(display, screen_resource, 0);
    vc_dispmanx_resource_read_data(screen_resource, &rect1, fbp, vinfo.xres * vinfo.bits_per_pixel / 8);
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen, NULL);
#endif
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_unlock(&g_mtxCritSection);
#endif
}

#endif



int main(int argc, char **argv) {
    uint8_t portrait_mode_pri = 0;
    uint8_t portrait_mode_sec = 0;
    uint8_t rotate_screen = 0;
    
    bcm_host_init();
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_init(&g_mtxCritSection, NULL);
#endif
    
    display = vc_dispmanx_display_open(0);
    if (!display) {
        syslog(LOG_ERR, "Unable to open primary display");
        return -1;
    }
    ret = vc_dispmanx_display_get_info(display, &display_info);
    if (ret) {
        syslog(LOG_ERR, "Unable to get primary display information");
        return -1;
    }
    syslog(LOG_INFO, "Freeplay-fbcp: Primary display is %d x %d", display_info.width, display_info.height);
    //printf("Freeplay-fbcp: Primary display is %d x %d\n", display_info.width, display_info.height);
    
    
    fbfd = open("/dev/fb1", O_RDWR);
    if (fbfd == -1) {
        syslog(LOG_ERR, "Unable to open secondary display");
        return -1;
    }
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        syslog(LOG_ERR, "Unable to get secondary display information");
        return -1;
    }
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        syslog(LOG_ERR, "Unable to get secondary display information");
        return -1;
    }
    
    syslog(LOG_INFO, "Freeplay-fbcp: Second display is %d x %d %dbps\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    //printf("Freeplay-fbcp: Second display is %d x %d %dbps\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    
    //if vinfo.xres < vinfo.yres, then we are in portrait mode
    if(vinfo.xres < vinfo.yres)
        portrait_mode_sec = 1;
    
    if(display_info.width < display_info.height)
        portrait_mode_pri = 1;
    
    if(portrait_mode_sec != portrait_mode_pri)
    {
        rotate_screen = 1;
        syslog(LOG_INFO, "Set rotate_screen mode on.\n");
        //printf("Set rotate_screen mode on.\n");
    }
    
    
    fbp = (char*) mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp <= 0) {
        syslog(LOG_ERR, "Unable to create memory mapping");
        close(fbfd);
        ret = vc_dispmanx_resource_delete(screen_resource);
        vc_dispmanx_display_close(display);
        return -1;
    }
    
    
#ifdef FREEPLAY_SCALE_TO_VIEWPORT
    image_scaled = calloc( 1, FREEPLAY_SCALED_W * FREEPLAY_SCALED_H * vinfo.bits_per_pixel / 8 );
    if(!image_scaled)
        return -1;
    
    if(rotate_screen)
    {
        image_fb_temp = calloc( 1, vinfo.yres * vinfo.xres * vinfo.bits_per_pixel / 8 );
        if(!image_fb_temp)
            return -1;
    }
    
    
    vc_dispmanx_rect_set(&rect1, 0, 0, FREEPLAY_SCALED_W, FREEPLAY_SCALED_H);
    
    screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, FREEPLAY_SCALED_W, FREEPLAY_SCALED_H, &image_prt);
    if (!screen_resource) {
        syslog(LOG_ERR, "Unable to create screen buffer");
        close(fbfd);
        vc_dispmanx_display_close(display);
        return -1;
    }
    
    // Initialise callback
    if(rotate_screen)
    {
#ifdef FREEPLAY_INTERRUPT_DRIVEN
        vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport_and_rotate, NULL);
#endif
        printf("ROTATION DOES NOT CURRENTLY WORK! (EXITING)\n");
    }
    else
    {
#ifdef FREEPLAY_INTERRUPT_DRIVEN
        vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport, NULL);
#endif
        unsigned int iter=0;
        wiringPiSetupGpio();
        
        pinMode(7, INPUT);//GPIO 7
        while(1)
        {
            copy_screen_scale_to_viewport(NULL,NULL);
            usleep(FREEPLAY_MS_SLEEP * 1000);
            
            
            iter++;
            if(iter >= (5 * 60))//about every 5s
            {
                //test the battery low signal and set batt_low acordingly
                if(!digitalRead(7))
                {
                    batt_low = 1;
                }
                else
                {
                    batt_low = 0;
                }
                
                iter = 0;
            }
        }
    }
    
#else
    
    vc_dispmanx_rect_set(&rect1, 0, 0, vinfo.xres, vinfo.yres);
    
    screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo.xres, vinfo.yres, &image_prt);
    if (!screen_resource) {
        syslog(LOG_ERR, "Unable to create screen buffer");
        close(fbfd);
        vc_dispmanx_display_close(display);
        return -1;
    }
    
#ifdef FREEPLAY_INTERRUPT_DRIVEN
    // Initialise callback
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen, NULL);
#else
    while(1)
    {
        copy_screen(NULL,NULL);
        usleep(FREEPLAY_MS_SLEEP * 1000);
    }
#endif
    
#endif
    
    // Do nothing
    //pause();
    
#ifdef FREEPLAY_INCLUDE_BATT_INDICATOR
    wiringPiSetupGpio();
    
    pinMode(7, INPUT);//GPIO 7
    while(1)
    {
        sleep(5);
        
        //test the battery low signal and set batt_low acordingly
        if(!digitalRead(7))
            batt_low = 1;
        else
            batt_low = 0;
    }
#else
    while(1)
    {
        sleep(60);
    }
#endif
    
    // Cleanup on kill signal
    munmap(fbp, finfo.smem_len);
    close(fbfd);
    vc_dispmanx_resource_delete(screen_resource);
    vc_dispmanx_vsync_callback(display, NULL, NULL);
    vc_dispmanx_display_close(display);
    
    return;
}
