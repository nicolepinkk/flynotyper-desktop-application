#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/gauge.h>
#include <fstream>
#include <string>

int numGridRows = 25;
int numGridCols = 6;
bool isHorizontal = false;
bool isSEM = false;
bool outputToCsv = false;
bool killThread = false;
int nCount = 200;
std::string flynotyperResult;
std::string nCountStr;
wxTextCtrl *countInput;
wxNotebook *notebook;
wxGrid *grid;
wxGauge *gauge;

class DesktopApp : public wxApp
{
public:
    bool OnInit() override;
};

class DesktopFrame : public wxFrame
{
public:
    DesktopFrame();
    
private:
    void OnExit(wxCommandEvent& event);
    void OnHorizontalCheck(wxCommandEvent& event);
    void OnSEMCheck(wxCommandEvent& event);
    void OnCsvCheck(wxCommandEvent& event);
    void OnOpenImage(wxCommandEvent& event);

    // FlynotyperThread m_flynotyperThread;
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