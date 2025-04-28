#include "KBloom.h"
#include "Graphics.h"
#include "framebuffer.h"

Bloom::Bloom()
{
    Graphics& graphics = Graphics::Instance();

    UINT width = graphics.Get_screen_width();
    UINT height = graphics.Get_screen_height();
    framebuffers[0] = std::make_unique<framebuffer>(graphics.Get_device(), width, height);
    framebuffers[1] = std::make_unique<framebuffer>(graphics.Get_device(), width / 2, height / 2);
}

Bloom::~Bloom()
{
}

void Bloom::Begin()
{
}

void Bloom::End()
{
}

void Bloom::Draw(ID3D11ShaderResourceView* srv)
{

}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ResizeSRV(ID3D11ShaderResourceView* srv,
    UINT Reduction_ratio)
{
    // ���̃��\�[�X���擾
    ID3D11Resource* originalResource = nullptr;
    srv->GetResource(&originalResource);

    // ���̃��\�[�X���e�N�X�`��2D�ł��邱�Ƃ��m�F
    ID3D11Texture2D* originalTexture = nullptr;
    HRESULT hr = originalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&originalTexture);
    originalResource->Release();
    if (FAILED(hr) || originalTexture == nullptr) {
        return nullptr; // �e�N�X�`��2D�łȂ��ꍇ�͏I��
    }

    // ���̃e�N�X�`���̃f�B�X�N���v�V�������擾
    D3D11_TEXTURE2D_DESC textureDesc = {};
    originalTexture->GetDesc(&textureDesc);

    // �V�����e�N�X�`���̃f�B�X�N���v�V������ݒ�
    textureDesc.Width = Graphics::Instance().Get_screen_width() / Reduction_ratio;
    textureDesc.Height = Graphics::Instance().Get_screen_height() / Reduction_ratio;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    ID3D11Texture2D* newTexture = nullptr;
    hr = Graphics::Instance().Get_device()->CreateTexture2D(&textureDesc, nullptr, &newTexture);
    if (FAILED(hr)) {
        originalTexture->Release();
        return nullptr; // �V�����e�N�X�`���̍쐬�Ɏ��s
    }

    // ���̃e�N�X�`������V�����e�N�X�`���ɃR�s�[ (���T�C�Y)
    Graphics::Instance().Get_device_context()->CopyResource(newTexture, originalTexture);

    // �V�����V�F�[�_�[���\�[�X�r���[���쐬
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resizedSRV;

    hr = Graphics::Instance().Get_device()->CreateShaderResourceView(newTexture, &srvDesc, resizedSRV.GetAddressOf());
    newTexture->Release();
    originalTexture->Release();

    return resizedSRV.Get();
}
