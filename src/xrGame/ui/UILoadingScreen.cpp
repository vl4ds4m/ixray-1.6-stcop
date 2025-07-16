#include "StdAfx.h"
#include "UILoadingScreen.h"

#include "../../xrEngine/GameFont.h"
#include "../../xrUI/UIHelper.h"
#include "../../xrUI/UIXmlInit.h"

u32 calc_progress_color(u32 idx, u32 total, int stage, int max_stage, bool useLegacyCount);

UILoadingScreen::UILoadingScreen()
    : loadingProgressBackground(nullptr), loadingProgressPercent(nullptr),
      loadingLogo(nullptr), loadingStage(nullptr), loadingHeader(nullptr),
      loadingTipNumber(nullptr), loadingTip(nullptr), useLegacyProgressbarMode(false)
{
    UILoadingScreen::Initialize();
}

void UILoadingScreen::Initialize() 
{
    CUIXml uiXml;
    uiXml.Load(CONFIG_PATH, UI_PATH, "ui_mm_loading_screen.xml");

    const auto loadProgressBar = [&]() 
        {
        if (uiXml.NavigateToNode("loading_progress_background", 0))
            loadingProgressBackground = UIHelper::CreateStatic(uiXml, "loading_progress_background", this);

        loadingProgress = new CUIStatic();
        loadingProgress->SetAutoDelete(true);
        CUIXmlInit::InitStatic(uiXml, "loading_progress", 0, loadingProgress);
        AttachChild(loadingProgress);
        hShader = loadingProgress->GetShader();
        loadingProgress->Show(false);
      };

    const auto loadBackground = [&] { CUIXmlInit::InitWindow(uiXml, "background", 0, this); };

    useLegacyProgressbarMode = uiXml.ReadAttribInt("loading_progress", 0, "double_progress", 0) ? true : false;

    const auto node =
        uiXml.NavigateToNodeWithAttribute("loading_progress", "under_background", "0");
    if (node) {
        loadBackground();
        loadProgressBar();
    } else {
        loadProgressBar();
        loadBackground();
    }

    loadingLogo = UIHelper::CreateStatic(uiXml, "loading_logo", this);

    if (uiXml.NavigateToNode("loading_progress_percent", 0))
        loadingProgressPercent = UIHelper::CreateStatic(uiXml, "loading_progress_percent", this);

    loadingStage = UIHelper::CreateStatic(uiXml, "loading_stage", this);

    if (uiXml.NavigateToNode("loading_header", 0))
        loadingHeader = UIHelper::CreateStatic(uiXml, "loading_header", this);

    if (uiXml.NavigateToNode("loading_tip_number", 0))
        loadingTipNumber = UIHelper::CreateStatic(uiXml, "loading_tip_number", this);

    if (uiXml.NavigateToNode("loading_tip", 0))
        loadingTip = UIHelper::CreateStatic(uiXml, "loading_tip", this);
}

void UILoadingScreen::Update(const int stagesCompleted, const int stagesTotal) 
{
    m_load_stage = stagesCompleted;
    m_max_load_stage = stagesTotal;
    CUIWindow::Update();
    Draw();
}

void UILoadingScreen::ForceFinish() 
{
}

void UILoadingScreen::SetLevelLogo(const char* name) const { loadingLogo->InitTexture(name); }

void UILoadingScreen::SetStageTitle(const char* title) const {
    loadingStage->TextItemControl()->SetText(title);
}

void UILoadingScreen::SetStageTip(const char* header, const char* tipNumber,
                                  const char* tip) const {
    if (loadingHeader)
        loadingHeader->TextItemControl()->SetText(header);

    if (loadingTipNumber)
        loadingTipNumber->TextItemControl()->SetText(tipNumber);

    if (loadingTip)
        loadingTip->TextItemControl()->SetText(tip);
}

u32 calc_progress_color(u32 idx, u32 total, int stage, int max_stage, bool useLegacyCount)
{
    if (useLegacyCount && idx > (total / 2))
        idx = total - idx;

    float divide = useLegacyCount ? 2.0f : 1.0f;

    float kk = (float(stage + 1) / float(max_stage)) * (total/divide);
    float f = 1 / (exp((float(idx) - kk) * 0.5f) + 1.0f);

    return color_argb_f(f, 1.0f, 1.0f, 1.0f);
}

void UILoadingScreen::Draw()
{
    CUIWindow::Draw();

    u32 v_cnt = 40;
    UIRender->SetShader(*hShader);

    Fvector2						tsize;
    UIRender->GetActiveTextureResolution(tsize);

    UIRender->StartPrimitive(2*(v_cnt+1), IUIRender::ptTriStrip, UI().m_currentPointType);

    Frect back_coords = loadingProgress->GetWndRect();

    UI().ClientToScreenScaled(back_coords.lt, back_coords.x1, back_coords.y1);
    UI().ClientToScreenScaled(back_coords.rb, back_coords.x2, back_coords.y2);

    Frect back_tex_coords = loadingProgress->GetTextureRect();

    back_tex_coords.lt.x /= tsize.x;
    back_tex_coords.lt.y /= tsize.y;
    back_tex_coords.rb.x /= tsize.x;
    back_tex_coords.rb.y /= tsize.y;

    static float offs = -0.5f;
    float pos_delta = back_coords.width() / v_cnt;
    float tc_delta = back_tex_coords.width() / v_cnt;

    for (u32 idx = 0; idx < v_cnt + 1; ++idx)
    {
        u32 clr = calc_progress_color(idx, v_cnt, m_load_stage, m_max_load_stage, useLegacyProgressbarMode);
        UIRender->PushPoint(back_coords.lt.x + pos_delta * idx + offs, back_coords.rb.y + offs, 0 + EPS_S, clr, back_tex_coords.lt.x + tc_delta * idx, back_tex_coords.rb.y);
        UIRender->PushPoint(back_coords.lt.x + pos_delta * idx + offs, back_coords.lt.y + offs, 0 + EPS_S, clr, back_tex_coords.lt.x + tc_delta * idx, back_tex_coords.lt.y);
    }
    UIRender->FlushPrimitive();
}
