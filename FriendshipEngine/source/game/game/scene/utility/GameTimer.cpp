#include "pch.h"
#include "GameTimer.h"
#include <scene\SceneCommon.h>
#include <scene\SceneManager.h>
#include <../engine/engine/graphics/GraphicsEngine.h>

void GameTimer::Init()
{
    auto viewport = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
    myRenderedText = Text(L"berlinSans.ttf", FontSize_36);
    myRenderedText.SetPosition({ 0.85f * viewport.x, 0.95f * viewport.y });
    
    Vector4f beige = { 0.9529411792755127f, 0.8470588326454163f, 0.7137255072593689f, 1.f };
    Vector4f orange = { 0.8705882430076599f, 0.4431372582912445f, 0.1882352977991104f, 1.f };

    myRenderedText.SetColor(orange);
}

void GameTimer::Update(SceneUpdateContext& aContext) 
{
    if (myFirstStart) 
    {
        myFirstStart = false;
        start_time = std::chrono::steady_clock::now();
    }

    bool isCurrentlyPaused = aContext.sceneManager->GetIsPaused();

    if (!isCurrentlyPaused)
    {
        myTimer += aContext.dt;
        myRenderedText.SetText(Display_Elapsed_Time());
    }
}


void GameTimer::Render() 
{
    GraphicsEngine::GetInstance()->GetSpriteRenderer().DrawText(&myRenderedText);
}

void GameTimer::ResetTimer() 
{
    myTimer = 0;
}

std::string GameTimer::GetTimeAsString()
{
    return myRenderedText.GetText();
}

std::string GameTimer::Display_Elapsed_Time() const 
{
    double elapsed_seconds = myTimer;

    int minutes = static_cast<int>(elapsed_seconds / 60);
    int seconds = static_cast<int>(elapsed_seconds) % 60;
    int milliseconds = static_cast<int>((elapsed_seconds - static_cast<int>(elapsed_seconds)) * 1000);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":"
    << std::setw(2) << seconds << ":"
    << std::setw(3) << milliseconds;

    return ss.str();
}