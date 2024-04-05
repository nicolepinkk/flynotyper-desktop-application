#include "desktop_app.h"
#include "flynotyper.cpp"

wxIMPLEMENT_APP(FlynotyperApp);

bool FlynotyperApp::OnInit()
{
    FlynotyperFrame *frame = new FlynotyperFrame();
    wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    frame->SetFont(font);
    frame->Show();
    return true;
}

FlynotyperFrame::FlynotyperFrame() : wxFrame(nullptr, wxID_ANY, "Flynotyper")
{   
    Maximize();

    wxMenu *menuFile = new wxMenu;
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    
    // wxMenu *menuHelp = new wxMenu;
    
    wxMenuBar *menuBar  = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    
    SetMenuBar(menuBar);

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    auto sizer = new wxGridBagSizer(margin, margin);
    panel->SetSizer(sizer);

    // Used to assign "coordinates" of widgets in the app
    std::vector<std::pair<wxGBPosition, wxGBSpan>> items = 
    {
        {{0, 0}, {1, 1}},
        {{1, 0}, {1, 1}},
        {{2, 0}, {1, 1}},
        {{3, 0}, {1, 1}},
        {{4, 0}, {1, 1}},
        {{5, 0}, {1, 1}},
        {{6, 0}, {1, 1}},
        {{7, 0}, {1, 1}},
        {{0, 1}, {9, 1}}
    };

    auto title = new wxStaticText(this, wxID_ANY, "FLYNOTYPER");
    auto instructions = new wxStaticText(this, wxID_ANY, 
            std::string("Upload the image(s) you want to analyze by pressing the button above.\n") + \
            std::string("Before selecting your image(s), you can use these parameters:\n") + \
            std::string("\t-Horizontal: use if your images were taken horizontally.\n") + \
            std::string("\t-SEM: use this parameter if the image was taken using SEM.\n") + \
            std::string("\t-n-Count: the number of most stable ommatidia taken into\n") + \
            std::string("\t    consideration when calculating the phenotypic score (200 by default)\n") + \
            std::string("\nThe data is displayed in five columns:\n") + \
            std::string("\t1. ODId: The total distance ommatidial disorderliness index\n") + \
            std::string("\t    of all stable ommatidia.\n") + \
            std::string("\t2. ODIa: The total angle ommatidial disorderliness index\n") + \
            std::string("\t    of all stable ommatidia.\n") + \
            std::string("\t3. ODI: The total ommatidial disorderliness of all stable ommatidia.\n") + \
            std::string("\t4. Z: The fusion index, which is the number of detected ommatidia.\n") + \
            std::string("\t5. P: The phenotypic score, calculated based on ODI and Z.\n"));
    auto imageButton = new wxButton(this, ID_Submit_Button, "Load Image", wxDefaultPosition, wxSize(200, 100));
    auto csvCheck = new wxCheckBox(this, ID_Csv_Checkbox, "Output to CSV", wxDefaultPosition, wxSize(150, 25));
    auto horizontalCheck = new wxCheckBox(this, ID_Hor_Checkbox, "Horizontal", wxDefaultPosition, wxSize(150, 25));
    auto semCheck = new wxCheckBox(this, ID_Sem_Checkbox, "SEM", wxDefaultPosition, wxSize(150, 25));
    countInput = new wxTextCtrl(this, ID_Count_Input, "", wxDefaultPosition, wxSize(150, 35));
    countInput->SetBackgroundColour(wxColour(200, 200, 200));
    auto countLabel = new wxStaticText(this, wxID_ANY, "n-Count");
    
    notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(750, 715)); // TODO: dynamically change height when updating rows?
    grid = new wxGrid(notebook, wxID_ANY);
    grid->CreateGrid(numGridRows, 6);
    grid->SetGridLineColour(wxColour(128, 128, 128));
    grid->SetColLabelValue(0, "Sample");
    grid->SetColLabelValue(1, "ODId");
    grid->SetColLabelValue(2, "ODIa");
    grid->SetColLabelValue(3, "ODI");
    grid->SetColLabelValue(4, "Z");
    grid->SetColLabelValue(5, "P");
    grid->SetColSize(0, 260);
    notebook->AddPage(grid, wxT("Flynotyper Results"));
    
    // Assign positions of different widgets in the app
    sizer->Add(title, items[0].first, items[0].second, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(imageButton, items[1].first, items[1].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(csvCheck, items[2].first, items[2].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(horizontalCheck, items[3].first, items[3].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(semCheck, items[4].first, items[4].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(countInput, items[5].first, items[5].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(countLabel, items[6].first, items[6].second, wxALIGN_LEFT | wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(instructions, items[7].first, items[7].second, wxEXPAND);
    sizer->Add(notebook, items[8].first, items[8].second, wxALIGN_RIGHT);

    sizer->AddGrowableRow(0, 2);
    sizer->AddGrowableRow(2, 1);

    sizer->AddGrowableCol(1);

    mainSizer->Add(panel, 1, wxEXPAND | wxALL, margin);
    this->SetSizerAndFit(mainSizer);
    
    Bind(wxEVT_MENU, &FlynotyperFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_BUTTON, &FlynotyperFrame::OnOpenImage, this, ID_Submit_Button);
    Bind(wxEVT_CHECKBOX, &FlynotyperFrame::OnCsvCheck, this, ID_Csv_Checkbox);
    Bind(wxEVT_CHECKBOX, &FlynotyperFrame::OnHorizontalCheck, this, ID_Hor_Checkbox);
    Bind(wxEVT_CHECKBOX, &FlynotyperFrame::OnSEMCheck, this, ID_Sem_Checkbox);
}

void FlynotyperFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void FlynotyperFrame::OnHorizontalCheck(wxCommandEvent& event)
{    
    isHorizontal = !isHorizontal;
}

void FlynotyperFrame::OnSEMCheck(wxCommandEvent& event)
{
    isSEM = !isSEM;
}

void FlynotyperFrame::OnCsvCheck(wxCommandEvent& event)
{
    outputToCsv = !outputToCsv;
}

void FlynotyperFrame::OnOpenImage(wxCommandEvent& event)
{   
    // Update n-count based on user input
    nCountStr = countInput->GetValue();
    if(nCountStr != "")
    {
        nCount = std::stoi(nCountStr);    
    }
    else
    {
        nCount = 200;
    }
    
    // Initialize or reset text field that shows application status
    wxStaticText *m_statictext = new wxStaticText(this, wxID_HIGHEST + 1, "",
        wxPoint(0, 600), wxSize(400, 100), wxALIGN_LEFT, wxStaticTextNameStr);;
    m_statictext->SetLabel("Calculating results. Please wait...");
    
    wxFileDialog dialog(this, "Open Image File", "", "", "Image Files (*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff)|*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    
    if(dialog.ShowModal() == wxID_CANCEL)
    {
       return;
    }
    
    // Get images submitted by user
    wxArrayString paths;
    dialog.GetPaths(paths);

    // Run flynotyper on all images
    flynotyperResult = "";
    
    #pragma omp parallel for
    for(int i = 0; i < paths.GetCount(); i++)
    {   
        std::string output = flynotyper(paths.Item(i).ToStdString(), isSEM, isHorizontal, nCount);

        std::stringstream ss(paths.Item(i).ToStdString());
        std::string str;
        std::string file;
        while(getline(ss, str, '/'))
        {
            file = str;
        }

        flynotyperResult += file + "\t" + output + "\n";
    }

    m_statictext->SetLabel("Calculations complete!");

    if(paths.GetCount() < numGridRows)
    {
        grid->DeleteRows(paths.GetCount() - 1, numGridRows - paths.GetCount());
    }
    else if(paths.GetCount() > numGridRows)
    {
        grid->AppendRows(paths.GetCount() - numGridRows);
    }
    numGridRows = paths.GetCount();
    
    std::stringstream ssNewline(flynotyperResult);
    std::string strNewline;
    int row = 0;
    int col = 0;

    // Show output from flynotyper
    while(getline(ssNewline, strNewline, '\n'))
    {
        std::stringstream ssTab(strNewline);
        std::string strTab;
        while(getline(ssTab, strTab, '\t'))
        {
            grid->SetCellValue(row, col, strTab);
            col++;
        }
        col = 0;
        row++;
    }

    if(outputToCsv)
    {
        std::ofstream output_file;
        output_file.open("output.csv");
        output_file << "Sample,ODId,ODIa,ODI,Z,P\n";
        
        int i = 0;
        while(i < numGridRows)
        {
            int j = 0;
            while(j < numGridCols)
            {
                output_file << grid->GetCellValue(i, j) << ",";
                j++;
            }
            output_file << "\n";
            i++;
        }
    }
}
