#
# Copyright 2011-2013 Blender Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# <pep8 compliant>
from __future__ import annotations

from bl_operators.presets import AddPresetBase
from bpy.types import Operator


class AddPresetIntegrator(AddPresetBase, Operator):
    '''Add an Integrator Preset'''
    bl_idname = "render.cycles_integrator_preset_add"
    bl_label = "Add Integrator Preset"
    preset_menu = "CYCLES_PT_integrator_presets"

    preset_defines = [
        "cycles = bpy.context.scene.cycles"
    ]

    preset_values = [
        "cycles.max_bounces",
        "cycles.diffuse_bounces",
        "cycles.glossy_bounces",
        "cycles.transmission_bounces",
        "cycles.volume_bounces",
        "cycles.transparent_max_bounces",
        "cycles.caustics_reflective",
        "cycles.caustics_refractive",
        "cycles.blur_glossy"
        "cycles.use_fast_gi"
        "cycles.ao_bounces"
        "cycles.ao_bounces_render"
    ]

    preset_subdir = "cycles/integrator"


class AddPresetSampling(AddPresetBase, Operator):
    '''Add a Sampling Preset'''
    bl_idname = "render.cycles_sampling_preset_add"
    bl_label = "Add Sampling Preset"
    preset_menu = "CYCLES_PT_sampling_presets"

    preset_defines = [
        "cycles = bpy.context.scene.cycles"
    ]

    preset_values = [
        "cycles.samples",
        "cycles.preview_samples",
        "cycles.use_square_samples",
        "cycles.seed",
        "cycles.sample_clamp_direct",
        "cycles.sample_clamp_indirect",
    ]

    preset_subdir = "cycles/sampling"


classes = (
    AddPresetIntegrator,
    AddPresetSampling,
)


def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)


def unregister():
    from bpy.utils import unregister_class
    for cls in classes:
        unregister_class(cls)


if __name__ == "__main__":
    register()
