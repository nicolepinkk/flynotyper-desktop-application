#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <fstream>

int numGridRows = 25;
int numGridCols = 6;
bool isHorizontal = false;
bool isSEM = false;
bool outputToCsv = false;
int nCount = 200;
std::string flynotyperResult;
std::string nCountStr;
wxTextCtrl *countInput;
wxNotebook *notebook;
wxGrid *grid;

class FlynotyperApp : public wxApp
{
public:
    bool OnInit() override;
};

class FlynotyperFrame : public wxFrame
{
public:
    FlynotyperFrame();
    
private:
    void OnExit(wxCommandEvent& event);
    void OnHorizontalCheck(wxCommandEvent& event);
    void OnSEMCheck(wxCommandEvent& event);
    void OnCsvCheck(wxCommandEvent& event);
    void OnOpenImage(wxCommandEvent& event);
};

enum
{
    ID_Hor_Checkbox = 1,
    ID_Submit_Button = 2,
    ID_Sem_Checkbox = 3,
    ID_Csv_Checkbox = 4,
    ID_Count_Input = 5,
    ID_Update_Count_Button = 6
};