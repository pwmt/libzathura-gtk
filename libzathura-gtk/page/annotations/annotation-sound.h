 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_SOUND
#define ZATHURA_GTK_ANNOTATION_SOUND

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"
#include "utils.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationSound ZathuraAnnotationSound;
typedef struct _ZathuraAnnotationSoundClass ZathuraAnnotationSoundClass;

#define ZATHURA_TYPE_ANNOTATION_SOUND            (zathura_gtk_annotation_sound_get_type())
#define ZATHURA_ANNOTATION_SOUND(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_SOUND, ZathuraAnnotationSound))
#define ZATHURA_ANNOTATION_SOUND_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_SOUND, ZathuraAnnotationSoundClass))
#define ZATHURA_IS_ANNOTATION_SOUND(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_SOUND))
#define ZATHURA_IS_ANNOTATION_SOUND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_SOUND))
#define ZATHURA_ANNOTATION_SOUND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_SOUND, ZathuraAnnotationSoundClass))

typedef struct _ZathuraAnnotationSoundPrivate ZathuraAnnotationSoundPrivate;

struct _ZathuraAnnotationSound {
  ZathuraAnnotation parent;
  ZathuraAnnotationSoundPrivate* priv;
};

struct _ZathuraAnnotationSoundClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_sound_get_type(void);

GtkWidget* zathura_gtk_annotation_sound_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_SOUND */
