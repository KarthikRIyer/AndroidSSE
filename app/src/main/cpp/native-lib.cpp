#include <jni.h>
#include <arm_neon.h>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>

unsigned int width = 1000;
unsigned int height = 1000;

void createImage(){
    uint8_t *image = new uint8_t[3*width*height];
    memset(image, 0, 3*width*height*sizeof(uint8_t));
    delete[] image;
    std::cout<<"Here!";
}

void rgb_deinterleave_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color){
    for (int i = 0; i < len_color; ++i) {
        r[i] = rgb[3*i];
        g[i] = rgb[3*i + 1];
        b[i] = rgb[3*i + 1];
    }
}

void rgb_deinterleave_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int len_color){
    int num8x16 = len_color/16;
    uint8x16x3_t intlv_rgb;
    for (int i = 0; i < num8x16; ++i) {
        intlv_rgb = vld3q_u8(rgb+3*16*i);
        vst1q_u8(r+16*i, intlv_rgb.val[0]);
        vst1q_u8(g+16*i, intlv_rgb.val[1]);
        vst1q_u8(b+16*i, intlv_rgb.val[2]);
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndkexample_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    createImage();
    uint8_t *image = new uint8_t[3*width*height];
    memset(image, 255, width*height*sizeof(uint8_t));

    uint8_t *red = new uint8_t[width*height];
    uint8_t *blue = new uint8_t[width*height];
    uint8_t *green = new uint8_t[width*height];
    memset(red, 0, width*height*sizeof(uint8_t));
    memset(blue, 0, width*height*sizeof(uint8_t));
    memset(green, 0, width*height*sizeof(uint8_t));

    //simple deinterleave
    auto start = std::chrono::system_clock::now();

    rgb_deinterleave_c(red, green, blue, image, width*height);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::string elapsed_time_c = std::to_string(elapsed_seconds.count()*1000);

    memset(red, 0, width*height*sizeof(uint8_t));
    memset(blue, 0, width*height*sizeof(uint8_t));
    memset(green, 0, width*height*sizeof(uint8_t));

    //neon deinterleave
    start = std::chrono::system_clock::now();

    rgb_deinterleave_neon(red, green, blue, image, width*height);

    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::string elapsed_time_neon = std::to_string(elapsed_seconds.count()*1000);
    delete[] image;
    delete[] red;
    delete[] green;
    delete[] blue;

    std::string finalResult = "C deinterleave time = " + elapsed_time_c + " milliseconds\n" + "Neon deinterleave time = " + elapsed_time_neon + " milliseconds";

    return env->NewStringUTF(finalResult.c_str());
}
