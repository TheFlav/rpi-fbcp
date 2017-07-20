#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <bcm_host.h>

//#define FREEPLAY_USE_MUTEX
#ifdef FREEPLAY_USE_MUTEX
#include <pthread.h>
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

//comment out this next line to remove the Freeplay scaling and rotation functionality
#define FREEPLAY_SCALE_TO_VIEWPORT

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

void copy_screen_scale_to_viewport(DISPMANX_UPDATE_HANDLE_T handle, void* arg) {
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_lock(&g_mtxCritSection);
#endif
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
    
    
    //snapshot the display
    ret = vc_dispmanx_snapshot(display, screen_resource, 0);
    
    //read data (and scale) from snapshot into image_scaled
    vc_dispmanx_resource_read_data(screen_resource, &rect1, image_scaled, FREEPLAY_SCALED_W * vinfo.bits_per_pixel / 8);
    
    
    //no rotation means that the LCD hardware/driver is handling the rotation
    
    //xres is the width of the LCD in landscape mode (320)
    //yres is the height of the LCD in landscape mode (240)
    
    //no rotation, so just push the data to the proper area of the LCD (fbp)
    copy_16bpp_offset((uint16_t *)image_scaled, (uint16_t *)fbp,           FREEPLAY_SCALED_W, FREEPLAY_SCALED_H, FREEPLAY_SCALED_OFFSET_X, FREEPLAY_SCALED_OFFSET_Y, vinfo.xres, vinfo.yres);
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport, NULL);
    
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
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
    
    
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
    
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport_and_rotate, NULL);
    
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_unlock(&g_mtxCritSection);
#endif
    
}

#else

void copy_screen(DISPMANX_UPDATE_HANDLE_T handle, void* arg) {
#ifdef FREEPLAY_USE_MUTEX
    pthread_mutex_lock(&g_mtxCritSection);
#endif
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)NULL, NULL);
    
    
    vc_dispmanx_snapshot(display, screen_resource, 0);
    vc_dispmanx_resource_read_data(screen_resource, &rect1, fbp, vinfo.xres * vinfo.bits_per_pixel / 8);
    
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen, NULL);
    
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
        vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport_and_rotate, NULL);
    else
        vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen_scale_to_viewport, NULL);
    
#else
    
    vc_dispmanx_rect_set(&rect1, 0, 0, vinfo.xres, vinfo.yres);
    
    screen_resource = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo.xres, vinfo.yres, &image_prt);
    if (!screen_resource) {
        syslog(LOG_ERR, "Unable to create screen buffer");
        close(fbfd);
        vc_dispmanx_display_close(display);
        return -1;
    }
    
    // Initialise callback
    vc_dispmanx_vsync_callback(display, (DISPMANX_CALLBACK_FUNC_T)copy_screen, NULL);
#endif
    
    // Do nothing
    pause();
    
    // Cleanup on kill signal
    munmap(fbp, finfo.smem_len);
    close(fbfd);
    vc_dispmanx_resource_delete(screen_resource);
    vc_dispmanx_vsync_callback(display, NULL, NULL);
    vc_dispmanx_display_close(display);
    
    return;
}
