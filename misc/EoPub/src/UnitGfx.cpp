#include <vcl.h>
#pragma hdrstop

#include "UnitGfx.h"
#include "UnitMain.h"
#include <Graphics.hpp>
#include <io.h>
#include <time.h>

#pragma package(smart_init)
#pragma link "ElScrollBar"
#pragma resource "*.dfm"

TFormGfx *FormGfx;

// Starts with room for 1000 graphics, five to a row.
__fastcall TFormGfx::TFormGfx(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    graphic_count = 1000;
    row_size = 5;
    int scroll_max = graphic_count / row_size;
    ElScrollBar1->Max = scroll_max;
    panels_pending = true;
    first_show = true;
    own_library = false;
}

// Themes the form and gives every frame the same background, its corner colour made see-through once.
void __fastcall TFormGfx::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
        imageGfxArrow->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Image2->Picture->Bitmap = Image20->Picture->Bitmap;
        Image4->Picture->Bitmap = Image20->Picture->Bitmap;
        Image6->Picture->Bitmap = Image20->Picture->Bitmap;
        Image8->Picture->Bitmap = Image20->Picture->Bitmap;
        Image10->Picture->Bitmap = Image20->Picture->Bitmap;
        Image12->Picture->Bitmap = Image20->Picture->Bitmap;
        Image14->Picture->Bitmap = Image20->Picture->Bitmap;
        Image16->Picture->Bitmap = Image20->Picture->Bitmap;
        Image18->Picture->Bitmap = Image20->Picture->Bitmap;
    }
    else
    {
        imageGfxArrow->Picture->Bitmap = FormMain->imageChangeItemView->Picture->Bitmap;
        Image4->Picture->Bitmap = Image2->Picture->Bitmap;
        Image6->Picture->Bitmap = Image2->Picture->Bitmap;
        Image8->Picture->Bitmap = Image2->Picture->Bitmap;
        Image10->Picture->Bitmap = Image2->Picture->Bitmap;
        Image12->Picture->Bitmap = Image2->Picture->Bitmap;
        Image14->Picture->Bitmap = Image2->Picture->Bitmap;
        Image16->Picture->Bitmap = Image2->Picture->Bitmap;
        Image18->Picture->Bitmap = Image2->Picture->Bitmap;
        Image20->Picture->Bitmap = Image2->Picture->Bitmap;
    }
    // The see-through colour is taken once, from inside the first frame.
    if (first_show)
    {
        Image2->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image2->Transparent = true;
        Image4->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image4->Transparent = true;
        Image6->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image6->Transparent = true;
        Image8->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image8->Transparent = true;
        Image10->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image10->Transparent = true;
        Image12->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image12->Transparent = true;
        Image14->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image14->Transparent = true;
        Image16->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image16->Transparent = true;
        Image18->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image18->Transparent = true;
        Image20->Picture->Bitmap->TransparentColor = Image2->Picture->Bitmap->Canvas->Pixels[30][30];
        Image20->Transparent = true;
        first_show = false;
    }
}

// Opens the numbered gfx file itself for viewing; false when the file is not there.
bool TFormGfx::OpenLibrary(int number, int first, int step, TElTreeStringGrid *grid, int column)
{
    own_library = true;
    library_file = LibraryFileName(number);
    String path = FormMain->config->GetGfxPath() + library_file;
    if (access(path.c_str(), 0))
        return false;
    opened_library = LoadLibraryA(path.c_str());
    labelGfxCaption->Hint = "Viewed: " + LibraryFileName(number);
    int bitmap_total = FormMain->gfx_vault->CountBitmaps(opened_library);
    int bitmap_count = bitmap_total - 100;
    bitmap_count = bitmap_count / step;
    SetLibrary(opened_library, first, step, bitmap_count, grid, column);
    return true;
}

String TFormGfx::LibraryFileName(int number)
{
    String number_text = IntToStr(number);
    if (number < 10)
        number_text = "0" + IntToStr(number);
    return "gfx0" + number_text + ".egf";
}

// The library, numbering and grid the viewer shows; sets how far it scrolls.
void TFormGfx::SetLibrary(HINSTANCE__ *library, int first, int step, int count, TElTreeStringGrid *grid, int column)
{
    scroll_ready = false;
    usage_grid = grid;
    view_library = library;
    usage_column = column;
    first_resource = first;
    graphic_count = count;
    resource_step = step;
    draw_mode = 0;
    last_row = graphic_count / row_size;
    last_row -= 2;
    if (count % row_size != 0)
        last_row++;
    ElScrollBar1->Max = last_row;
}

void TFormGfx::SetDrawMode(char mode)
{
    draw_mode = mode;
}

// Shows the viewer over the owner, scrolled to the page that holds the value, and waits for a choice.
void TFormGfx::ShowCaptions(TForm *owner, String gfx_caption, String name_caption, int value)
{
    labelGfxCaption->Caption = gfx_caption;
    labelNameCaption->Caption = name_caption;
    if (!own_library)
        labelGfxCaption->Hint = String("");
    imageGfxArrow->Left = labelGfxCaption->Left + labelGfxCaption->Width + 8;
    labelNameCaption->Left = imageGfxArrow->Left + imageGfxArrow->Width + 8;
    if (value > 0)
        value--;
    scroll_row = value / row_size;
    if (scroll_row >= last_row)
        scroll_row = last_row;
    first_shown = scroll_row * row_size;
    ShowPage(first_shown + 1);
    ElScrollBar1->Position = scroll_row;
    labelMaxGfxIDS->Caption = "TOTAL: " + IntToStr(graphic_count);
    Left = (owner->Width - Width) / 2 + owner->Left;
    Top = (owner->Height - Height) / 2 + owner->Top;
    ShowPanels(true);
    chosen_number = IntToStr(value);
    chosen_size = -1;
    wheel_time = clock();
    scroll_ready = true;
    ShowModal();
}

// Shows or hides the cover panels of the ten frames; showing them starts the timer once.
void TFormGfx::ShowPanels(bool on)
{
    if (on)
    {
        if (!panels_pending)
            return;
        panels_pending = false;
        Timer1->Enabled = true;
    }
    Panel22->Visible = on;
    Panel23->Visible = on;
    Panel24->Visible = on;
    Panel25->Visible = on;
    Panel26->Visible = on;
    Panel27->Visible = on;
    Panel28->Visible = on;
    Panel29->Visible = on;
    Panel30->Visible = on;
    Panel31->Visible = on;
}

// Shows ten graphics from the number on.
void TFormGfx::ShowPage(int first)
{
    page_library = view_library;
    ShowGraphic(Image1, Panel2, Label1, first++);
    ShowGraphic(Image3, Panel4, Label2, first++);
    ShowGraphic(Image5, Panel6, Label3, first++);
    ShowGraphic(Image7, Panel8, Label4, first++);
    ShowGraphic(Image9, Panel10, Label5, first++);
    ShowGraphic(Image11, Panel12, Label6, first++);
    ShowGraphic(Image13, Panel14, Label7, first++);
    ShowGraphic(Image15, Panel16, Label8, first++);
    ShowGraphic(Image17, Panel18, Label9, first++);
    ShowGraphic(Image19, Panel20, Label10, first++);
}

// Draws one layer of a spell effect's first frame when the effect has that layer; true when it drew.
char TFormGfx::DrawFrame(TImage *image, int index, int frame, int resource, char drawn)
{
    Graphics::TBitmap *bitmap;
    Graphics::TBitmap *layer_bitmap;
    char has_base = FormMain->magic_mapper->base_layer[index - 1];
    char has_blend = FormMain->magic_mapper->blend_layer[index - 1];
    char has_solid = FormMain->magic_mapper->solid_layer[index - 1];
    int frame_count = FormMain->magic_mapper->GetFrameCount(index - 1);
    if ((!resource && has_base) || (resource == 1 && has_blend) || (resource == 2 && has_solid))
    {
        try
        {
            int effect_offset = 3 * index - 3;
            int resource_id = effect_offset + 101;
            HINSTANCE__ *library = page_library;
            if (!drawn)
            {
                image->Picture->Bitmap->Width = 0;
                image->Picture->Bitmap->Height = 0;
                image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id + resource);
                bitmap = image->Picture->Bitmap;
                bitmap->Width = image->Picture->Bitmap->Width / frame_count;
                return 1;
            }
            // Later layers are drawn over the first, skipping their background colour.
            layer_bitmap = new Graphics::TBitmap();
            layer_bitmap->LoadFromResourceID((unsigned int)library, resource_id + resource);
            layer_bitmap->Width = layer_bitmap->Width / frame_count;
            int key_color = layer_bitmap->Canvas->Pixels[0][0];
            for (int x = 0; x < layer_bitmap->Width; x++)
            {
                for (int y = 0; y < layer_bitmap->Height; y++)
                {
                    if (layer_bitmap->Canvas->Pixels[x][y] != key_color)
                        image->Picture->Bitmap->Canvas->Pixels[x][y] = layer_bitmap->Canvas->Pixels[x][y];
                }
            }
        }
        catch (...)
        {
        }
        return 1;
    }
    return 0;
}

bool TFormGfx::LoadImage(TImage *image, int index, int resource)
{
    try
    {
        if (!draw_mode)
            image->Picture->Bitmap->LoadFromResourceID((unsigned int)page_library, resource);
        if (draw_mode == 1)
        {
            char layer_drawn = 0;
            layer_drawn = DrawFrame(image, index, resource, 0, layer_drawn);
            layer_drawn = DrawFrame(image, index, resource, 1, layer_drawn);
            layer_drawn = DrawFrame(image, index, resource, 2, layer_drawn);
            image->Picture->Bitmap->TransparentColor = clBlack;
            image->Picture->Bitmap->Transparent = true;
            image->Visible = true;
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

// Shows the numbered graphic in its frame with its resource ID, and a hint of where it is used.
bool TFormGfx::ShowGraphic(TImage *image, TPanel *panel, TLabel *label, int number)
{
    if (number < 1)
        number = 1;
    int resource_id = (number - 1) * resource_step + first_resource;
    image->Picture->Bitmap->TransparentColor = clBlack;
    image->Picture->Bitmap->Transparent = true;
    panel->Caption = number;
    label->Caption = "res.id:" + String(resource_id);
    if (!LoadImage(image, number, resource_id))
    {
        image->Visible = false;
        label->Caption = "error: " + String(resource_id);
        label->Font->Color = clRed;
        return false;
    }
    image->Visible = true;
    label->Font->Color = (TColor)0x808040;
    image->Width = 2 * image->Picture->Bitmap->Width;
    image->Height = 2 * image->Picture->Bitmap->Height;
    image->Left = (Panel1->Width - image->Width) / 2;
    image->Top = (Panel1->Height - image->Height) / 2;
    image->Top = Panel2->Height / 2 + image->Top;
    if (!own_library)
        image->Hint = GraphicHint(number, usage_grid, usage_column);
    else
        image->Hint = "resource id: " + IntToStr(number);
    return true;
}

String TFormGfx::GraphicHint(int number, TElTreeStringGrid *grid, int column)
{
    String hint = "";
    int use_count = 0;
    int name_column = 0;
    for (int i = 1; i < grid->HeaderSections->Count; i++)
    {
        if (String(grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_column = i - 1;
            break;
        }
    }
    for (int row_index = 0; row_index < grid->Items->Count; row_index++)
    {
        if (grid->Items->Item[row_index]->ColumnText->Count != 0)
        {
            if (grid->Items->Item[row_index]->ColumnText->Strings[column] == IntToStr(number))
            {
                hint += "\n - " + grid->Items->Item[row_index]->Text;
                hint += " : ";
                hint += grid->Items->Item[row_index]->ColumnText->Strings[name_column];
                use_count++;
            }
        }
    }
    if (hint.Length() > 0)
        hint = "Gfx " + IntToStr(use_count) + "x used in items:" + hint;
    else
        hint = "Never used";
    return hint;
}

// Takes the first graphic of the page as the choice.
void __fastcall TFormGfx::Panel1Click(TObject *Sender)
{
    ShowBigImage(Panel2, Image1);
}

void __fastcall TFormGfx::Image3Click(TObject *Sender)
{
    ShowBigImage(Panel4, Image3);
}

void __fastcall TFormGfx::Panel5Click(TObject *Sender)
{
    ShowBigImage(Panel6, Image5);
}

void __fastcall TFormGfx::Panel7Click(TObject *Sender)
{
    ShowBigImage(Panel8, Image7);
}

void __fastcall TFormGfx::Panel9Click(TObject *Sender)
{
    ShowBigImage(Panel10, Image9);
}

void __fastcall TFormGfx::Panel11Click(TObject *Sender)
{
    ShowBigImage(Panel12, Image11);
}

void __fastcall TFormGfx::Panel13Click(TObject *Sender)
{
    ShowBigImage(Panel14, Image13);
}

void __fastcall TFormGfx::Panel15Click(TObject *Sender)
{
    ShowBigImage(Panel16, Image15);
}

void __fastcall TFormGfx::Image17Click(TObject *Sender)
{
    ShowBigImage(Panel18, Image17);
}

void __fastcall TFormGfx::Panel19Click(TObject *Sender)
{
    ShowBigImage(Panel20, Image19);
}

// Takes the clicked graphic as the choice, with an inventory size worked out from its width and height.
void TFormGfx::ShowBigImage(TPanel *panel, TImage *image)
{
    chosen_number = panel->Caption;
    chosen_width = image->Picture->Bitmap->Width;
    chosen_height = image->Picture->Bitmap->Height;
    chosen_size = 0;
    chosen_size += chosen_height / 26;
    if (chosen_size > 3)
        chosen_size = 3;
    if (chosen_width > 26)
        chosen_size += 4;
    chosen_size_text = IntToStr(chosen_size);
    ModalResult = mrOk;
}

// The mouse wheel scrolls the graphics a row at a time.
void __fastcall TFormGfx::FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos,
    bool &Handled)
{
    wheel_time = clock();
    if (WheelDelta < 0)
        scroll_row++;
    if (WheelDelta > 0)
        scroll_row--;
    if (ElScrollBar1->Min > scroll_row)
        scroll_row = ElScrollBar1->Min;
    if (ElScrollBar1->Max < scroll_row)
        scroll_row = ElScrollBar1->Max;
    ElScrollBar1->Position = scroll_row;
    first_shown = scroll_row * row_size;
    ShowPage(first_shown + 1);
}

// Scrolls to the bar's row, ignoring the bar for a moment after a wheel turn.
void __fastcall TFormGfx::ElScrollBar1Scroll(TObject *Sender, TElScrollCode ScrollCode, int &ScrollPos, bool &DoChange)
{
    if (scroll_ready && scroll_row != ScrollPos)
    {
        float elapsed = clock() - wheel_time;
        if (elapsed < 200.0)
            DoChange = false;
        else
        {
            scroll_row = ScrollPos;
            first_shown = ScrollPos * row_size;
            ShowPage(first_shown + 1);
        }
    }
}

// Escape or End closes the viewer; the number keys pick one of the ten graphics.
void __fastcall TFormGfx::ElScrollBar1KeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_ESCAPE || Key == '#')
    {
        ((TCustomForm *)this)->Close();
        return;
    }
    if (Key == '1')
        Panel1Click((TObject *)this);
    if (Key == '2')
        Image3Click((TObject *)this);
    if (Key == '3')
        Panel5Click((TObject *)this);
    if (Key == '4')
        Panel7Click((TObject *)this);
    if (Key == '5')
        Panel9Click((TObject *)this);
    if (Key == '6' || Key == 'q')
        Panel11Click((TObject *)this);
    if (Key == '7' || Key == 'w')
        Panel13Click((TObject *)this);
    if (Key == '8' || Key == 'e')
        Panel15Click((TObject *)this);
    if (Key == '9' || Key == 'r')
        Image17Click((TObject *)this);
    if (Key == '0' || Key == 't')
        Panel19Click((TObject *)this);
}

// Hides the cover panels a moment after the viewer opens.
void __fastcall TFormGfx::Timer1Timer(TObject *Sender)
{
    ShowPanels(false);
    Timer1->Enabled = false;
}

// Frees a gfx file the viewer opened for itself.
void __fastcall TFormGfx::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (own_library)
    {
        FreeLibrary(opened_library);
        own_library = false;
    }
}
