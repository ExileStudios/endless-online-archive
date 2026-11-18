#ifndef UnitGfxH
#define UnitGfxH

#include <Classes.hpp>
#include <ElSBCtrl.hpp>
#include <ElScrollBar.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// The graphic viewer: pages of graphics from a gfx file, one of them picked.
class TFormGfx : public TForm
{
__published:
    TPanel *Panel1;
    TPanel *Panel2;
    TImage *Image1;
    TElScrollBar *ElScrollBar1;
    TImage *Image2;
    TLabel *Label1;
    TPanel *Panel3;
    TImage *Image3;
    TImage *Image4;
    TLabel *Label2;
    TPanel *Panel4;
    TPanel *Panel5;
    TImage *Image5;
    TImage *Image6;
    TLabel *Label3;
    TPanel *Panel6;
    TPanel *Panel7;
    TImage *Image7;
    TImage *Image8;
    TLabel *Label4;
    TPanel *Panel8;
    TPanel *Panel9;
    TImage *Image9;
    TImage *Image10;
    TLabel *Label5;
    TPanel *Panel10;
    TPanel *Panel11;
    TImage *Image11;
    TImage *Image12;
    TLabel *Label6;
    TPanel *Panel12;
    TPanel *Panel13;
    TImage *Image13;
    TImage *Image14;
    TLabel *Label7;
    TPanel *Panel14;
    TPanel *Panel15;
    TImage *Image15;
    TImage *Image16;
    TLabel *Label8;
    TPanel *Panel16;
    TPanel *Panel17;
    TImage *Image17;
    TImage *Image18;
    TLabel *Label9;
    TPanel *Panel18;
    TPanel *Panel19;
    TImage *Image19;
    TImage *Image20;
    TLabel *Label10;
    TPanel *Panel20;
    TPanel *Panel21;
    TLabel *labelGfxCaption;
    TImage *imageGfxArrow;
    TLabel *labelNameCaption;
    TPanel *Panel22;
    TLabel *Label13;
    TPanel *Panel23;
    TLabel *Label14;
    TPanel *Panel24;
    TLabel *Label15;
    TPanel *Panel25;
    TLabel *Label16;
    TPanel *Panel26;
    TLabel *Label17;
    TPanel *Panel27;
    TLabel *Label18;
    TPanel *Panel28;
    TLabel *Label19;
    TPanel *Panel29;
    TLabel *Label20;
    TPanel *Panel30;
    TLabel *Label21;
    TPanel *Panel31;
    TLabel *Label22;
    TTimer *Timer1;
    TLabel *labelMaxGfxIDS;
    void __fastcall ElScrollBar1Scroll(TObject *Sender, TElScrollCode ScrollCode, int &ScrollPos, bool &DoChange);
    void __fastcall Panel1Click(TObject *Sender);
    void __fastcall Image3Click(TObject *Sender);
    void __fastcall Panel5Click(TObject *Sender);
    void __fastcall Panel7Click(TObject *Sender);
    void __fastcall Panel9Click(TObject *Sender);
    void __fastcall Panel11Click(TObject *Sender);
    void __fastcall Panel13Click(TObject *Sender);
    void __fastcall Panel15Click(TObject *Sender);
    void __fastcall Image17Click(TObject *Sender);
    void __fastcall Panel19Click(TObject *Sender);
    void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos,
    bool &Handled);
    void __fastcall ElScrollBar1KeyPress(TObject *Sender, char &Key);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:
public:
    TElTreeStringGrid *usage_grid;
    bool own_library;
    HINSTANCE__ *opened_library;
    HINSTANCE__ *view_library;
    HINSTANCE__ *page_library;
    unsigned char draw_mode;
    int first_resource;
    int usage_column;
    int resource_step;
    int graphic_count;
    int last_row;
    int row_size;
    int scroll_row;
    int first_shown;
    bool first_show;
    bool scroll_ready;
    int wheel_time;
    String chosen_number;
    int chosen_width;
    int chosen_height;
    int chosen_size;
    String chosen_size_text;
    String library_file;
    bool panels_pending;

    __fastcall TFormGfx(TComponent *owner);
    bool OpenLibrary(int number, int first, int step, TElTreeStringGrid *grid, int column);
    String LibraryFileName(int number);
    void SetLibrary(HINSTANCE__ *library, int first, int step, int count, TElTreeStringGrid *grid, int column);
    void SetDrawMode(char mode);
    void ShowCaptions(TForm *owner, String gfx_caption, String name_caption, int value);
    void ShowPanels(bool on);
    void ShowPage(int first);
    char DrawFrame(TImage *image, int index, int frame, int resource, char drawn);
    bool LoadImage(TImage *image, int index, int resource);
    bool ShowGraphic(TImage *image, TPanel *panel, TLabel *label, int number);
    String GraphicHint(int number, TElTreeStringGrid *grid, int column);
    void ShowBigImage(TPanel *panel, TImage *image);
};

extern PACKAGE TFormGfx *FormGfx;

#endif
