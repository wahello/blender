/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "UI_interface.h"
#include "UI_resources.h"

#include "node_geometry_util.hh"

static bNodeSocketTemplate geo_node_attribute_fill_in[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {SOCK_STRING, N_("Attribute")},
    {SOCK_VECTOR, N_("Value"), 0.0f, 0.0f, 0.0f, 0.0f, -FLT_MAX, FLT_MAX},
    {SOCK_FLOAT, N_("Value"), 0.0f, 0.0f, 0.0f, 0.0f, -FLT_MAX, FLT_MAX},
    {SOCK_RGBA, N_("Value"), 0.0f, 0.0f, 0.0f, 0.0f, -FLT_MAX, FLT_MAX},
    {SOCK_BOOLEAN, N_("Value"), 0.0f, 0.0f, 0.0f, 0.0f, -FLT_MAX, FLT_MAX},
    {SOCK_INT, N_("Value"), 0, 0, 0, 0, -10000000.0f, 10000000.0f},
    {-1, ""},
};

static bNodeSocketTemplate geo_node_attribute_fill_out[] = {
    {SOCK_GEOMETRY, N_("Geometry")},
    {-1, ""},
};

static void geo_node_attribute_fill_layout(uiLayout *layout, bContext *UNUSED(C), PointerRNA *ptr)
{
  uiLayoutSetPropSep(layout, true);
  uiLayoutSetPropDecorate(layout, false);
  uiItemR(layout, ptr, "domain", 0, "", ICON_NONE);
  uiItemR(layout, ptr, "data_type", 0, "", ICON_NONE);
}

static void geo_node_attribute_fill_init(bNodeTree *UNUSED(tree), bNode *node)
{
  node->custom1 = CD_PROP_FLOAT;
  node->custom2 = ATTR_DOMAIN_AUTO;
}

static void geo_node_attribute_fill_update(bNodeTree *UNUSED(ntree), bNode *node)
{
  bNodeSocket *socket_value_vector = (bNodeSocket *)BLI_findlink(&node->inputs, 2);
  bNodeSocket *socket_value_float = socket_value_vector->next;
  bNodeSocket *socket_value_color4f = socket_value_float->next;
  bNodeSocket *socket_value_boolean = socket_value_color4f->next;
  bNodeSocket *socket_value_int32 = socket_value_boolean->next;

  const CustomDataType data_type = static_cast<CustomDataType>(node->custom1);

  nodeSetSocketAvailability(socket_value_vector, data_type == CD_PROP_FLOAT3);
  nodeSetSocketAvailability(socket_value_float, data_type == CD_PROP_FLOAT);
  nodeSetSocketAvailability(socket_value_color4f, data_type == CD_PROP_COLOR);
  nodeSetSocketAvailability(socket_value_boolean, data_type == CD_PROP_BOOL);
  nodeSetSocketAvailability(socket_value_int32, data_type == CD_PROP_INT32);
}

namespace blender::nodes {

static AttributeDomain get_result_domain(const GeometryComponent &component,
                                         StringRef attribute_name)
{
  /* Use the domain of the result attribute if it already exists. */
  ReadAttributeLookup result_attribute = component.attribute_try_get_for_read(attribute_name);
  if (result_attribute) {
    return result_attribute.domain;
  }
  return ATTR_DOMAIN_POINT;
}

static void fill_attribute(GeometryComponent &component, const GeoNodeExecParams &params)
{
  const std::string attribute_name = params.get_input<std::string>("Attribute");
  if (attribute_name.empty()) {
    return;
  }

  const bNode &node = params.node();
  const CustomDataType data_type = static_cast<CustomDataType>(node.custom1);
  const AttributeDomain domain = static_cast<AttributeDomain>(node.custom2);
  const AttributeDomain result_domain = (domain == ATTR_DOMAIN_AUTO) ?
                                            get_result_domain(component, attribute_name) :
                                            domain;

  MaybeUnsavedWriteAttribute attribute = component.attribute_try_get_for_output(
      attribute_name, result_domain, data_type);
  if (!attribute) {
    return;
  }

  switch (data_type) {
    case CD_PROP_FLOAT: {
      const float value = params.get_input<float>("Value_001");
      attribute->fill(&value);
      break;
    }
    case CD_PROP_FLOAT3: {
      const float3 value = params.get_input<float3>("Value");
      attribute->fill(&value);
      break;
    }
    case CD_PROP_COLOR: {
      const Color4f value = params.get_input<Color4f>("Value_002");
      attribute->fill(&value);
      break;
    }
    case CD_PROP_BOOL: {
      const bool value = params.get_input<bool>("Value_003");
      attribute->fill(&value);
      break;
    }
    case CD_PROP_INT32: {
      const int value = params.get_input<int>("Value_004");
      attribute->fill(&value);
      break;
    }
    default:
      break;
  }

  attribute.save_if_necessary();
}

static void geo_node_attribute_fill_exec(GeoNodeExecParams params)
{
  GeometrySet geometry_set = params.extract_input<GeometrySet>("Geometry");

  geometry_set = geometry_set_realize_instances(geometry_set);

  if (geometry_set.has<MeshComponent>()) {
    fill_attribute(geometry_set.get_component_for_write<MeshComponent>(), params);
  }
  if (geometry_set.has<PointCloudComponent>()) {
    fill_attribute(geometry_set.get_component_for_write<PointCloudComponent>(), params);
  }

  params.set_output("Geometry", geometry_set);
}

}  // namespace blender::nodes

void register_node_type_geo_attribute_fill()
{
  static bNodeType ntype;

  geo_node_type_base(&ntype, GEO_NODE_ATTRIBUTE_FILL, "Attribute Fill", NODE_CLASS_ATTRIBUTE, 0);
  node_type_socket_templates(&ntype, geo_node_attribute_fill_in, geo_node_attribute_fill_out);
  node_type_init(&ntype, geo_node_attribute_fill_init);
  node_type_update(&ntype, geo_node_attribute_fill_update);
  ntype.geometry_node_execute = blender::nodes::geo_node_attribute_fill_exec;
  ntype.draw_buttons = geo_node_attribute_fill_layout;
  nodeRegisterType(&ntype);
}
