#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

class GameObject
{
    GameObject() = delete;
    GameObject(const GameObject&) = delete;
    void operator=(const GameObject&) = delete;

public:
    GameObject(ComPtr<ID2D1Bitmap1>& bitmap)
    {
        m_BitmapPtr = bitmap;

        ++s_id;
        m_name += std::to_wstring(s_id); // ID�� �̸��� �߰�

        m_renderTM = MYTM::MakeRenderMatrix(true);

        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

        //m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
        //m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
    }

    GameObject(ComPtr<ID2D1Bitmap1>& bitmap, float width, float height)
    {
        m_BitmapPtr = bitmap;

        ++s_id;
        m_name += std::to_wstring(s_id); // ID�� �̸��� �߰�

        m_renderTM = MYTM::MakeRenderMatrix(true);

        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

        m_transform.SetScale(Vec2(width/100, height/100));

        //m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
        //m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
    }

    ~GameObject() = default;

    void Update(float deltaTime)
    {
        if (m_isSelfRotation)
        {
            m_transform.Rotate(deltaTime * 36.f * rotateSpeed); // �ڱ� ȸ��
        }
    }

    void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
    {
        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        D2D1::ColorF boxColor = D2D1::ColorF::LightGray;

        if(m_isSelected) boxColor = D2D1::ColorF::Red;

        testRender.SetTransform(finalTM);
        //testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

        D2D1_RECT_F dest = D2D1::RectF(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

        testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        //testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
    }

    void SetPosition(const Vec2& pos)
    {
        m_transform.SetPosition(pos);
    }

    //Vec2 GetPosition()
    //{
    //    return 
    //}

    void Move(const Vec2& offset)
    {
        m_transform.Translate(offset);
    }

    void Rotate(float angle)
    {
        m_transform.Rotate(angle);
    }

    void ToggleSelected()
    {
        m_isSelected = !m_isSelected;
    }

    bool IsSelected() const
    {
        return m_isSelected;
    }

    void ToggleSelfRotation()
    {
        m_isSelfRotation = !m_isSelfRotation;
    }

    bool IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM)
    {
        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        finalTM.Invert();

        // 2) ���� ��ǥ�� ����Ʈ ��ȯ
        D2D1_POINT_2F localPt = finalTM.TransformPoint(worldPoint);

        // 3) ���� �簢�� ����
        // (0,0) ~ (width, height) ������ �ִٸ� ��Ʈ!
        // m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

        //std::cout << "BoxObject::IsHitTest: localPt = ("
        //    << localPt.x << ", " << localPt.y << ")" << std::endl;

        //std::cout << "BoxObject::IsHitTest: m_rect = ("
        //    << m_rect.left << ", " << m_rect.top << ", "
        //    << m_rect.right << ", " << m_rect.bottom << ")" << std::endl;

        // 4) ���� �������� �˻�
        return MYTM::IsPointInRect(localPt, m_rect);
    }

    D2DTM::Transform* GetTransform()
    {
        return &m_transform;
    }



    void SetParent(GameObject* parent)
    {
        assert(parent != nullptr);

        if (nullptr != m_transform.GetParent())
        {
            // �̹� �θ� �ִٸ� �θ� ���踦 �����մϴ�.
            m_transform.DetachFromParent();
        }

        m_transform.SetParent(parent->GetTransform());
    }

    void DetachFromParent()
    {
        m_transform.DetachFromParent();
    }

    void SetLeader(bool isLeader)
    {
        m_isLeader = isLeader;
    }

    float rotateSpeed = 0;

private:
    D2DTM::Transform m_transform;

    MAT3X2F m_renderTM; // ������ ��ȯ ���

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false; // ���� �ڽ� ����

    bool m_isSelfRotation = false; // �ڱ� ȸ�� ����

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    static int s_id; // static ��� ������ ID�� �����մϴ�. (������)

};

int GameObject::s_id = 0; // static ��� ���� �ʱ�ȭ




inline D2D1_POINT_2F ToFloatPoint(const MouseState& ms)
{
    return { static_cast<float>(ms.pos.x),
             static_cast<float>(ms.pos.y) };
}

TransformPracticeScene::~TransformPracticeScene()
{
    /*for (int i = m_GameObjects.size() - 1; i >= 0; i--)
        delete (m_GameObjects.front() + i);

    m_GameObjects.clear();*/

    ClearBoxObjects();
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"������ �¾�踦 ����� �ּ���. ���� ��Ģ�� ���� �մϴ�. ^^;;");

    std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¿��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�."<< std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;


    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/cat.png", *m_BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/sun.png", *m_SunBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/star.png", *m_StarBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/aqua_star.png", *m_Star2BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/neo_star.png", *m_Star3BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/floria_star.png", *m_Star4BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/ripple_star.png", *m_Star5BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/shiver_star.png", *m_Star6BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/shiver_star2.png", *m_Star7BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/rock_star.png", *m_Star8BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/volcano_star.png", *m_Star9BitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/kirby.png", *m_Star0BitmapPtr.GetAddressOf());



    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }

    GameObject* pNewBox = new GameObject(m_SunBitmapPtr);

    pNewBox->SetPosition(Vec2(-50, 50));

    pNewBox->ToggleSelfRotation();

    pNewBox->rotateSpeed = 0.5f;

    m_GameObjects.push_back(pNewBox);

    {
        GameObject* pChildBox = new GameObject(m_StarBitmapPtr, 30, 30);

        pChildBox->SetPosition(Vec2(50, 50));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star2BitmapPtr, 35, 35);

        pChildBox->SetPosition(Vec2(-80, 80));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star3BitmapPtr, 50, 50);

        pChildBox->SetPosition(Vec2(120, -120));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star4BitmapPtr, 60, 60);

        pChildBox->SetPosition(Vec2(-150, 150));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star5BitmapPtr, 70, 70);

        pChildBox->SetPosition(Vec2(200, 200));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star6BitmapPtr, 65, 65);

        pChildBox->SetPosition(Vec2(-240, -240));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);

        GameObject* pCChildBox = new GameObject(m_Star7BitmapPtr, 35, 35);

        pCChildBox->SetPosition(Vec2(-220, -260));

        pCChildBox->ToggleSelfRotation();

        pCChildBox->SetParent(pChildBox);

        pCChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pCChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star8BitmapPtr, 70, 70);

        pChildBox->SetPosition(Vec2(270, -270));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star9BitmapPtr, 80, 80);

        pChildBox->SetPosition(Vec2(-300, 300));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }

}

void TransformPracticeScene::Tick(float deltaTime)
{
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    for (auto& box : m_GameObjects)
    {
        //if (box == m_GameObjects.front())
        //    continue;
        box->Update(deltaTime);

        //if (box != m_GameObjects.front())
        //    continue;

        //D2D1_POINT_2F point;
        //point.x = box->GetTransform()->GetPosition().x;
        //point.y = box->GetTransform()->GetPosition().y;
        //box->GetTransform()->Translate(D2DTM::Transform::Vec2());
        //point = box->GetTransform()->GetInverseWorldMatrix().TransformPoint(point);

        //float dx = 400 - point.x;
        //float dy = 400 - point.y;

        //float total = sqrt(dx * dx + dy * dy);

        //dx = dx / total;
        //dy = dy / total;

        //point.x += dx;
        //point.y += dy;

        //point = box->GetTransform()->GetWorldMatrix().TransformPoint(point);

        ///*box->GetTransform()->Translate(D2DTM::Transform::Vec2(point.x, point.y));*/
        //box->SetPosition(Vec2((point.x), (point.y)));
    }

    // ī�޶� ������Ʈ

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // ī�޶� ��ġ ������ ��Ʈ����

    MAT3X2F finalTM = renderTM * cameraTM;

    // ������

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    //wchar_t buffer[128] = L"";
    //MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    //// ī�޶� ��ġ ǥ��
    //globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    //globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    //globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

    for (auto& box : m_GameObjects)
    {
        box->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();



  
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
}

void TransformPracticeScene::OnMouseLButtonDown(D2D1_POINT_2F pos)
{
    AddBoxObjects(pos);
}

void TransformPracticeScene::OnMouseRButtonDown(D2D1_POINT_2F pos)
{
    D2D1_POINT_2F point;
    point.x = m_GameObjects.front()->GetTransform()->GetPosition().x;
    point.y = m_GameObjects.front()->GetTransform()->GetPosition().y;
    m_GameObjects.front()->GetTransform()->Translate(D2DTM::Transform::Vec2());

    point = m_GameObjects.front()->GetTransform()->GetWorldMatrix().TransformPoint(point);

    std::cout << point.x << ' ' << point.y << std::endl;

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    m_SelectedObject = nullptr;
    for (auto& box : m_GameObjects)
    {
        if (box->IsSelected()) 
        {
            box->ToggleSelected();
        }
    }
    for (auto& box : m_GameObjects)
    {
        if (box->IsHitTest(pos, cameraTM))
        {
            box->ToggleSelected();

            m_SelectedObject = box; // ���� ���õ� �ڽ� �߰�
            break;

        }
    }

    // ���� ���� ������Ʈ
    UpdateRelationship();
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
    // Ŭ����
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        ClearBoxObjects();
        m_SelectedObject = nullptr;
    }



    // ī�޶� �̵� ó��, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17���ʹ� structured binding�� ����Ͽ� �� �����ϰ� ǥ���� �� �ֽ��ϴ�.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    //// ù��° ���õ� �ڽ��� �̵�
    //static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
    //  { 'D', {  1.f,  0.f } }, // DŰ�� ������ �̵�
    //  { 'A', { -1.f,  0.f } }, // AŰ�� ���� �̵�
    //  { 'W', {  0.f,  1.f } }, // WŰ�� ���� �̵�
    //  { 'S', {  0.f, -1.f } }, // SŰ�� �Ʒ��� �̵�
    //};

    //for (auto& [vk, dir] : kBoxMoves)
    //{
    //    if (InputManager::Instance().GetKeyDown(vk))
    //    {
    //        m_SelectedBoxObjects.front()->Move(dir);
    //    }
    //}

    //// ù��° ���õ� �ڽ��� ȸ��
    //if (InputManager::Instance().GetKeyDown(VK_SPACE) && !m_SelectedBoxObjects.empty())
    //{
    //    m_SelectedBoxObjects.front()->Rotate(1.f); // ���� ������ ȸ��
    //}
}

void TransformPracticeScene::AddBoxObjects(D2D1_POINT_2F point)
{
    if (m_SelectedObject == nullptr)
    {
        return;
    }
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    cameraTM.Invert();

    D2D1_POINT_2F worldPt = cameraTM.TransformPoint(point);

    GameObject* pNewBox = new GameObject(m_Star0BitmapPtr);

    pNewBox->SetPosition(Vec2(worldPt.x, worldPt.y));

    pNewBox->ToggleSelfRotation();

    pNewBox->rotateSpeed = 0.5f;

    m_GameObjects.push_back(pNewBox);

    if (m_SelectedObject != nullptr)
    {
        pNewBox->SetParent(m_SelectedObject);
    }
}

void TransformPracticeScene::ClearBoxObjects()
{
    for (auto& box : m_GameObjects)
    {
        delete box;
    }
    m_GameObjects.clear();



    GameObject* pNewBox = new GameObject(m_SunBitmapPtr);

    pNewBox->SetPosition(Vec2(-50, 50));

    pNewBox->ToggleSelfRotation();

    pNewBox->rotateSpeed = 0.5f;

    m_GameObjects.push_back(pNewBox);

    {
        GameObject* pChildBox = new GameObject(m_StarBitmapPtr, 30, 30);

        pChildBox->SetPosition(Vec2(50, 50));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star2BitmapPtr, 35, 35);

        pChildBox->SetPosition(Vec2(-80, 80));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star3BitmapPtr, 50, 50);

        pChildBox->SetPosition(Vec2(120, -120));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star4BitmapPtr, 60, 60);

        pChildBox->SetPosition(Vec2(-150, 150));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star5BitmapPtr, 70, 70);

        pChildBox->SetPosition(Vec2(200, 200));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star6BitmapPtr, 65, 65);

        pChildBox->SetPosition(Vec2(-240, -240));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);

        GameObject* pCChildBox = new GameObject(m_Star7BitmapPtr, 35, 35);

        pCChildBox->SetPosition(Vec2(-220, -260));

        pCChildBox->ToggleSelfRotation();

        pCChildBox->SetParent(pChildBox);

        pCChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pCChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star8BitmapPtr, 70, 70);

        pChildBox->SetPosition(Vec2(270, -270));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
    {
        GameObject* pChildBox = new GameObject(m_Star9BitmapPtr, 80, 80);

        pChildBox->SetPosition(Vec2(-300, 300));

        pChildBox->ToggleSelfRotation();

        pChildBox->SetParent(pNewBox);

        pChildBox->rotateSpeed = 1;

        m_GameObjects.push_back(pChildBox);
    }
}

void TransformPracticeScene::SelectBoxObject(D2D1_POINT_2F point)
{
}

void TransformPracticeScene::SetBoxSelfRotation()
{
}

void TransformPracticeScene::UpdateRelationship()
{
}

