/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * Double Glow Effect 2022 Beaver 
 */


/*There was once a gaussian 2 property that was never used. I Kept the name gaussian 3 to avoid preset breakages. It is what it is. 

Here is Double Glow's GEGL Graph from July 2022. This may not be 100% the same 
as the plugin. If you feed this info to Gimp's GEGL Graph you can test it.

photocopy black=3
color-to-alpha
color-overlay value=#ffe76c
opacity value=4
dropshadow radius=10 grow-radius=33 color=#ee91ff
gaussian-blur std-dev-x=40 std-dev-y=40


*/
#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (photocopy, _("Internal Photocopy mask radius"), 5.0)
    value_range (3, 50.0)

property_double (photocopy2, _("Internal Photocopy White percentage"), 1)
    value_range (0.5, 1.0)


property_double (photocopy3, _("Internal Photocopy Black percentage"), 1)
    value_range (0.5, 1.0)
    ui_meta     ("role", "output-extent")

property_double (photocopy4, _("Internal Photocopy Sharpness"), 0.99)
    value_range (0.6, 1.0)


property_color (color, _("Glow 1 Color"), "#0061ff")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")

property_double (gaussian, _("Glow 1 Blur"), 24.0)
   description (_("Standard deviation for the X and Y axis"))
   value_range (0.0, 50.0)
   ui_range    (0.0, 50.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")


property_double (opacity, _("Glow 1 Opacity"), 2.5)
    description (_("Global opacity value that is always used on top of the optional auxiliary input buffer."))
    value_range (0.0, 4.0)
    ui_range    (0.0, 4.0)


property_color  (color2, _("Glow 2 Color"), "#ffdb00")
  description   (_("The shadow's color (defaults to 'black')"))



property_double (radius, _("Glow 2 Blur radius"), 1.0)
  value_range   (0.0, G_MAXDOUBLE)
  ui_range      (0.0, 40.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")


property_double (opacity2, _("Glow 2 Opacity"), 1.2)
  value_range   (0.0, 2.0)
  ui_steps      (0.01, 0.10)

property_double (grow_radius, _("Glow 2 Glow Radius"), 17.0)
  value_range   (-1, 50.0)
  ui_range      (-1.0, 50.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow before blurring; a negative value will contract the shadow instead"))




property_double (gaussian3, _("Universal Blur"), 10.0)
   description (_("Blur that applies to the whole filter"))
   value_range (0.0, 60.0)
   ui_range    (0.24, 60.0)
   ui_gamma    (3.0)
   ui_meta     ("unit", "pixel-distance")

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     doubleglow
#define GEGL_OP_C_SOURCE doubleglow.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *photocopy, *c2a, *color, *gaussian, *gaussian3, *glow2, *opacity;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

   photocopy = gegl_node_new_child (gegl,
                                  "operation", "gegl:photocopy",
                                  NULL);

   c2a = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-to-alpha",
                                  NULL);

   color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

   gaussian = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);


   gaussian3 = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur",
                                  NULL);

   opacity = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);

   glow2 = gegl_node_new_child (gegl,
                                  "operation", "gegl:dropshadow", "x", 0.0, "y", 0.0,  NULL);
                          

  gegl_node_link_many (input, photocopy, c2a, color, gaussian, opacity, glow2, gaussian3, output, NULL);



  gegl_operation_meta_redirect (operation, "photocopy", photocopy, "mask-radius");
  gegl_operation_meta_redirect (operation, "photocopy2", photocopy, "white");
  gegl_operation_meta_redirect (operation, "photocopy3", photocopy, "black");
  gegl_operation_meta_redirect (operation, "photocopy4", photocopy, "sharpness");
  gegl_operation_meta_redirect (operation, "color", color, "value");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian", gaussian, "std-dev-y");
  gegl_operation_meta_redirect (operation, "gaussian3", gaussian3, "std-dev-x");
  gegl_operation_meta_redirect (operation, "gaussian3", gaussian3, "std-dev-y");
  gegl_operation_meta_redirect (operation, "opacity2", glow2, "opacity");
  gegl_operation_meta_redirect (operation, "grow_radius", glow2, "grow-radius");
  gegl_operation_meta_redirect (operation, "radius", glow2, "radius");
  gegl_operation_meta_redirect (operation, "color2", glow2, "color");



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",               "lb:doubleglow",
    "title",              _("Double Glow Duel Color Effect"),
    "categories",         "Artistic",
    "position-dependent", "true",
    "reference-hash",     "f1bfgec914f0b44da36130b3abb73c9",
    "reference-hashB",    "b11bc33d1089355aad0642b4aca8791",
    "license",            "GPL3+",
    "description", _("For filter to properly work enable an alpha channel and use Gimp's layer blend modes. HSL Color (mid opacity),  Overlay, Grain Merge, Linear Light (low opacity) work best. GEGL's built in blend modes will cause color loss. Where as Gimp's layer blend modes will not.'"),
    NULL);
}

#endif
