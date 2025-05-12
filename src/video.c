#include "video.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VideoCapture* init_video_capture(const char *dev_path, int width, int height) {
    VideoCapture *ctx = calloc(1, sizeof(VideoCapture));
    if (!ctx) {
        perror("Failed to allocate VideoCapture context");
        return NULL;
    }

    // Register FFmpeg devices
    avformat_network_init();
    ctx->fmt_ctx = avformat_alloc_context();
    if (!ctx->fmt_ctx) goto fail;

    // Open video device
    if (avformat_open_input(&ctx->fmt_ctx, dev_path, NULL, NULL) < 0) {
        fprintf(stderr, "Failed to open video device: %s\n", dev_path);
        goto fail;
    }

    // Find stream info
    if (avformat_find_stream_info(ctx->fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Failed to get stream info\n");
        goto fail;
    }

    // Find video stream
    for (int i = 0; i < (int)ctx->fmt_ctx->nb_streams; i++) {
        if (ctx->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            ctx->video_stream_idx = i;
            break;
        }
    }
    if (ctx->video_stream_idx == -1) {
        fprintf(stderr, "No video stream found\n");
        goto fail;
    }

    // Get codec
    const AVCodec *codec = avcodec_find_decoder(ctx->fmt_ctx->streams[ctx->video_stream_idx]->codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "Unsupported codec\n");
        goto fail;
    }

    ctx->codec_ctx = avcodec_alloc_context3(codec);
    if (!ctx->codec_ctx) goto fail;

    if (avcodec_parameters_to_context(ctx->codec_ctx, ctx->fmt_ctx->streams[ctx->video_stream_idx]->codecpar) < 0) {
        fprintf(stderr, "Failed to copy codec parameters\n");
        goto fail;
    }

    ctx->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // Intermediate format
    if (avcodec_open2(ctx->codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Failed to open codec\n");
        goto fail;
    }

    // Frame and packet
    ctx->frame = av_frame_alloc();
    ctx->packet = av_packet_alloc();
    if (!ctx->frame || !ctx->packet) goto fail;

    // Scaling context for resizing to target resolution
    ctx->sws_ctx = sws_getContext(
        ctx->codec_ctx->width, ctx->codec_ctx->height, ctx->codec_ctx->pix_fmt,
        width, height, AV_PIX_FMT_GRAY8, // Output grayscale
        SWS_BILINEAR, NULL, NULL, NULL
    );
    if (!ctx->sws_ctx) {
        fprintf(stderr, "Failed to create scaling context\n");
        goto fail;
    }

    return ctx;

fail:
    free_video_capture(ctx);
    return NULL;
}

int read_frame(VideoCapture *ctx, uint8_t *buffer) {
    while (av_read_frame(ctx->fmt_ctx, ctx->packet) >= 0) {
        if (ctx->packet->stream_index != ctx->video_stream_idx)
            continue;

        if (avcodec_send_packet(ctx->codec_ctx, ctx->packet) >= 0) {
            fprintf(stderr, "Failed to send packet to decoder\n");
            return -1;
        }

        while (avcodec_receive_frame(ctx->codec_ctx, ctx->frame) >= 0) {
            // Scale frame to grayscale and resize
            uint8_t *dst_data[4] = {buffer, NULL, NULL, NULL};
            int dst_linesize[4] = {ctx->codec_ctx->width, 0, 0, 0};
            sws_scale(ctx->sws_ctx, (const uint8_t *const *)ctx->frame->data, ctx->frame->linesize,
                      0, ctx->codec_ctx->height, dst_data, dst_linesize);
            return 0;
        }
    }
    return -1;
}

void free_video_capture(VideoCapture *ctx) {
    if (!ctx) return;
    if (ctx->sws_ctx) sws_freeContext(ctx->sws_ctx);
    if (ctx->codec_ctx) avcodec_free_context(&ctx->codec_ctx);
    if (ctx->fmt_ctx) avformat_close_input(&ctx->fmt_ctx);
    if (ctx->frame) av_frame_free(&ctx->frame);
    if (ctx->packet) av_packet_free(&ctx->packet);
    free(ctx);
}