 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_ICONS
#define ZATHURA_GTK_ANNOTATION_ICONS

#include <librsvg/rsvg.h>

typedef enum zathura_gtk_annotation_icon_e {
  ZATHURA_GTK_ANNOTATION_ICON_SPEAKER = 0,
  ZATHURA_GTK_ANNOTATION_ICON_MIC,
  ZATHURA_GTK_ANNOTATION_ICON_N
} zathura_gtk_annotation_icon_t;

RsvgHandle* zathura_gtk_annotation_icon_get_handle(zathura_gtk_annotation_icon_t icon);

void zathura_gtk_annotation_icon_cache_free();

#endif /* ZATHURA_GTK_ANNOTATION_ICONS */
