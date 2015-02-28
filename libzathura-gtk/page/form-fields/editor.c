/* See LICENSE file for license and copyright information */

#include "editor.h"
#include "../internal.h"
#include "../../macros.h"

#include "form-field-button.h"
#include "form-field-text.h"

static zathura_rectangle_t
rotate_rectangle(zathura_rectangle_t rectangle, unsigned int degree, unsigned int width, unsigned int height)
{
  zathura_rectangle_t tmp;

  switch (degree) {
    case 90:
      tmp.p1.x = height - rectangle.p2.y;
      tmp.p1.y = rectangle.p1.x;
      tmp.p2.x = height - rectangle.p1.y;
      tmp.p2.y = rectangle.p2.x;
      break;
    case 180:
      tmp.p1.x = width  - rectangle.p2.x;
      tmp.p1.y = height - rectangle.p2.y;
      tmp.p2.x = width  - rectangle.p1.x;
      tmp.p2.y = height - rectangle.p1.y;
      break;
    case 270:
      tmp.p1.x = rectangle.p1.y;
      tmp.p1.y = width - rectangle.p2.x;
      tmp.p2.x = rectangle.p2.y;
      tmp.p2.y = width - rectangle.p1.x;
      break;
    default:
      tmp.p1.x = rectangle.p1.x;
      tmp.p1.y = rectangle.p1.y;
      tmp.p2.x = rectangle.p2.x;
      tmp.p2.y = rectangle.p2.y;
      break;
  }

  return tmp;
}

static zathura_rectangle_t
scale_rectangle(zathura_rectangle_t rectangle, double scale)
{
  zathura_rectangle_t scaled_rectangle;

  scaled_rectangle.p1.x = rectangle.p1.x * scale;
  scaled_rectangle.p1.y = rectangle.p1.y * scale;
  scaled_rectangle.p2.x = rectangle.p2.x * scale;
  scaled_rectangle.p2.y = rectangle.p2.y * scale;

  return scaled_rectangle;
}

static zathura_rectangle_t
calculate_correct_position(ZathuraPagePrivate* priv, GtkWidget* UNUSED(widget), zathura_rectangle_t position)
{
  /* Rotate rectangle */
  zathura_rectangle_t correct_position = rotate_rectangle(
      position,
      priv->settings.rotation,
      priv->dimensions.width,
      priv->dimensions.height
      );

  /* Scale rectangle */
  correct_position = scale_rectangle(correct_position, priv->settings.scale);

  return correct_position;
}

gboolean
cb_form_field_editor_build(GtkWidget *widget, cairo_t *UNUSED(cairo), gpointer data)
{
  ZathuraPagePrivate* priv = (ZathuraPagePrivate*) data;

  /* Draw form-fields if requested */
  if (priv->form_fields.edit == false) {
    return FALSE;
  }

  /* Check if we created the editor already */
  if (priv->form_fields.retrieved == true) {
    return FALSE;
  }

  priv->form_fields.retrieved = true;

  /* Retrieve the form fields for this page */
  if (zathura_page_get_form_fields(priv->page, &(priv->form_fields.list)) != ZATHURA_ERROR_OK) {
    return FALSE;
  }

  /* Create link widgets */
  zathura_form_field_mapping_t* form_field_mapping;
  ZATHURA_LIST_FOREACH(form_field_mapping, priv->form_fields.list) {
    zathura_form_field_type_t form_field_type = ZATHURA_FORM_FIELD_UNKNOWN;
    if (zathura_form_field_get_type(form_field_mapping->form_field, &form_field_type) != ZATHURA_ERROR_OK) {
      continue;
    }

    GtkWidget* form_field_widget = NULL;
    switch (form_field_type) {
      case ZATHURA_FORM_FIELD_BUTTON:
        form_field_widget = zathura_gtk_form_field_button_new(form_field_mapping->form_field);
        break;
      case ZATHURA_FORM_FIELD_TEXT:
        form_field_widget = zathura_gtk_form_field_text_new(form_field_mapping->form_field);
        break;
      default:
        break;
    }

    if (form_field_widget != NULL) {
      zathura_rectangle_t position = calculate_correct_position(priv, widget, form_field_mapping->position);
      unsigned int width  = position.p2.x - position.p1.x;
      unsigned int height = position.p2.y - position.p1.y;

      gtk_fixed_put(GTK_FIXED(priv->layer.form_fields), form_field_widget, position.p1.x, position.p1.y);
      gtk_widget_set_size_request(form_field_widget, width, height);
      gtk_widget_show(form_field_widget);
    }
  }

  return TRUE;
}
