#include <stdlib.h>
#include <stdio.h>

#include "shadowmap.h"

Shadowmap *shadowmap_create(int width, int height) {
    Shadowmap *shadowmap = calloc(1, sizeof(Shadowmap));
    glGenFramebuffers(1, &shadowmap->fbo);
    glGenTextures(1, &shadowmap->shadowmap);
    glBindTexture(GL_TEXTURE_2D, shadowmap->shadowmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowmap->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap->shadowmap, 0);
    // Only used for depth, not for drawing color
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error creating shadowmap: Framebuffer error, status: 0x%x\n", status);
        shadowmap_destroy(shadowmap);
        return NULL;
    }
    return shadowmap;
}

void shadowmap_destroy(Shadowmap *shadowmap) {
    if (shadowmap == NULL) {
        return;
    }
    free(shadowmap);
}

void shadowmap_bind(Shadowmap *shadowmap, GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, shadowmap->shadowmap);
}

void shadowmap_set_as_render_target(Shadowmap *shadowmap) {
     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowmap->fbo);
}
