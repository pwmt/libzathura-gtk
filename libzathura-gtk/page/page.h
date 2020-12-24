 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_PAGE
#define ZATHURA_GTK_PAGE

#include <gtk/gtk.h>
#include <libzathura/libzathura.h>

G_BEGIN_DECLS

typedef struct _ZathuraPage ZathuraPage;
typedef struct _ZathuraPageClass ZathuraPageClass;

#define ZATHURA_TYPE_PAGE            (zathura_gtk_page_get_type())
#define ZATHURA_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), ZATHURA_TYPE_PAGE, ZathuraPage))
#define ZATHURA_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),  ZATHURA_TYPE_PAGE, ZathuraPageClass))
#define ZATHURA_IS_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZATHURA_TYPE_PAGE))
#define ZATHURA_IS_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),  ZATHURA_TYPE_PAGE))
#define ZATHURA_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj),  ZATHURA_TYPE_PAGE, ZathuraPageClass))

typedef struct _ZathuraPagePrivate ZathuraPagePrivate;

struct _ZathuraPage {
  GtkWidget parent;
  ZathuraPagePrivate* priv;
};

struct _ZathuraPageClass {
  GtkWidgetClass parent_class;
};

GType zathura_gtk_page_get_type(void);

GtkWidget* zathura_gtk_page_new(zathura_page_t* page);

G_END_DECLS

#endif /* ZATHURA_GTK_PAGE */
