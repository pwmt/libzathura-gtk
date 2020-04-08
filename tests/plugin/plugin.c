/* See LICENSE file for license and copyright information */

#if HAVE_CAIRO
#include <cairo.h>
#endif

#include <libzathura/plugin-api.h>
#include <libzathura/macros.h>

#include <stdio.h>

/* forward declarations */
void register_functions(zathura_plugin_functions_t* functions);
zathura_error_t document_open(zathura_document_t* document);
zathura_error_t document_free(zathura_document_t* document);
zathura_error_t document_save_as(zathura_document_t* document, const char* path);
zathura_error_t document_get_outline(zathura_document_t* document, zathura_node_t** outline);
zathura_error_t document_get_attachments(zathura_document_t* document, zathura_list_t** attachments);
zathura_error_t document_get_metadata(zathura_document_t* document, zathura_list_t** metadata);
zathura_error_t page_init(zathura_page_t* page);
zathura_error_t page_clear(zathura_page_t* page);
zathura_error_t page_search_text(zathura_page_t* page, const char* text, zathura_search_flag_t flags, zathura_list_t** results);
zathura_error_t page_get_text(zathura_page_t* page, char** text);
zathura_error_t page_get_selected_text(zathura_page_t* page, char** text, zathura_rectangle_t rectangle);
zathura_error_t page_get_links(zathura_page_t* page, zathura_list_t** links);
zathura_error_t page_get_form_fields(zathura_page_t* page, zathura_list_t** form_fields);
zathura_error_t page_get_images(zathura_page_t* page, zathura_list_t** images);
zathura_error_t page_get_annotations(zathura_page_t* page, zathura_list_t** annotations);
zathura_error_t page_render(zathura_page_t* page, zathura_image_buffer_t** buffer, double scale, int rotation, int flags);
#ifdef HAVE_CAIRO
zathura_error_t page_render_cairo(zathura_page_t* page, cairo_t* cairo, double scale, int rotation, int flags);
#endif

/* register plugin */
ZATHURA_PLUGIN_REGISTER(
  "plugin",
  0,
  0,
  0,
  register_functions,
  ZATHURA_PLUGIN_MIMETYPES({
    "libzathura-gtk/test-plugin",
  })
)

/* functions implementation */
void
register_functions(zathura_plugin_functions_t* functions)
{
  functions->document_open = document_open;
  functions->document_free = document_free;
  functions->document_save_as = document_save_as;
  functions->document_get_outline = document_get_outline;
  functions->document_get_attachments = document_get_attachments;
  functions->document_get_metadata = document_get_metadata;

  functions->page_init = page_init;
  functions->page_clear = page_clear;
  functions->page_search_text = page_search_text;
  functions->page_get_text = page_get_text;
  functions->page_get_selected_text = page_get_selected_text;
  functions->page_get_links = page_get_links;
  functions->page_get_form_fields = page_get_form_fields;
  functions->page_get_images = page_get_images;
  functions->page_get_annotations = page_get_annotations;
  functions->page_render = page_render;
#ifdef HAVE_CAIRO
  functions->page_render_cairo = page_render_cairo;
#endif
}

zathura_error_t
document_open(zathura_document_t* document)
{
  if (document == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  zathura_error_t error = ZATHURA_ERROR_OK;

  if ((error = zathura_document_set_number_of_pages(document, 10))
      != ZATHURA_ERROR_OK) {
    return error;
  }

  if ((error = zathura_document_set_page_mode(document, ZATHURA_PAGE_MODE_USE_NONE))
      != ZATHURA_ERROR_OK) {
    return error;
  }

  if ((error = zathura_document_set_page_layout(document, ZATHURA_PAGE_LAYOUT_SINGLE_PAGE))
      != ZATHURA_ERROR_OK) {
    return error;
  }

  if ((error = zathura_document_set_permissions(document, ZATHURA_PERMISSION_PRINT))
      != ZATHURA_ERROR_OK) {
    return error;
  }

  return ZATHURA_ERROR_OK;
}

zathura_error_t
document_free(zathura_document_t* UNUSED(document))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_init(zathura_page_t* page)
{
  if (page == NULL) {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  unsigned int index;
  if (zathura_page_get_index(page, &index) == ZATHURA_ERROR_OK && index == 0) {
    zathura_page_set_label(page, "abc");
  }

  if (zathura_page_set_width(page, 600) != ZATHURA_ERROR_OK) {
    return ZATHURA_ERROR_UNKNOWN;
  }

  if (zathura_page_set_height(page, 800) != ZATHURA_ERROR_OK) {
    return ZATHURA_ERROR_UNKNOWN;
  }

  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_clear(zathura_page_t* UNUSED(page))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
document_save_as(zathura_document_t* UNUSED(document), const char* UNUSED(path))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
document_get_outline(zathura_document_t* UNUSED(document), zathura_node_t**
    UNUSED(outline))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
document_get_attachments(zathura_document_t* UNUSED(document),
    zathura_list_t** UNUSED(attachments))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
document_get_metadata(zathura_document_t* UNUSED(document), zathura_list_t**
    UNUSED(metadata))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_search_text(zathura_page_t* UNUSED(page), const char* UNUSED(text),
    zathura_search_flag_t UNUSED(flags), zathura_list_t** UNUSED(results))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_text(zathura_page_t* UNUSED(page), char** UNUSED(text))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_selected_text(zathura_page_t* UNUSED(page), char** UNUSED(text), zathura_rectangle_t
    UNUSED(rectangle))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_links(zathura_page_t* UNUSED(page), zathura_list_t** UNUSED(links))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_form_fields(zathura_page_t* UNUSED(page), zathura_list_t**
    UNUSED(form_fields))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_images(zathura_page_t* UNUSED(page), zathura_list_t** UNUSED(images))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_get_annotations(zathura_page_t* UNUSED(page), zathura_list_t**
    UNUSED(annotations))
{
  return ZATHURA_ERROR_OK;
}

zathura_error_t
page_render(zathura_page_t* UNUSED(page), zathura_image_buffer_t** UNUSED(buffer),
    double UNUSED(scale), int UNUSED(rotation), int UNUSED(flags))
{
  return ZATHURA_ERROR_OK;
}

#ifdef HAVE_CAIRO
zathura_error_t
page_render_cairo(zathura_page_t* page, cairo_t* cairo, double
    UNUSED(scale), int UNUSED(rotation), int UNUSED(flags))
{
  return ZATHURA_ERROR_OK;
}

#endif
