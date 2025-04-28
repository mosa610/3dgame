#pragma once

#include <DirectXMath.h>

//�J����
class Camera
{
private:
    Camera()   {}
    ~Camera()   {}
public:
    //�B��̃C���X�^���X�擾
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }

    //�w�����������
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
        const DirectX::XMFLOAT3& up);

    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
        const DirectX::XMFLOAT3& up, const DirectX::XMFLOAT3& position);

    //�p�[�X�y�N�e�B�u�ݒ�
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    //�r���[�s��擾
    const DirectX::XMFLOAT4X4& GetView() const { return view; }

    //�v���W�F�N�V�����s��擾
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

    //���_�擾
    const DirectX::XMFLOAT3& GetEye() const { return eye; }

    //�����_�擾
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }

    //������擾
    const DirectX::XMFLOAT3& GetUp() const { return up; }

    //�O�����擾
    const DirectX::XMFLOAT3& GetFront() const { return front; }

    //�E�����擾
    const DirectX::XMFLOAT3& GetRight() const { return right; }

    const void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    const float& GetNearClipDistance() const { return near_clip_distance; }

    const float& GetFarClipDistance() const { return far_clip_distance; }


private:
    DirectX::XMFLOAT4X4     view;
    DirectX::XMFLOAT4X4     projection;

    DirectX::XMFLOAT3       eye;
    DirectX::XMFLOAT3       focus;

    DirectX::XMFLOAT3       up;
    DirectX::XMFLOAT3       front;
    DirectX::XMFLOAT3       right;

    DirectX::XMFLOAT3       position;

    float near_clip_distance;
    float far_clip_distance;
};