 /* See LICENSE file for license and copyright information */

#include <librsvg/rsvg.h>
#include <string.h>

#include "icons.h"

#define LENGTH(x) (sizeof(x)/sizeof((x)[0]))

static RsvgHandle* icons[ZATHURA_GTK_ANNOTATION_ICON_N] = {0};

RsvgHandle*
zathura_gtk_annotation_icon_get_handle(zathura_gtk_annotation_icon_t icon)
{
  if (icons[icon] != NULL) {
    return icons[icon];
  }

  typedef struct icon_mapping_s {
    zathura_gtk_annotation_icon_t icon;
    const char* path;
  } icon_mapping_t;

  icon_mapping_t icon_mappings[] = {
    { ZATHURA_GTK_ANNOTATION_ICON_SPEAKER,    LIBZATHURA_GTK_ICONDIR "/annotations/Speaker.svg" },
    { ZATHURA_GTK_ANNOTATION_ICON_MIC,        LIBZATHURA_GTK_ICONDIR "/annotations/Mic.svg" },
    { ZATHURA_GTK_ANNOTATION_ICON_GRAPH,      LIBZATHURA_GTK_ICONDIR "/annotations/Graph.svg" },
    { ZATHURA_GTK_ANNOTATION_ICON_PAPER_CLIP, LIBZATHURA_GTK_ICONDIR "/annotations/PaperClip.svg" },
    { ZATHURA_GTK_ANNOTATION_ICON_TAG,        LIBZATHURA_GTK_ICONDIR "/annotations/Tag.svg" },
    { ZATHURA_GTK_ANNOTATION_ICON_PUSH_PIN,   LIBZATHURA_GTK_ICONDIR "/annotations/PushPin.svg" }
  };

  /* Select icon file */
  const char* file_path = NULL;
  for (unsigned int i = 0; i < LENGTH(icon_mappings); i++) {
    if (icon_mappings[i].icon == icon) {
      file_path = icon_mappings[i].path;
      break;
    }
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
      g_object_unref(icons[i]);
    }
  }
}
