// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
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

    // Check Extra Extensions: Uniforms
    if (GLAD_GL_ARB_uniform_buffer_object)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_UNIFORM_BUFFER_OBJECT);
    if (GLAD_GL_ARB_shader_storage_buffer_object)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_UNIFORM_SHADER_STORAGE);
    if (GLAD_GL_ARB_shader_atomic_counters)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_UNIFORM_ATOMIC_COUNTER);
    // Check Extra Extensions: SPIR-V
    if (GLAD_GL_ARB_gl_spirv)
        features |= driver_feature_flag(GPUDriverFeature::DRIVER_SHADER_SPIRV);

    // Return Features
    return features;
}

// --------------------
// OpenGL Debug Context
// --------------------

static void GLAD_API_PTR nogpu_debugCallback(
    GLenum source, 
    GLenum type, 
    unsigned int id, 
    GLenum severity, 
    GLsizei length, 
    const char *message, 
    const void *user)
{
    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            GPUReport::error("0x%x: %s", id, message);
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
            GPUReport::warning("0x%x: %s", id, message);
            break;

        case GL_DEBUG_TYPE_OTHER_ARB:
            GPUReport::info("0x%x: %s", id, message);
            break;

        // Not Relevant Messages
        default: break;
    }
}

void GLDevice::prepareDebugContext(GPUDriverMode mode) {
    switch (mode) {
        case GPUDriverMode::DRIVER_MODE_REPORT:
        case GPUDriverMode::DRIVER_MODE_LOGGER:
        case GPUDriverMode::DRIVER_MODE_DEBUG:
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); 
            glDebugMessageCallbackARB(nogpu_debugCallback, nullptr);
            glDebugMessageControlARB( // Enable All Messages
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                0, nullptr, GL_TRUE);
        
        // Debug not Enabled
        default: break;
    }
}

// --------------------
// OpenGL Texture Stole
// --------------------

void GLDevice::prepareStoleTexture() {
    GLint max_texture_units = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        &max_texture_units);

    // Prepare Stole Texture Unit
    m_stole = GL_TEXTURE0;
    if (max_texture_units > 0)
        m_stole += max_texture_units - 1;
}
