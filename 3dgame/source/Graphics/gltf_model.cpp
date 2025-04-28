#include "gltf_model.h"
#define TINYGLTF_IMPLEMENTATION
#include "..//..//tinygltf-release/tiny_gltf.h"
#include "..//misc.h"
#include "shader.h"
#include "texture.h"
#include "GraphicsState.h"
#include <stack>

bool null_load_image_data(tinygltf::Image*, const int, std::string*,std::string*,
    int, int, const unsigned char*, int, void*)
{
    return true;
}

gltf_model::gltf_model(ID3D11Device* device, const std::string& filename) : filename(filename)
{
    tinygltf::TinyGLTF tiny_gltf;
    tiny_gltf.SetImageLoader(null_load_image_data, nullptr);

    tinygltf::Model gltf_model;
    std::string error, warning;
    bool succeeded = { false };
    if (filename.find(".glb") != std::string::npos)
    {
        succeeded = tiny_gltf.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
    }
    else if(filename.find(".gltf") != std::string::npos)
    {
        succeeded = tiny_gltf.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
    }

    _ASSERT_EXPR_A(warning.empty(), warning.c_str());
    _ASSERT_EXPR_A(error.empty(), error.c_str());
    _ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");
    for (std::vector<tinygltf::Scene>::const_reference gltf_scene : gltf_model.scenes)
    {
        scene& scene{ scenes.emplace_back() };
        scene.name = gltf_model.scenes.at(0).name;
        scene.nodes = gltf_model.scenes.at(0).nodes;
    }

    fetch_meshes(device, gltf_model);
    fetch_nodes(gltf_model);
    fetch_materials(device, gltf_model);
    fetch_textures(device, gltf_model);
    fetch_animations(gltf_model);

    g_model = &gltf_model;

    //  TODO: This is a force-brute programming, may cause bugs.
    const std::map<std::string, buffer_view>& vertex_buffer_views{
        meshes.at(0).primitives.at(0).vertex_buffer_views };
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, vertex_buffer_views.at("POSITION").format, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL", 0, vertex_buffer_views.at("NORMAL").format, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TANGENT", 0, vertex_buffer_views.at("TANGENT").format, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, vertex_buffer_views.at("TEXCOORD_0").format, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "JOINTS", 0, vertex_buffer_views.at("JOINTS_0").format, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "WEIGHTS", 0, vertex_buffer_views.at("WEIGHTS_0").format, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    create_vs_from_cso(device, ".//Data//Shader//gltf_model_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
        input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, ".//Data//Shader//gltf_model_ps.cso", pixel_shader.ReleaseAndGetAddressOf());


    HRESULT hr;

    //{
    //    D3D11_RASTERIZER_DESC rasterizer_desc{};
    //    /*rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    //    rasterizer_desc.CullMode = D3D11_CULL_BACK;
    //    rasterizer_desc.FrontCounterClockwise = TRUE;
    //    rasterizer_desc.DepthBias = 0;
    //    rasterizer_desc.DepthBiasClamp = 0;
    //    rasterizer_desc.SlopeScaledDepthBias = 0;
    //    rasterizer_desc.DepthClipEnable = TRUE;
    //    rasterizer_desc.ScissorEnable = FALSE;
    //    rasterizer_desc.MultisampleEnable = FALSE;
    //    rasterizer_desc.AntialiasedLineEnable = FALSE;*/
    //    hr = device->CreateRasterizerState(&rasterizer_desc, GState.GetRasterizerState(RASTER_STATE::SOLID).GetAddressOf());
    //    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //}
    //{
    //    D3D11_BLEND_DESC blend_desc{};
    //    /*blend_desc.AlphaToCoverageEnable = FALSE;
    //    blend_desc.IndependentBlendEnable = FALSE;
    //    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    //    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    //    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    //    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    //    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/
    //    hr = device->CreateBlendState(&blend_desc, GState.GetBlendState(BLEND_STATE::ALPHA).GetAddressOf());
    //    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //}
    //{
    //    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
    //    //depth_stencil_desc.DepthEnable = true;	//深度テストのon,off
    //    //depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	//深度ステンシルバッファへの書き込みのon,off
    //    //depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    //    hr = device->CreateDepthStencilState(&depth_stencil_desc, GState.GetDepthStencilState(DEPTH_STATE::ZT_ON_ZW_ON).GetAddressOf());
    //    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //}
    {
        D3D11_SAMPLER_DESC sampler_desc;
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MipLODBias = 0;
        sampler_desc.MaxAnisotropy = 16;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.BorderColor[0] = 1.0f;
        sampler_desc.BorderColor[1] = 1.0f;
        sampler_desc.BorderColor[2] = 1.0f;
        sampler_desc.BorderColor[3] = 1.0f;
        sampler_desc.MinLOD = 0;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
        hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.ByteWidth = sizeof(primitive_constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_cbuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    buffer_desc.ByteWidth = sizeof(primitive_joint_constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, NULL, primitive_joint_cbuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    InitBoundingBoxVertices(device);
}

void gltf_model::fetch_nodes(const tinygltf::Model& gltf_model)
{
    for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
    {
        node& node = { nodes.emplace_back() };
        node.name = gltf_node.name;
        node.skin = gltf_node.skin;
        node.mesh = gltf_node.mesh;
        node.children = gltf_node.children;
        if (!gltf_node.matrix.empty())
        {
            DirectX::XMFLOAT4X4 matrix;
            for (size_t row = 0; row < 4; row++)
            {
                for (size_t column = 0; column < 4; column++)
                {
                    matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
                }
            }

            DirectX::XMVECTOR S, T, R;
            bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
            _ASSERT_EXPR_A(succeed, L"Failed to decompose matrix");

            DirectX::XMStoreFloat3(&node.scale, S);
            DirectX::XMStoreFloat4(&node.rotation, R);
            DirectX::XMStoreFloat3(&node.translation, T);
        }
        else
        {
            if (gltf_node.scale.size() > 0)
            {
                node.scale.x = static_cast<float>(gltf_node.scale.at(0));
                node.scale.y = static_cast<float>(gltf_node.scale.at(1));
                node.scale.z = static_cast<float>(gltf_node.scale.at(2));
            }
            if(gltf_node.translation.size() > 0)
            {
                node.translation.x = static_cast<float>(gltf_node.translation.at(0));
                node.translation.y = static_cast<float>(gltf_node.translation.at(1));
                node.translation.z = static_cast<float>(gltf_node.translation.at(2));
            }
            if(gltf_node.rotation.size() > 0)
            {
                node.rotation.x = static_cast<float>(gltf_node.rotation.at(0));
                node.rotation.y = static_cast<float>(gltf_node.rotation.at(1));
                node.rotation.z = static_cast<float>(gltf_node.rotation.at(2));
                node.rotation.w = static_cast<float>(gltf_node.rotation.at(3));
            }
        }
    }
    cumulate_transforms(nodes);
}

void gltf_model::cumulate_transforms(std::vector<node>& nodes)
{
    using namespace DirectX;

    std::stack<XMFLOAT4X4> parent_global_transforms;
    std::function<void(int)> traverse = { [&](int node_index)->void
    {
        node& node = {nodes.at(node_index)};
        XMMATRIX S = { XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z) };
        XMMATRIX R = { XMMatrixRotationQuaternion(
            XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w)) };
        XMMATRIX T = { XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };
        XMStoreFloat4x4(&node.global_transform, S * R * T * XMLoadFloat4x4(&parent_global_transforms.top()));
        for (int child_index : node.children)
        {
            parent_global_transforms.push(node.global_transform);
            traverse(child_index);
            parent_global_transforms.pop();
        }
    } };
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        parent_global_transforms.push({1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});
        traverse(node_index);
        parent_global_transforms.pop();
    }
}

gltf_model::buffer_view gltf_model::make_buffer_view(const tinygltf::Accessor& accessor)
{
    buffer_view buffer_view;
    switch (accessor.type)
    {
    case TINYGLTF_TYPE_SCALAR:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            buffer_view.format = DXGI_FORMAT_R16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            buffer_view.format = DXGI_FORMAT_R32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT);
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC2:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 2;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC3:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32B32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 3;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    case TINYGLTF_TYPE_VEC4:
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            buffer_view.format = DXGI_FORMAT_R16G16B16A16_UINT;
            buffer_view.stride_in_bytes = sizeof(USHORT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_UINT;
            buffer_view.stride_in_bytes = sizeof(UINT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            buffer_view.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            buffer_view.stride_in_bytes = sizeof(FLOAT) * 4;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            buffer_view.format = DXGI_FORMAT_R8G8B8A8_UINT;
            buffer_view.stride_in_bytes = sizeof(UCHAR) * 4;
            break;
        default:
            _ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
            break;
        }
        break;
    default:
        _ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
        break;
    }
    buffer_view.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view.stride_in_bytes);
    return buffer_view;
}

void gltf_model::fetch_meshes(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr;
    for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
    {
        mesh& mesh = { meshes.emplace_back() };
        mesh.name = gltf_mesh.name;
        for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
        {
            mesh::primitive& primitive = {mesh.primitives.emplace_back()};
            primitive.material = gltf_primitive.material;

            // Create index buffer
            const tinygltf::Accessor& gltf_accessor = { gltf_model.accessors.at(gltf_primitive.indices) };
            const tinygltf::BufferView& gltf_buffer_view = { gltf_model.bufferViews.at(gltf_accessor.bufferView) };

            primitive.index_buffer_view = make_buffer_view(gltf_accessor);

            D3D11_BUFFER_DESC buffer_desc = {};
            buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            D3D11_SUBRESOURCE_DATA subresource_data = {};
            subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
            hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            // Create vertex buffer
            for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
            {
                ///
                const std::string& attr_name = gltf_attribute.first;
                const int accessor_index = gltf_attribute.second;
                ///
                const tinygltf::Accessor& gltf_accessor = { gltf_model.accessors.at(gltf_attribute.second) };
                const tinygltf::BufferView& gltf_buffer_view = { gltf_model.bufferViews.at(gltf_accessor.bufferView) };

                buffer_view vertex_buffer_view = { make_buffer_view(gltf_accessor) };


                D3D11_BUFFER_DESC buffer_desc = {};
                buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
                buffer_desc.Usage = D3D11_USAGE_DEFAULT;
                buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA subresource_data = {};
                subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                    + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
                hr = device->CreateBuffer(&buffer_desc, &subresource_data,
                    vertex_buffer_view.buffer.ReleaseAndGetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

                primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));

                if (attr_name == "POSITION") {
                    const float* positions = reinterpret_cast<const float*>(
                        gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
                        + gltf_buffer_view.byteOffset + gltf_accessor.byteOffset
                        );
                    size_t count = gltf_accessor.count;
                    primitive.vertex.reserve(count);
                    primitive.vertex_count = 0;

                    // 3D空間の座標を取得
                    for (size_t i = 0; i < count; ++i) {
                        float x = positions[i * 3 + 0];
                        float y = positions[i * 3 + 1];
                        float z = positions[i * 3 + 2];
                        primitive.vertex.push_back({ x, y, z });
                        primitive.vertex_count++;
                    }
                }
            }

            // Add dummy attributes if any are missing.
            const std::unordered_map<std::string, buffer_view> attributes{
                {"TANGENT",{DXGI_FORMAT_R32G32B32A32_FLOAT}},
                {"TEXCOORD_0",{DXGI_FORMAT_R32G32_FLOAT}},
                {"JOINTS_0",{DXGI_FORMAT_R16G16B16A16_UINT}},
                {"WEIGHTS_0",{DXGI_FORMAT_R32G32B32A32_FLOAT}},
            };
            for (std::unordered_map<std::string, buffer_view>::const_reference attribute : attributes)
            {
                if (primitive.vertex_buffer_views.find(attribute.first) == primitive.vertex_buffer_views.end())
                {
                    primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
                }
            }
        }
    }
}

void gltf_model::fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    for (std::vector<tinygltf::Material>::const_reference gltf_material : gltf_model.materials)
    {
        std::vector<material>::reference material = materials.emplace_back();

        material.name = gltf_material.name;

        material.data.emissive_factor[0] = static_cast<float>(gltf_material.emissiveFactor.at(0));
        material.data.emissive_factor[1] = static_cast<float>(gltf_material.emissiveFactor.at(1));
        material.data.emissive_factor[2] = static_cast<float>(gltf_material.emissiveFactor.at(2));

        material.data.alpha_mode = gltf_material.alphaMode == "OPAQUE" ?
            0 : gltf_material.alphaMode == "MASK" ? 1 : gltf_material.alphaMode == "BLEND" ? 2 : 0;
        material.data.alpha_cutoff = static_cast<float>(gltf_material.alphaCutoff);
        material.data.double_sided = gltf_material.doubleSided ? 1 : 0;

        material.data.pbr_metallic_roughness.basecolor_factor[0] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(0));
        material.data.pbr_metallic_roughness.basecolor_factor[1] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(1));
        material.data.pbr_metallic_roughness.basecolor_factor[2] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(2));
        material.data.pbr_metallic_roughness.basecolor_factor[3] =
            static_cast<float>(gltf_material.pbrMetallicRoughness.baseColorFactor.at(3));
        material.data.pbr_metallic_roughness.basecolor_texture.index =
            gltf_material.pbrMetallicRoughness.baseColorTexture.index;
        material.data.pbr_metallic_roughness.basecolor_texture.texcoord =
            gltf_material.pbrMetallicRoughness.baseColorTexture.texCoord;
        material.data.pbr_metallic_roughness.metallic_factor =
            static_cast<float>(gltf_material.pbrMetallicRoughness.metallicFactor);
        material.data.pbr_metallic_roughness.roughness_factor =
            static_cast<float>(gltf_material.pbrMetallicRoughness.roughnessFactor);
        material.data.pbr_metallic_roughness.metallic_roughness_texture.index =
            gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        material.data.pbr_metallic_roughness.metallic_roughness_texture.texcoord =
            gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;

        material.data.normal_texture.index = gltf_material.normalTexture.index;
        material.data.normal_texture.texcoord = gltf_material.normalTexture.texCoord;
        material.data.normal_texture.scale = static_cast<float>(gltf_material.normalTexture.scale);

        material.data.occlusion_texture.index = gltf_material.occlusionTexture.index;
        material.data.occlusion_texture.texcoord = gltf_material.occlusionTexture.texCoord;
        material.data.occlusion_texture.strength =
            static_cast<float>(gltf_material.occlusionTexture.strength);

        material.data.emissive_texture.index = gltf_material.emissiveTexture.index;
        material.data.emissive_texture.texcoord = gltf_material.emissiveTexture.texCoord;
    }

    // Create material data as shader resource view on GPU
    std::vector<material::cbuffer> material_data;
    for (std::vector<material>::const_reference material : materials)
    {
        material_data.push_back(material.data);
    }

    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> material_buffer;
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(material::cbuffer) * material_data.size());
    buffer_desc.StructureByteStride = sizeof(material::cbuffer);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = material_data.data();
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, material_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(material_data.size());
    hr = device->CreateShaderResourceView(material_buffer.Get(),
        &shader_resource_view_desc, material_resource_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void gltf_model::fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
    HRESULT hr;
    for (const tinygltf::Texture& gltf_texture : gltf_model.textures)
    {
        texture& texture = { textures.emplace_back() };
        texture.name = gltf_texture.name;
        texture.source = gltf_texture.source;
    }
    for (const tinygltf::Image& gltf_image : gltf_model.images)
    {
        image& image = { images.emplace_back() };
        image.name = gltf_image.name;
        image.width = gltf_image.width;
        image.height = gltf_image.height;
        image.component = gltf_image.component;
        image.bits = gltf_image.bits;
        image.pixel_type = gltf_image.pixel_type;
        image.buffer_view = gltf_image.bufferView;
        image.mime_type = gltf_image.mimeType;
        image.uri = gltf_image.uri;
        image.as_is = gltf_image.as_is;

        if (gltf_image.bufferView > -1)
        {
            const tinygltf::BufferView& buffer_view = { gltf_model.bufferViews.at(gltf_image.bufferView) };
            const tinygltf::Buffer& buffer = { gltf_model.buffers.at(buffer_view.buffer) };
            const byte* data = buffer.data.data() + buffer_view.byteOffset;

            ID3D11ShaderResourceView* texture_resource_view = {};
            hr = load_texture_from_memory(device, data, buffer_view.byteLength, &texture_resource_view);
            if (hr == S_OK)
            {
                texture_resource_views.emplace_back().Attach(texture_resource_view);
            }
        }
        else
        {
            const std::filesystem::path path(filename);
            ID3D11ShaderResourceView* shader_resource_view = {};
            D3D11_TEXTURE2D_DESC texture2d_desc;
            std::wstring filename{
                path.parent_path().concat(L"/").wstring() +
                std::wstring(gltf_image.uri.begin(), gltf_image.uri.end()) };
            hr = load_texture_from_file(device, filename.c_str(), &shader_resource_view, &texture2d_desc);
            if (hr == S_OK)
            {
                texture_resource_views.emplace_back().Attach(shader_resource_view);
            }
        }
    }
}

void gltf_model::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4 world, const std::vector<node>& animated_nodes)
{
    using namespace DirectX;

    const std::vector<node>& nodes = { animated_nodes.size() > 0 ? animated_nodes : gltf_model::nodes };

    GraphicsState& GState = GraphicsState::GetInstance();

    immediate_context->RSSetState(GState.GetRasterizerState(RASTER_STATE::SOLID).Get());
    immediate_context->OMSetBlendState(GState.GetBlendState(BLEND_STATE::ALPHA).Get(), nullptr, 0xFFFFFFFF);
    immediate_context->OMSetDepthStencilState(GState.GetDepthStencilState(DEPTH_STATE::ZT_ON_ZW_ON).Get(), 1);
    immediate_context->PSSetSamplers(0, 1, sampler_states[0].GetAddressOf());
    immediate_context->PSSetSamplers(1, 1, sampler_states[1].GetAddressOf());
    immediate_context->PSSetSamplers(2, 1, sampler_states[2].GetAddressOf());

    immediate_context->PSSetShaderResources(0,1,material_resource_view.GetAddressOf());
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::function<void(int)> traverse = { [&](int node_index)->void {
        const node& node = {nodes.at(node_index)};
        if (node.mesh > -1)
        {
            const mesh& mesh = { meshes.at(node.mesh) };
            for (std::vector<mesh::primitive>::const_reference primitive : mesh.primitives)
            {
                ID3D11Buffer* vertex_buffers[]{
                    primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
                    primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
                    primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
                    primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
                    primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
                    primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
                };
                UINT strides[]{
                    static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
                    static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
                    static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
                    static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
                    static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
                    static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
                };
                UINT offsets[_countof(vertex_buffers)] = { 0 };
                immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
                immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
                    primitive.index_buffer_view.format, 0);

                const material& material = { materials.at(primitive.material) };
                const int texture_indices[]
                {
                    material.data.pbr_metallic_roughness.basecolor_texture.index,
                    material.data.pbr_metallic_roughness.metallic_roughness_texture.index,
                    material.data.normal_texture.index,
                    material.data.emissive_texture.index,
                    material.data.occlusion_texture.index,
                };
                ID3D11ShaderResourceView* null_shader_resource_view = {};
                std::vector<ID3D11ShaderResourceView*> shader_resource_views(_countof(texture_indices));
                for (int texture_index = 0; texture_index < shader_resource_views.size(); ++texture_index)
                {
                    shader_resource_views.at(texture_index) = texture_indices[texture_index] > -1 ?
                        texture_resource_views.at(textures.at(texture_indices[texture_index]).source).Get() :
                        null_shader_resource_view;
                }
                immediate_context->PSSetShaderResources(1, static_cast<UINT>(shader_resource_views.size()),
                    shader_resource_views.data());

                primitive_constants primitive_data = {};
                primitive_data.material = primitive.material;
                primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
                primitive_data.skin = node.skin;
                XMStoreFloat4x4(&primitive_data.world,
                    XMLoadFloat4x4(&node.global_transform) * XMLoadFloat4x4(&world));
                immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
                immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
                immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());

                immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);

            }
            
        }
        if (node.skin > -1)
        {
            const skin& skin = { skins.at(node.skin) };
            primitive_joint_constants primitive_joint_data = {};
            for (size_t joint_index = 0; joint_index < skin.joints.size(); ++joint_index)
            {
                XMStoreFloat4x4(&primitive_joint_data.matrices[joint_index],
                    XMLoadFloat4x4(&skin.inverse_bind_matrices.at(joint_index)) *
                    XMLoadFloat4x4(&nodes.at(skin.joints.at(joint_index)).global_transform) *
                    XMMatrixInverse(NULL, XMLoadFloat4x4(&node.global_transform))
                );
            }
            immediate_context->UpdateSubresource(primitive_joint_cbuffer.Get(), 0, 0, &primitive_joint_data, 0, 0);
            immediate_context->VSSetConstantBuffers(2, 1, primitive_joint_cbuffer.GetAddressOf());
        }
        for (std::vector<int>::value_type child_index : node.children)
        {
            traverse(child_index);
        }
    } };
    for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
    {
        traverse(node_index);
    }
}

void gltf_model::InitBoundingBoxVertices(ID3D11Device* device)
{
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

    };
    create_vs_from_cso(device, ".//Data//Shader//LineVS.cso", line_vertex_shader.ReleaseAndGetAddressOf(),
        line_input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, ".//Data//Shader//LinePS.cso", line_pixel_shader.ReleaseAndGetAddressOf());

    // 定数バッファ
    {
        // シーン用バッファ
        D3D11_BUFFER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(LineVertexBuffer);
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, 0, line_vertex_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // 頂点バッファ
    {
        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = sizeof(LineVertex) * 8;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr, line_cbuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

void gltf_model::render_bounding_box(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4 world, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection,
    DirectX::XMFLOAT4 color)
{
    DirectX::XMFLOAT3 start = { 0, 0, 0 }, end = { 0, 0, 0 };
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
    DirectX::XMMATRIX WVP = DirectX::XMMatrixMultiply(W, DirectX::XMMatrixMultiply(V, P));
    DirectX::XMVECTOR PV;
    static DirectX::XMFLOAT3 bStart = { 0, 0, 0 }, bEnd = { 0, 0, 0 };


    for (auto& mesh : meshes)
    {
        for (auto& primitive : mesh.primitives)
        {
            for (int i = 0; i < primitive.vertex_count; i++)
            {

                if (bStart.x > primitive.vertex[i].x)
                {
                    bStart.x = primitive.vertex[i].x;
                }
                else if (bEnd.x < primitive.vertex[i].x)
                {
                    bEnd.x = primitive.vertex[i].x;
                }
                if (bStart.y > primitive.vertex[i].y)
                {
                    bStart.y = primitive.vertex[i].y;
                }
                else if (bEnd.y < primitive.vertex[i].y)
                {
                    bEnd.y = primitive.vertex[i].y;
                }
                if (bStart.z > primitive.vertex[i].z)
                {
                    bStart.z = primitive.vertex[i].z;
                }
                else if (bEnd.z < primitive.vertex[i].z)
                {
                    bEnd.z = primitive.vertex[i].z;
                }
            }
        }
    }

    DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&bStart);
    DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&bEnd);
    DirectX::XMStoreFloat3(&start, DirectX::XMVector3Transform(S, W));
    DirectX::XMStoreFloat3(&end, DirectX::XMVector3Transform(E, W));


    LineVertex v[8];
    v[0] = { { start.x, start.y, start.z }, color };
    v[1] = { { end.x, start.y, start.z }, color };
    v[2] = { { end.x, start.y, end.z }, color };

    v[3] = { { end.x, end.y, start.z }, color };
    v[4] = { { start.x, end.y, start.z }, color };
    v[4] = { { start.x, start.y, end.z }, color };
    v[6] = { { start.x, end.y, end.z }, color };
    v[7] = { { end.x, end.y, end.z }, color };
    for (int i = 0; i < 8; i++)
    {
        bounding_box_vertices.emplace_back(v[i]);
    }

    GraphicsState& GState = GraphicsState::GetInstance();

    // シェーダー設定
    immediate_context->VSSetShader(line_vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(line_pixel_shader.Get(), nullptr, 0);
    immediate_context->IASetInputLayout(line_input_layout.Get());

    immediate_context->RSSetState(GState.GetRasterizerState(RASTER_STATE::WIREFRAME_CULL_NONE).Get());
    immediate_context->OMSetBlendState(GState.GetBlendState(BLEND_STATE::ALPHA).Get(), nullptr, 0xFFFFFFFF);
    immediate_context->OMSetDepthStencilState(GState.GetDepthStencilState(DEPTH_STATE::ZT_ON_ZW_ON).Get(), 1);

    // 定数バッファ設定
    immediate_context->VSSetConstantBuffers(10, 1, line_vertex_buffer.GetAddressOf());

    // プリミティブ設定
    UINT stride = sizeof(LineVertex);
    UINT offset = 0;
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
    immediate_context->IASetVertexBuffers(0, 1, line_cbuffer.GetAddressOf(), &stride, &offset);

   /* DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);*/
    DirectX::XMMATRIX VP = V * P;
    LineVertexBuffer data;
    DirectX::XMStoreFloat4x4(&data.wvp, VP);
    immediate_context->UpdateSubresource(line_vertex_buffer.Get(), 0, 0, &data, 0, 0);

    UINT totalVertexCount = static_cast<UINT>(bounding_box_vertices.size());
    UINT start_v = 0;
    UINT count = totalVertexCount;

    while (start_v < totalVertexCount)
    {
        D3D11_MAPPED_SUBRESOURCE mappedVB;
        HRESULT hr = immediate_context->Map(line_cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        memcpy(mappedVB.pData, &bounding_box_vertices[start_v], sizeof(LineVertex) * count);

        immediate_context->Unmap(line_cbuffer.Get(), 0);

        immediate_context->Draw(count, 0);

        start_v += count;
        if ((start_v + count) > totalVertexCount)
        {
            count = totalVertexCount - start_v;
        }
    }
    bounding_box_vertices.clear();
}

void gltf_model::fetch_animations(const tinygltf::Model& gltf_model)
{
    using namespace std;
    using namespace tinygltf;
    using namespace DirectX;

    for (vector<Skin>::const_reference transmission_skin : gltf_model.skins)
    {
        skin& skin = { skins.emplace_back() };
        const Accessor& gltf_accessor = { gltf_model.accessors.at(transmission_skin.inverseBindMatrices) };
        const BufferView& gltf_buffer_view = { gltf_model.bufferViews.at(gltf_accessor.bufferView) };
        skin.inverse_bind_matrices.resize(gltf_accessor.count);
        memcpy(skin.inverse_bind_matrices.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
            gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4X4));
        skin.joints = transmission_skin.joints;
    }

    for (vector<Animation>::const_reference gltf_animation : gltf_model.animations)
    {
        animation& animation = { animations.emplace_back() };
        animation.name = gltf_animation.name;
        for (vector<AnimationSampler>::const_reference gltf_sampler : gltf_animation.samplers)
        {
            animation::sampler& sampler = { animation.samplers.emplace_back() };
            sampler.input = gltf_sampler.input;
            sampler.output = gltf_sampler.output;
            sampler.interpolation = gltf_sampler.interpolation;

            const Accessor& gltf_accessor = { gltf_model.accessors.at(gltf_sampler.input) };
            const BufferView& gltf_buffer_view = { gltf_model.bufferViews.at(gltf_accessor.bufferView) };
            const pair<unordered_map<int, vector<float>>::iterator, bool>& timelines{
                animation.timelines.emplace(gltf_sampler.input,gltf_accessor.count) };
            if (timelines.second)
            {
                memcpy(timelines.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                    gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(FLOAT));
            }
        }
        for (vector<AnimationChannel>::const_reference gltf_channel : gltf_animation.channels)
        {
            animation::channel& channel = { animation.channels.emplace_back() };
            channel.sampler = gltf_channel.sampler;
            channel.target_node = gltf_channel.target_node;
            channel.target_path = gltf_channel.target_path;

            const AnimationSampler& gltf_sampler = { gltf_animation.samplers.at(gltf_channel.sampler) };
            const Accessor& gltf_accessor = { gltf_model.accessors.at(gltf_sampler.output) };
            const BufferView& gltf_buffer_view = { gltf_model.bufferViews.at(gltf_accessor.bufferView) };
            if (gltf_channel.target_path == "scale")
            {
                const pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& scales{
                    animation.scales.emplace(gltf_sampler.output,gltf_accessor.count) };
                if (scales.second)
                {
                    memcpy(scales.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
                }
            }
            else if (gltf_channel.target_path == "rotation")
            {
                const pair<unordered_map<int, vector<XMFLOAT4>>::iterator, bool>& rotations{
                    animation.rotations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (rotations.second)
                {
                    memcpy(rotations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT4));
                }
            }
            else if (gltf_channel.target_path == "translation")
            {
                const pair<unordered_map<int, vector<XMFLOAT3>>::iterator, bool>& translations{
                    animation.translations.emplace(gltf_sampler.output, gltf_accessor.count) };
                if (translations.second)
                {
                    memcpy(translations.first->second.data(), gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
                        gltf_buffer_view.byteOffset + gltf_accessor.byteOffset, gltf_accessor.count * sizeof(XMFLOAT3));
                }
            }
        }
    }
    // Find a longest animation duration in timeline of each channel.
    for (decltype(animations)::reference animation : animations)
    {
        for (decltype(animation.timelines)::reference timelines : animation.timelines)
        {
            animation.duration = std::max<float>(animation.duration, timelines.second.back());
        }
    }
}

void gltf_model::animate(size_t animation_index, float time, std::vector<node>& animated_nodes)
{
    using namespace std;
    using namespace DirectX;

    function<size_t(const vector<float>&, float, float&)> indexof{
        [](const vector<float>& timelines, float time, float& interpolation_factor)->size_t {
            const size_t keyframe_count = {timelines.size()};
            if (time > timelines.at(keyframe_count - 1))
            {
                interpolation_factor = 1.0f;
                return keyframe_count - 2;
            }
            else if (time < timelines.at(0))
            {
                interpolation_factor = 0.0f;
                return 0;
            }
            size_t keyframe_index = { 0 };
            for (size_t time_index = 1; time_index < keyframe_count; ++time_index)
            {
                if (time < timelines.at(time_index))
                {
                    keyframe_index = max<size_t>(0LL, time_index - 1);
                    break;
                }
            }
            interpolation_factor = (time - timelines.at(keyframe_index + 0)) /
                (timelines.at(keyframe_index + 1) - timelines.at(keyframe_index + 0));
            return keyframe_index;
    } };

    if (animations.size() > 0)
    {
        const animation& animation = { animations.at(animation_index) };
        for (vector<animation::channel>::const_reference channel : animation.channels)
        {
            const animation::sampler& sampler = { animation.samplers.at(channel.sampler) };
            const vector<float>& timeline = { animation.timelines.at(sampler.input) };
            if (timeline.size() == 0)
            {
                continue;
            }
            float interpolation_factor = {};
            size_t keyframe_index = { indexof(timeline, time, interpolation_factor) };
            if (channel.target_path == "scale")
            {
                const vector<XMFLOAT3>& scales{ animation.scales.at(sampler.output) };
                XMStoreFloat3(&animated_nodes.at(channel.target_node).scale,
                    XMVectorLerp(XMLoadFloat3(&scales.at(keyframe_index + 0)),
                        XMLoadFloat3(&scales.at(keyframe_index + 1)), interpolation_factor));
            }
            else if (channel.target_path == "rotation")
            {
                const vector<XMFLOAT4>& rotations{ animation.rotations.at(sampler.output) };
                XMStoreFloat4(&animated_nodes.at(channel.target_node).rotation,
                    XMQuaternionNormalize(XMQuaternionSlerp(XMLoadFloat4(&rotations.at(keyframe_index + 0)),
                        XMLoadFloat4(&rotations.at(keyframe_index + 1)), interpolation_factor)));
            }
            else if (channel.target_path == "translation")
            {
                const vector<XMFLOAT3>& translations{ animation.translations.at(sampler.output) };
                XMStoreFloat3(&animated_nodes.at(channel.target_node).translation,
                    XMVectorLerp(XMLoadFloat3(&translations.at(keyframe_index + 0)),
                        XMLoadFloat3(&translations.at(keyframe_index + 1)), interpolation_factor));
            }
        }
        cumulate_transforms(animated_nodes);
    }
}
