 /* See LICENSE file for license and copyright information */

#include <librsvg/rsvg.h>

#include "icons.h"

static RsvgHandle* icons[ZATHURA_GTK_ANNOTATION_ICON_N] = {0};

RsvgHandle*
zathura_gtk_annotation_icon_get_handle(zathura_gtk_annotation_icon_t icon)
{
  if (icons[icon] != NULL) {
    return icons[icon];
  }

  char* file_path = NULL;

  /* Select icon file */
  switch (icon) {
    case ZATHURA_GTK_ANNOTATION_ICON_SPEAKER:
      file_path = LIBZATHURA_GTK_ICONDIR "/annotations/Speaker.svg";
      break;
    case ZATHURA_GTK_ANNOTATION_ICON_MIC:
      file_path = LIBZATHURA_GTK_ICONDIR "/annotations/Mic.svg";
      break;
    default:
      return NULL;
  }

  if (file_path == NULL) {
    return NULL;
  }

  /* Open new handle */
  RsvgHandle* svg_handle = rsvg_handle_new_from_file(file_path, NULL);
  icons[icon] = svg_handle;

  return svg_handle;
}

void
zathura_gtk_annotation_icon_cache_free()
{
  for (unsigned int i = 0; i < ZATHURA_GTK_ANNOTATION_ICON_N; i++) {
    if (icons[i] != NULL) {
      rsvg_handle_close(icons[i], NULL);
    }
  }
}
