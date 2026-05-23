#!/usr/bin/env python3
"""
Generates Tetrahedron.gltf and Tetrahedron.bin with correct UV coordinates.
Each face of the tetrahedron is mapped to a different quadrant of the texture.
Run from the demo-scene/assets directory, with uv-test.png present.
"""
import struct, json

# The 4 corner positions of the tetrahedron
P = [
    (0.0, 1.0, 0.0),  # top
    (1.0, 2.0, 1.0),  # back
    (2.0, 1.0, 0.0),  # right
    (1.0, 0.0, 2.0),  # front
]

# Index triplets for each face
faces = [
    (0, 1, 2),  # top-back-right
    (0, 1, 3),  # top-back-front
    (0, 2, 3),  # top-right-front
    (1, 2, 3),  # back-right-front
]

# Each face gets a different quadrant of the UV texture
face_uvs = [
    [(0.0, 0.5), (0.5, 1.0), (0.5, 0.5)],  # bottom-left quadrant
    [(0.5, 0.5), (1.0, 1.0), (1.0, 0.5)],  # bottom-right quadrant
    [(0.0, 0.0), (0.5, 0.5), (0.5, 0.0)],  # top-left quadrant
    [(0.5, 0.0), (1.0, 0.5), (1.0, 0.0)],  # top-right quadrant
]

# Unroll vertices (each face gets its own 3 vertices with unique UVs)
positions_out, uvs_out, indices_out = [], [], []
for fi, (i0, i1, i2) in enumerate(faces):
    base = fi * 3
    indices_out += [base, base + 1, base + 2]
    positions_out += [P[i0], P[i1], P[i2]]
    uvs_out += face_uvs[fi]

n_verts = len(positions_out)  # 12
n_idx   = len(indices_out)    # 12

# Pack binary data
data = bytearray()

idx_offset = len(data)
for idx in indices_out:
    data += struct.pack('<H', idx)
while len(data) % 4 != 0:  # align to 4 bytes
    data += b'\x00'

pos_offset = len(data)
for p in positions_out:
    data += struct.pack('<3f', *p)

uv_offset = len(data)
for uv in uvs_out:
    data += struct.pack('<2f', *uv)

total = len(data)

with open('Tetrahedron.bin', 'wb') as f:
    f.write(data)

gltf = {
    "asset": {"version": "2.0"},
    "scene": 0,
    "scenes": [{"nodes": [0]}],
    "nodes": [{"mesh": 0}],
    "meshes": [{
        "primitives": [{
            "attributes": {"POSITION": 1, "TEXCOORD_0": 2},
            "indices": 0,
            "material": 0
        }]
    }],
    "images": [{"uri": "./uv-test.png"}],
    "samplers": [{}],
    "textures": [{"sampler": 0, "source": 0}],
    "materials": [{
        "pbrMetallicRoughness": {
            "baseColorTexture": {"index": 0, "texCoord": 0}
        }
    }],
    "buffers": [{"uri": "./Tetrahedron.bin", "byteLength": total}],
    "bufferViews": [
        {"buffer": 0, "byteOffset": idx_offset, "byteLength": n_idx * 2,   "target": 34963},
        {"buffer": 0, "byteOffset": pos_offset,  "byteLength": n_verts * 12, "target": 34962},
        {"buffer": 0, "byteOffset": uv_offset,   "byteLength": n_verts * 8,  "target": 34962},
    ],
    "accessors": [
        {
            "bufferView": 0, "byteOffset": 0,
            "componentType": 5123, "count": n_idx,
            "type": "SCALAR", "min": [0], "max": [n_verts - 1]
        },
        {
            "bufferView": 1, "byteOffset": 0,
            "componentType": 5126, "count": n_verts,
            "type": "VEC3", "min": [0.0, 0.0, 0.0], "max": [2.0, 2.0, 2.0]
        },
        {
            "bufferView": 2, "byteOffset": 0,
            "componentType": 5126, "count": n_verts,
            "type": "VEC2"
        }
    ]
}

with open('Tetrahedron.gltf', 'w') as f:
    json.dump(gltf, f, indent=2)

print(f"Generated Tetrahedron.bin ({total} bytes) and Tetrahedron.gltf")
print(f"  {n_verts} vertices, {n_idx} indices, 4 faces")
