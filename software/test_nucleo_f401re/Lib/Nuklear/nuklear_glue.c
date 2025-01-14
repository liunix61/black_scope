/*
 * nuklear_glue.c
 *
 *  Created on: Jul 27, 2023
 *      Author: jgpei
 */

#include "nuklear.h"
#include "framebuf.h"
#include "lcd.h"
#include "FontUbuntuBookRNormal16.h"

uint32_t nk_color_to_rgb565( struct nk_color color )
{
	int tmp1, tmp2;
	uint32_t rgb565 = 0;

	color.r = (color.r*color.a)/256.0;
	color.g = (color.g*color.a)/256.0;
	color.b = (color.b*color.a)/256.0;

	color.r >>= 3;
	color.g >>= 2;
	color.b >>= 3;

	rgb565 = (color.r<<11) | (color.g<<5) | (color.b<<0);

	tmp1 = (rgb565>>8)&0xFF;
	tmp2 = (rgb565>>0)&0xFF;
	rgb565 = (tmp2<<8) | (tmp1<<0);

	return rgb565;
}

void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, int16_t x0, int16_t y0 )
{
	const struct nk_command *cmd = NULL;

	for( cmd = nk__begin(ctx); cmd != 0; cmd = nk__next(ctx, cmd) )
	{
		switch (cmd->type) {
			case NK_COMMAND_NOP: {
				break;
			}
			case NK_COMMAND_SCISSOR: {
				const struct nk_command_scissor *s = (const struct nk_command_scissor*) cmd;
				break;
			}
			case NK_COMMAND_LINE: {
				const struct nk_command_line *l = (const struct nk_command_line*) cmd;
				if (l->begin.y == l->end.y) {
					framebuf_hline(pfb, l->begin.x, l->begin.y - y0, l->end.x - l->begin.x, nk_color_to_rgb565(l->color));
				} else if (l->begin.x == l->end.x) {
					framebuf_vline(pfb, l->begin.x, l->begin.y - y0, l->end.y - l->begin.y, nk_color_to_rgb565(l->color));
				} else {
					framebuf_line(pfb, l->begin.x, l->begin.y - y0, l->end.x, l->end.y - y0, nk_color_to_rgb565(l->color));
				}
				break;
			}
			case NK_COMMAND_RECT: {
				struct nk_command_rect *r = (struct nk_command_rect*) cmd;
				framebuf_rect( pfb, r->x, r->y-y0, r->w, r->h, 0, nk_color_to_rgb565( r->color ) );
				break;
			}
			case NK_COMMAND_RECT_FILLED: {
				struct nk_command_rect_filled *r = (struct nk_command_rect_filled*) cmd;
				framebuf_rect( pfb, r->x, r->y-y0, r->w, r->h, 1, nk_color_to_rgb565( r->color ) );
				break;
			}
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle *c = (const struct nk_command_circle*) cmd;
				framebuf_circle( pfb, c->x+c->w/2, c->y-y0+c->w/2, c->w/2, 0, FRAMEBUF_CIRCLE_QALL, nk_color_to_rgb565(c->color) );
				break;
			}
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled*) cmd;
				framebuf_circle( pfb, c->x+c->w/2, c->y-y0+c->w/2, c->w/2, 1, FRAMEBUF_CIRCLE_QALL, nk_color_to_rgb565(c->color) );
				break;
			}
			case NK_COMMAND_TEXT: {
				const struct nk_command_text *t = (const struct nk_command_text*) cmd;
				framebuf_text( pfb, &fontUbuntuBookRNormal16, t->x, t->y - y0, (uint8_t*) t->string, nk_color_to_rgb565(t->foreground) );
				break;
			}
			case NK_COMMAND_IMAGE: {
				const struct nk_command_image *i = (const struct nk_command_image*) cmd;
				framebuf_rect( pfb, i->x, i->y-y0, i->w, i->h, 1, 0xFFFF );
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*) cmd;
				framebuf_line(pfb, t->a.x, t->a.y - y0, t->b.x, t->b.y - y0, nk_color_to_rgb565(t->color));
				framebuf_line(pfb, t->b.x, t->b.y - y0, t->c.x, t->c.y - y0, nk_color_to_rgb565(t->color));
				framebuf_line(pfb, t->c.x, t->c.y - y0, t->a.x, t->a.y - y0, nk_color_to_rgb565(t->color));
				break;
			}
			default: {
				break;
			}
		}
	}
}
