#include <vcl.h>
#pragma hdrstop

#include "Ctrl_captcha.h"
#include "Tmainform.h"
#include "Ctrl_interface.h"
#include "Gfx_captcha.h"
#include "Packetoperations.h"
#include <stdlib.h>

#pragma package(smart_init)

CaptchaController::CaptchaController()
{
    window_x = 190;
    window_y = 80;
    is_dragging = false;
    is_visible = false;
    ok_hovered = false;
}

CaptchaController::~CaptchaController()
{
}

// Steps the captcha's animation: new letter heights every 40 frames and two letters hidden in turn.
void CaptchaController::Update()
{
    if (is_visible)
    {
        MainForm->redraw_needed = true;
        frame++;
        if (frame > 139)
            frame = 0;
        if (frame % 40 == 0)
        {
            for (int i = 0; i < 7; i++)
                letter_offsets[i] = random(5);
        }
        hide_counter++;
        if (hide_counter > 10)
        {
            hide_counter = 0;
            hidden_letter = random(5) + 1;
            second_hidden_letter = random(5) + 1;
        }
    }
}

// Draws the captcha: a random noise strip, the swaying letters except the two hidden ones, and now and then a faded
// decoy letter.
void CaptchaController::Draw()
{
    int i;
    int decoy_slot;
    int x = window_x + 16;
    int y = window_y + 72;
    int sway = frame / 10;
    if (sway > 6)
        sway = 12 - sway;
    sway += 8;
    MainForm->OffscreenSurface->BltFast(x, y, MainForm->gfx->captcha->letters_surface,
        &MainForm->gfx->captcha->GetNoiseRect(random(160)), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    for (i = 1; i <= answer.Length(); i++)
    {
        char letter = answer[i];
        if (i != hidden_letter && i != second_hidden_letter)
            MainForm->OffscreenSurface->BltFast(x + sway + 34 * i - 34, (unsigned char)letter_offsets[i] + y - 1,
                MainForm->gfx->captcha->noise_surface, &MainForm->gfx->captcha->GetLetterRect(letter),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    decoy_counter++;
    if ((int)(random(4) + 12) < decoy_counter)
    {
        decoy_slot = random(4);
        char decoy = (char)random(24) + 'A';
        MainForm->blitter->AlphaBlt(34 * decoy_slot + x + 17, random(2) + y + 1, MainForm->OffscreenSurface,
            MainForm->gfx->captcha->noise_surface, &MainForm->gfx->captcha->GetLetterRect(decoy), 180);
        if (decoy_counter > 16)
            decoy_counter = 0;
    }
}

bool CaptchaController::Show(int new_captcha_id, int new_reward)
{
    return Open(new_captcha_id, "", new_reward);
}

// Five random capital letters from A to X.
String CaptchaController::RandomAnswer()
{
    String text = "NOYES";
    for (int i = 1; i <= 5; i++)
        text[i] = (char)random(24) + 'A';
    return text;
}

// Opens a captcha at a random place with a new shift and its answer box; false when its pictures cannot be loaded.
bool CaptchaController::Open(int new_captcha_id, String new_answer, int new_reward)
{
    if (is_visible)
        Close();
    // The window's place, its frame and the hidden letters are picked at random.
    randomize();
    window_x = random(20) + 200;
    window_y = random(16) + 60;
    frame = 10 * random(4);
    shift = random(20);
    hidden_letter = random(5) + 1;
    second_hidden_letter = random(5) + 1;
    for (int i = 0; i < 7; i++)
        letter_offsets[i] = random(5);
    SetAnswer(new_captcha_id, new_answer);
    reward = new_reward;
    is_visible = true;
    MainForm->redraw_needed = true;
    MainForm->edits->AddEdit(EditId_Captcha, EditGroup_Game, window_x + 118, window_y + 116, 100, EDIT_LINE_HEIGHT);
    MainForm->edits->SetBackgroundVisible(EditId_Captcha, false);
    MainForm->edits->SetBorderVisible(EditId_Captcha, false);
    MainForm->edits->SetTextColor(EditId_Captcha, MainForm->themes->color_popup_edits);
    MainForm->edits->SetMaxLength(EditId_Captcha, 5);
    MainForm->edits->SetCaseMode(EditId_Captcha, EditCaseMode_Upper);
    MainForm->edits->SetAllowSpaces(EditId_Captcha, false);
    MainForm->edits->SetCharFilter(EditId_Captcha, EditCharFilter_Letters);
    MainForm->edits->FocusEdit(EditId_Captcha);
    MainForm->gfx->CreateCaptchaGraphics();
    if (MainForm->gfx->LoadCaptchaGraphics())
    {
        music_paused = false;
        return true;
    }
    music_paused = true;
    return false;
}

// Sets the captcha's answer, a random one when the server sent none, and with it how hard it is.
void CaptchaController::SetAnswer(int id, String text)
{
    if (text.Length() < 5)
    {
        answer = RandomAnswer();
        difficulty = 101 - (char)random(100);
    }
    else
    {
        answer = UpperCase(text);
        difficulty = (char)random(100) + 100;
    }
    shifted_answer = answer;
    for (int i = 1; i < 6; i++)
        shifted_answer[i] = shifted_answer[i] - shift;
    captcha_id = id;
    misses = 0;
}

// Closes the captcha, its answer box and pictures, giving the focus back to chat.
void CaptchaController::Close()
{
    MainForm->edits->RemoveEdit(EditId_Captcha);
    MainForm->edits->FocusEdit(EditId_Chat);
    MainForm->gfx->FreeCaptchaGraphics();
    is_visible = false;
    MainForm->redraw_needed = true;
}

// Checks the five typed characters against the captcha: a right answer is sent when it earns a reward or the captcha
// is a hard one, and an easy captcha closes; every fifth wrong answer brings a new one.
void CaptchaController::Submit()
{
    String typed = MainForm->edits->GetText(EditId_Captcha);
    if (typed.Length() < 5)
    {
        MainForm->sfx->PlaySfx(SfxSound_Blocked);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            "Please enter the 5 characters of the captcha.");
        return;
    }
    String shifted = typed;
    for (int i = 1; i < 6; i++)
        shifted[i] = typed[i] - shift;
    if (shifted_answer == shifted)
    {
        if (difficulty < 102)
        {
            if (reward > 0)
                MainForm->net_client->sender->SendCaptchaAnswer(captcha_id, typed);
            MainForm->sfx->PlaySfx(SfxSound_Accept);
            Close();
        }
        else
            MainForm->net_client->sender->SendCaptchaAnswer(captcha_id, typed);
    }
    else
    {
        MainForm->edits->SetText(EditId_Captcha, "");
        if (misses < 4)
        {
            misses++;
            MainForm->sfx->PlaySfx(SfxSound_Blocked);
        }
        else
        {
            MainForm->sfx->PlaySfx(SfxSound_Failed);
            misses = 0;
            if (difficulty < 102)
            {
                answer = RandomAnswer();
                for (int k = 1; k < 6; k++)
                    shifted_answer[k] = shifted_answer[k] - shift;
            }
            else
                MainForm->net_client->sender->RequestNewCaptcha(captcha_id);
        }
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            "Wrong captcha answer, please try again.");
    }
}

// A click on the ok button sends the answer; any other click in the window starts moving it.
void CaptchaController::MouseDown(int x, int y, int button)
{
    if (x >= window_x + 116 && x <= window_x + 206 && y >= window_y + 144 && y <= window_y + 175)
        Submit();
    else if (window_x <= x && x <= window_x + 225 && y >= window_y && y <= window_y + 185)
    {
        is_dragging = true;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Moves a dragged window with its answer box, kept on screen, and tracks the ok button.
void CaptchaController::MouseMove(int x, int y)
{
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 404)
            window_x = 404;
        if (window_y >= 123)
            window_y = 123;
        MainForm->edits->SetPosition(EditId_Captcha, window_x + 118, window_y + 116);
        MainForm->redraw_needed = true;
    }
    else if (x >= window_x + 116 && x <= window_x + 206 && y >= window_y + 144 && y <= window_y + 175)
    {
        if (!ok_hovered)
        {
            ok_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (ok_hovered)
    {
        ok_hovered = false;
        MainForm->redraw_needed = true;
    }
}

void CaptchaController::MouseUp()
{
    is_dragging = false;
}
