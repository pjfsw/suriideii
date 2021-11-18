#ifndef _HUD_SHADER_H
#define _HUD_SHADER_H

typedef struct _HudShader HudShader;

HudShader *hud_shader_create();

void hud_shader_destroy(HudShader *shader);

#endif