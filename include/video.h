#ifndef VIDEO_H
#define VIDEO_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

// Context structure for video capture
typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *codec_ctx;
    int video_stream_idx;
    struct SwsContext *sws_ctx;
    AVFrame *frame;
    AVPacket *packet;
} VideoCapture;

/**
 * @brief Initialize video capture from a device (e.g., /dev/video0)
 * @param dev_path Path to the video device
 * @param width Target frame width
 * @param height Target frame height
 * @return Pointer to VideoCapture context or NULL on failure
 */
VideoCapture* init_video_capture(const char *dev_path, int width, int height);

/**
 * @brief Read a frame into a grayscale buffer
 * @param ctx VideoCapture context
 * @param buffer Output buffer for grayscale pixels (size: width * height)
 * @return 0 on success, -1 on error
 */
int read_frame(VideoCapture *ctx, uint8_t *buffer);

/**
 * @brief Free video capture resources
 * @param ctx VideoCapture context
 */
void free_video_capture(VideoCapture *ctx);

#endif