#include "SystemSkymap.h"
#include "..//Graphics/Graphics.h"
#include "..//Graphics/PipelineManager.h"
#include "ComponentIBL.h"

void SystemSkymap::Initialize(Register& reg)
{

}

void SystemSkymap::update(Register& reg, float elapsed_time)
{
}

void SystemSkymap::render(Register& reg)
{
    // begin
    {
        D3D11_INPUT_ELEMENT_DESC input_element_desc[]
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        PipelineStateDesc desc;
        desc.name = "SkyBox"; // 識別用の名前に変更（数値IDは廃止）

        desc.vs_path = ".//Data//Shader//sky_box_vs.cso";
        desc.ps_path = ".//Data//Shader//sky_box_gbuffer_ps.cso";

        // 必要に応じて、ステートやトポロジを明示的に指定（任意）
        desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        desc.blend = BLEND_STATE::NONE;
        desc.depth = DEPTH_STATE::ZT_OFF_ZW_OFF; // スカイボックスは奥行き書き込みしないことが多いため例
        desc.raster = RASTER_STATE::CULL_NONE;
        desc.sampler = SAMPLER_STATE::ALL;

        // 入力レイアウトがある場合はセット
        desc.input_layout_desc.assign(std::begin(input_element_desc), std::end(input_element_desc));

        // Pipeline 登録
        PipelineManager::Instance().Add(desc, Graphics::Instance().Get_device());

        // Pipeline 使用
        PipelineManager::Instance().BindByName("SkyBox", Graphics::Instance().Get_device_context());
    }
    ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
    // draw
    {
        float screenWidth = Graphics::Instance().Get_screen_width();
        float screenHeight = Graphics::Instance().Get_screen_height();
        for(Entity e : reg.view<ComponentIBL>()) {
            auto& skymap = reg.getComponent<ComponentIBL>(e);
            skymap.skymap_sprite->render(dc, 0, 0, screenWidth, screenHeight);
        }
    }

    // end
    {
        dc->VSSetShader(nullptr, nullptr, 0);
        dc->GSSetShader(nullptr, nullptr, 0);
        dc->PSSetShader(nullptr, nullptr, 0);
        dc->IASetInputLayout(nullptr);
    }
}

void SystemSkymap::finalize(Register& reg)
{
}
