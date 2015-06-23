 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_ANNOTATION_MOVIE
#define ZATHURA_GTK_ANNOTATION_MOVIE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

#include "annotation.h"

G_BEGIN_DECLS

typedef struct _ZathuraAnnotationMovie ZathuraAnnotationMovie;
typedef struct _ZathuraAnnotationMovieClass ZathuraAnnotationMovieClass;

#define ZATHURA_TYPE_ANNOTATION_MOVIE            (zathura_gtk_annotation_movie_get_type())
#define ZATHURA_ANNOTATION_MOVIE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ANNOTATION_MOVIE, ZathuraAnnotationMovie))
#define ZATHURA_ANNOTATION_MOVIE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ANNOTATION_MOVIE, ZathuraAnnotationMovieClass))
#define ZATHURA_IS_ANNOTATION_MOVIE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ANNOTATION_MOVIE))
#define ZATHURA_IS_ANNOTATION_MOVIE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ANNOTATION_MOVIE))
#define ZATHURA_ANNOTATION_MOVIE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ANNOTATION_MOVIE, ZathuraAnnotationMovieClass))

typedef struct _ZathuraAnnotationMoviePrivate ZathuraAnnotationMoviePrivate;

struct _ZathuraAnnotationMovie {
  ZathuraAnnotation parent;
  ZathuraAnnotationMoviePrivate* priv;
};

struct _ZathuraAnnotationMovieClass {
  ZathuraAnnotationClass parent_class;
};


GType zathura_gtk_annotation_movie_get_type(void);

GtkWidget* zathura_gtk_annotation_movie_new(zathura_annotation_t* annotation);

G_END_DECLS

#endif /* ZATHURA_GTK_ANNOTATION_MOVIE */
