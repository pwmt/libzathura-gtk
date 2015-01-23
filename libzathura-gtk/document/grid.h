 /* See LICENSE file for license and copyright information */

#ifndef ZATHURA_GTK_DOCUMENT_GRID
#define ZATHURA_GTK_DOCUMENT_GRID

#include "../document.h"

void zathura_gtk_setup_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_fill_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_clear_grid(ZathuraDocumentPrivate* priv);
void zathura_gtk_free_grid(ZathuraDocumentPrivate* priv);

#endif /* ZATHURA_GTK_DOCUMENT_GRID */
