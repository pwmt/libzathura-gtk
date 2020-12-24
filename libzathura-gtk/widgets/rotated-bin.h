 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_rotated_bin
#define ZATHURA_GTK_rotated_bin

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraRotatedBin ZathuraRotatedBin;
typedef struct _ZathuraRotatedBinClass ZathuraRotatedBinClass;

#define ZATHURA_TYPE_ROTATED_BIN            (zathura_gtk_rotated_bin_get_type())
#define ZATHURA_ROTATED_BIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_ROTATED_BIN, ZathuraRotatedBin))
#define ZATHURA_ROTATED_BIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_ROTATED_BIN, ZathuraRotatedBinClass))
#define ZATHURA_IS_ROTATED_BIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_ROTATED_BIN))
#define ZATHURA_IS_ROTATED_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_ROTATED_BIN))
#define ZATHURA_ROTATED_BIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_ROTATED_BIN, ZathuraRotatedBinClass))

typedef struct _ZathuraRotatedBinPrivate ZathuraRotatedBinPrivate;

struct _ZathuraRotatedBin {
  GtkWidget parent;
  ZathuraRotatedBinPrivate* priv;
};

struct _ZathuraRotatedBinClass {
  GtkWidgetClass parent_class;
};

GType zathura_gtk_rotated_bin_get_type(void);

GtkWidget* zathura_gtk_rotated_bin_new(void);
void zathura_gtk_rotated_bin_set_angle(ZathuraRotatedBin* bin, gdouble angle);

G_END_DECLS

#endif /* ZATHURA_GTK_rotated_bin */
