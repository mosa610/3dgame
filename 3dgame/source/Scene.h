#pragma once
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/render_context.h"
#include "Graphics/Graphics.h"

//シーン
class Scene
{
public:
    Scene() 
    {
        Graphics& graphics = Graphics::Instance();
        Microsoft::WRL::ComPtr<ID3D11Device> device = graphics.Get_device();

        if (!scene_constant_buffer)
        {
            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.ByteWidth = sizeof(gbuffer_scene_constants);
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            buffer_desc.CPUAccessFlags = 0;
            buffer_desc.MiscFlags = 0;
            buffer_desc.StructureByteStride = 0;
            HRESULT hr = Graphics::Instance().Get_device()->CreateBuffer(&buffer_desc, nullptr, scene_constant_buffer.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }


        {
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            texture2d_desc.Width = graphics.Get_screen_width();
            texture2d_desc.Height = graphics.Get_screen_height();
            texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            texture2d_desc.MipLevels = 1;
            texture2d_desc.ArraySize = 1;
            texture2d_desc.SampleDesc.Count = 1;
            texture2d_desc.SampleDesc.Quality = 0;
            texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
            texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            texture2d_desc.CPUAccessFlags = 0;
            texture2d_desc.MiscFlags = 0;

            Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};

            HRESULT hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.ReleaseAndGetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, scene_render_target_view.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, scene_shader_resource_view.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }
    }
    virtual ~Scene() {
        if (scene_shader_resource_view) scene_shader_resource_view.Reset();
        if (scene_render_target_view) scene_render_target_view.Reset();
    }

    //初期化
    virtual void Initialize() = 0;

    //終了化
    virtual void Finalize() = 0;

    //更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    virtual void Render(float elapsedTime) = 0;

    virtual void DrawGUI() = 0;


    virtual void ResetShaderResource() = 0;

    virtual void RemakeShaderResource(float width, float height) = 0;

    void SetSceneConstant(int start_slot, DirectX::XMFLOAT2 viewport_size, bool is_update_resource);

    //準備が完了しているか
    bool IsReady() const { return ready; }

    //準備完了設定
    void SetReady() { ready = true; }

    Microsoft::WRL::ComPtr < ID3D11RenderTargetView> GetSceneRenderTargetView() { return scene_render_target_view; }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSceneShaderResourceView() { return scene_shader_resource_view; }
protected:

    float   timer = 0.0f;
    DirectX::XMFLOAT4X4 scene_view_projection;
    DirectX::XMFLOAT4X4 scene_previous_view_projection;

    Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> scene_render_target_view;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view;
private:
    bool    ready = false;

};