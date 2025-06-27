#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <wrl/client.h>
#include <d2d1_1.h>

class GameObject;

class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    UnityCamera m_UnityCamera;

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_SunBitmapPtr;
    ComPtr<ID2D1Bitmap1> m_StarBitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star2BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star3BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star4BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star5BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star6BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star7BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star8BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star9BitmapPtr;
    ComPtr<ID2D1Bitmap1> m_Star0BitmapPtr;


    void ProcessKeyboardEvents();

    void OnMouseLButtonDown(D2D1_POINT_2F pos) override;

    void OnMouseRButtonDown(D2D1_POINT_2F pos) override;

    void AddBoxObjects(D2D1_POINT_2F point);

    void ClearBoxObjects();

    void SelectBoxObject(D2D1_POINT_2F point);

    void SetBoxSelfRotation();

    void UpdateRelationship();

    std::list<GameObject*> m_GameObjects; // 선택된 박스 객체들
    GameObject* m_SelectedObject; // 선택된 박스 객체들

    int maxSize = 100;
};

