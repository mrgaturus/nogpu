#include <nogpu_private.h>
#include "private/driver.h"
#include "private/glad.h"

unsigned int GLDriver::initializeGL(void* getProcAddress) {
    if (gladLoadGL((GLADloadfunc) eglGetProcAddress) == 0) {
        GPUReport::error("[opengl] failed loading OpenGL functions");
        return 0;
    } else if (GLAD_GL_VERSION_3_3 == 0 && !GLAD_GL_ARB_texture_storage) {
        GPUReport::error("[opengl] device doesn't support OpenGL 3.3");
        return 0;
    }

    unsigned int features = // Activate Fundamental Features
        driver_feature_flag(GPUDriverFeature::DRIVER_FEATURE_RASTERIZE) |
        driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_1D) |
        driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_RGBA16) |
        driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_COMPRESSED_RGTC) |
        driver_feature_flag(GPUDriverFeature::DRIVER_SHADER_GLSL);

    // Check Extra Extensions
    if (GLAD_GL_ARB_compute_shader && GLAD_GL_ARB_shader_image_load_store)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_FEATURE_COMPUTE);
    if (GLAD_GL_ARB_debug_output) // Debug Feature
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_FEATURE_DEBUG);
    if (GLAD_GL_ARB_texture_buffer_range) // Texture Buffer Feature
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_BUFFER);
    if (GLAD_GL_ARB_texture_cube_map_array) // Texture Cubemap Array Feature
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_CUBEMAP_ARRAY);
    if (GLAD_GL_ARB_texture_storage_multisample) // Texture Multisample Feature
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_MULTISAMPLE);

    // Check Extra Extensions: Compressed Textures
    if (GLAD_GL_EXT_texture_compression_s3tc)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_COMPRESSED_DXTC);
    if (GLAD_GL_ARB_texture_compression_bptc)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_COMPRESSED_BC7);
    if (GLAD_GL_ARB_ES3_compatibility)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_COMPRESSED_ETC2);
    if (GLAD_GL_KHR_texture_compression_astc_ldr || GLAD_GL_KHR_texture_compression_astc_hdr)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_TEXTURE_COMPRESSED_ASTC);

    // Check Extra Extensions: SPIR-V
    if (GLAD_GL_ARB_gl_spirv)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_SHADER_SPIRV);

    // Return Features
    return features;
}
