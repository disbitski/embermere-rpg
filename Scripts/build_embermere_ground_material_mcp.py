import json


MATERIAL_PATH = (
    "/Game/Art/Embermere/Environment/"
    "M_EmbermereGround.M_EmbermereGround"
)


def call(tool_name, payload):
    return execute_tool(tool_name, json.dumps(payload))["returnValue"]


def materials(tool_name, payload):
    return call("editor_toolset.toolsets.material.MaterialTools." + tool_name, payload)


def objects(tool_name, payload):
    return call("editor_toolset.toolsets.object.ObjectTools." + tool_name, payload)


def assets(tool_name, payload):
    return call("editor_toolset.toolsets.asset.AssetTools." + tool_name, payload)


def add(expression_class, x, y):
    return materials("add_expression", {
        "material_or_function": {"refPath": MATERIAL_PATH},
        "expression_class": {"refPath": "/Script/Engine." + expression_class},
        "x": x,
        "y": y,
    })


def set_properties(expression, values):
    return objects("set_properties", {
        "instance": expression,
        "values": json.dumps(values),
    })


def connect(source, target, input_name, output_name=""):
    return materials("connect_expressions", {
        "from_expression": source,
        "from_output_name": output_name,
        "to_expression": target,
        "to_input_name": input_name,
    })


def connect_output(source, material_property):
    return materials("connect_to_output", {
        "expression": source,
        "output_name": "",
        "material_property": material_property,
    })


def scalar(value, x, y):
    node = add("MaterialExpressionConstant", x, y)
    set_properties(node, {"r": float(value)})
    return node


def scalar_parameter(name, value, group, x, y):
    node = add("MaterialExpressionScalarParameter", x, y)
    set_properties(node, {
        "parameterName": name,
        "defaultValue": float(value),
        "group": group,
    })
    return node


def vector_parameter(name, color, group, x, y):
    node = add("MaterialExpressionVectorParameter", x, y)
    set_properties(node, {
        "parameterName": name,
        "defaultValue": {
            "r": float(color[0]),
            "g": float(color[1]),
            "b": float(color[2]),
            "a": 1.0,
        },
        "group": group,
    })
    return node


def binary(expression_class, left, right, x, y):
    node = add(expression_class, x, y)
    connect(left, node, "A")
    connect(right, node, "B")
    return node


def unary(expression_class, source, x, y):
    node = add(expression_class, x, y)
    connect(source, node, "")
    return node


def run():
    material_ref = {"refPath": MATERIAL_PATH}
    for material_property in ("MP_BaseColor", "MP_Roughness"):
        try:
            materials("disconnect_from_output", {
                "material": material_ref,
                "material_property": material_property,
            })
        except Exception:
            # Interrupted editor runs may leave an output already disconnected.
            pass
    for expression in materials("get_expressions", {
        "material_or_function": material_ref,
    }):
        materials("delete_expression", {
            "material_or_function": material_ref,
            "expression": expression,
        })

    world_position = add("MaterialExpressionWorldPosition", -1800, 0)
    world_x = add("MaterialExpressionComponentMask", -1580, -120)
    world_y = add("MaterialExpressionComponentMask", -1580, 120)
    set_properties(world_x, {"r": True, "g": False, "b": False, "a": False})
    set_properties(world_y, {"r": False, "g": True, "b": False, "a": False})
    connect(world_position, world_x, "")
    connect(world_position, world_y, "")

    # Two crossed low-frequency waves provide inexpensive, broad moss breakup.
    wave_1_x = binary(
        "MaterialExpressionMultiply",
        world_x,
        scalar(0.00062, -1580, -360),
        -1320,
        -330,
    )
    wave_1_y = binary(
        "MaterialExpressionMultiply",
        world_y,
        scalar(0.00041, -1580, -480),
        -1320,
        -430,
    )
    wave_1_sum = binary("MaterialExpressionAdd", wave_1_x, wave_1_y, -1060, -380)
    wave_1 = unary("MaterialExpressionSine", wave_1_sum, -840, -380)

    wave_2_x = binary(
        "MaterialExpressionMultiply",
        world_x,
        scalar(-0.00037, -1580, 360),
        -1320,
        330,
    )
    wave_2_y = binary(
        "MaterialExpressionMultiply",
        world_y,
        scalar(0.00078, -1580, 480),
        -1320,
        430,
    )
    wave_2_sum = binary("MaterialExpressionAdd", wave_2_x, wave_2_y, -1060, 380)
    wave_2 = unary("MaterialExpressionSine", wave_2_sum, -840, 380)

    wave_sum = binary("MaterialExpressionAdd", wave_1, wave_2, -610, -80)
    wave_quarter = binary(
        "MaterialExpressionMultiply",
        wave_sum,
        scalar(0.25, -840, -40),
        -380,
        -80,
    )
    macro_alpha = binary(
        "MaterialExpressionAdd",
        wave_quarter,
        scalar(0.5, -610, 80),
        -150,
        -80,
    )

    moss_dark = vector_parameter(
        "MossDark",
        (0.045, 0.090, 0.035),
        "Ground Colors",
        -380,
        -300,
    )
    moss_light = vector_parameter(
        "MossLight",
        (0.105, 0.180, 0.070),
        "Ground Colors",
        -380,
        -220,
    )
    moss_color = add("MaterialExpressionLinearInterpolate", 100, -180)
    connect(moss_dark, moss_color, "A")
    connect(moss_light, moss_color, "B")
    connect(macro_alpha, moss_color, "Alpha")

    # The starter route follows y - 0.4444x + 133 = 0.
    path_slope = scalar(-0.4444, -1300, 700)
    path_x = binary("MaterialExpressionMultiply", world_x, path_slope, -1040, 700)
    path_line = binary("MaterialExpressionAdd", path_x, world_y, -800, 700)
    path_offset = binary(
        "MaterialExpressionAdd",
        path_line,
        scalar(133.0, -1040, 820),
        -560,
        700,
    )
    path_abs = unary("MaterialExpressionAbs", path_offset, -340, 700)
    path_width = scalar_parameter(
        "PathHalfWidthCm",
        300.0,
        "Path",
        -560,
        840,
    )
    path_ratio = binary("MaterialExpressionDivide", path_abs, path_width, -100, 700)
    path_saturated = unary("MaterialExpressionSaturate", path_ratio, 120, 700)
    path_mask = unary("MaterialExpressionOneMinus", path_saturated, 340, 700)

    path_dirt = vector_parameter(
        "PathDirt",
        (0.135, 0.080, 0.035),
        "Ground Colors",
        100,
        0,
    )
    final_color = add("MaterialExpressionLinearInterpolate", 600, -80)
    connect(moss_color, final_color, "A")
    connect(path_dirt, final_color, "B")
    connect(path_mask, final_color, "Alpha")

    moss_roughness = scalar_parameter(
        "MossRoughness",
        0.82,
        "Surface",
        100,
        260,
    )
    path_roughness = scalar_parameter(
        "PathRoughness",
        0.92,
        "Surface",
        100,
        360,
    )
    final_roughness = add("MaterialExpressionLinearInterpolate", 600, 300)
    connect(moss_roughness, final_roughness, "A")
    connect(path_roughness, final_roughness, "B")
    connect(path_mask, final_roughness, "Alpha")

    connect_output(final_color, "MP_BaseColor")
    connect_output(final_roughness, "MP_Roughness")
    materials("layout_expressions", {"material_or_function": material_ref})
    materials("recompile", {"material_or_function": material_ref})
    saved = assets("save_assets", {
        "asset_paths": [
            "/Game/Art/Embermere/Environment/M_EmbermereGround",
        ],
    })

    expression_count = len(materials("get_expressions", {
        "material_or_function": material_ref,
    }))
    if expression_count != 38:
        raise RuntimeError(
            "Expected 38 ground-material expressions, found " + str(expression_count)
        )
    return {
        "material": MATERIAL_PATH,
        "expression_count": expression_count,
        "path_half_width_cm": 300.0,
        "saved": saved,
    }
