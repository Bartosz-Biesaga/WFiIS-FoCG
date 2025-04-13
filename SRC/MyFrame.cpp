#include "MyFrame.h"
#include <cmath>
#include <cfloat>

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(ID_LoadImage, MyFrame::OnLoadImage)
EVT_SLIDER(ID_SliderTolerance, MyFrame::OnSliderUpdate)
EVT_SLIDER(ID_SliderStrength, MyFrame::OnSliderUpdate)
EVT_SLIDER(ID_SliderBrightness, MyFrame::OnBrightnessSliderUpdate)
EVT_SIZE(MyFrame::OnResize)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800)),
    m_mixing_level(100.0), m_strength(50.), m_brightness(255), m_newColor(*wxWHITE) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* vboxLeft = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vboxRight = new wxBoxSizer(wxVERTICAL);

    m_imagePanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
    vboxLeft->Add(m_imagePanel, 1, wxALL | wxCENTER | wxEXPAND, 10);

    m_hexImagePanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    vboxRight->Add(m_hexImagePanel, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);

    wxBoxSizer* hboxColorPanels = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* vboxSelectedColor = new wxBoxSizer(wxVERTICAL);
    wxStaticText* imageColorLabel = new wxStaticText(panel, wxID_ANY, "Selected Color:");
    vboxSelectedColor->Add(imageColorLabel, 0, wxALL | wxCENTER, 5);
    m_imageColorPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(50, 50));
    m_imageColorPanel->SetBackgroundColour(*wxWHITE);
    vboxSelectedColor->Add(m_imageColorPanel, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);
    hboxColorPanels->Add(vboxSelectedColor, 0, wxALL | wxCENTER, 5);

    wxBoxSizer* vboxHexColor = new wxBoxSizer(wxVERTICAL);
    wxStaticText* hexColorLabel = new wxStaticText(panel, wxID_ANY, "Hexagon Color:");
    vboxHexColor->Add(hexColorLabel, 0, wxALL | wxCENTER, 5);
    m_hexColorPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(50, 50));
    m_hexColorPanel->SetBackgroundColour(*wxWHITE);
    vboxHexColor->Add(m_hexColorPanel, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);
    hboxColorPanels->Add(vboxHexColor, 0, wxALL | wxCENTER, 5);

    vboxRight->Add(hboxColorPanels, 0, wxALL | wxCENTER, 10);

    wxStaticText* mixingLevelLabel = new wxStaticText(panel, wxID_ANY, "Mixing Level:");
    vboxRight->Add(mixingLevelLabel, 0, wxALL | wxCENTER, 5);

    m_sliderMixingLevel = new wxSlider(panel, ID_SliderTolerance, 100, 1, 100, wxDefaultPosition, wxSize(200, wxDefaultCoord), wxSL_LABELS);
    vboxRight->Add(m_sliderMixingLevel, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);

    wxStaticText* strengthLabel = new wxStaticText(panel, wxID_ANY, "Strength:");
    vboxRight->Add(strengthLabel, 0, wxALL | wxCENTER, 5);

    m_sliderStrength = new wxSlider(panel, ID_SliderStrength, 50, 0, 100, wxDefaultPosition, wxSize(200, wxDefaultCoord), wxSL_LABELS);
    vboxRight->Add(m_sliderStrength, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);

    wxStaticText* brightnessLabel = new wxStaticText(panel, wxID_ANY, "Brightness:");
    vboxRight->Add(brightnessLabel, 0, wxALL | wxCENTER, 5);

    m_sliderBrightness = new wxSlider(panel, ID_SliderBrightness, 255, 0, 255, wxDefaultPosition, wxSize(200, wxDefaultCoord), wxSL_LABELS);
    vboxRight->Add(m_sliderBrightness, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);

    m_loadImageButton = new wxButton(panel, ID_LoadImage, "Load Image");
    vboxRight->Add(m_loadImageButton, 0, wxALL | wxCENTER | wxFIXED_MINSIZE, 10);

    hbox->Add(vboxLeft, 1, wxALL | wxEXPAND, 10);
    hbox->Add(vboxRight, 0, wxALL | wxEXPAND, 10);

    panel->SetSizer(hbox);

    m_imagePanel->Bind(wxEVT_LEFT_DOWN, &MyFrame::OnImageClick, this);
    m_hexImagePanel->Bind(wxEVT_LEFT_DOWN, &MyFrame::OnHexagonClick, this);

    GenerateHexagonImage();

    wxQueueEvent(m_sliderBrightness, new wxCommandEvent(wxEVT_SLIDER, ID_SliderBrightness));
}

void MyFrame::OnResize(wxSizeEvent& event) {
    wxFrame::OnSize(event);

    if (m_image.IsOk()) {
        m_scaledModifiedImage = m_modifiedImage.Copy().Scale(m_imagePanel->GetSize().GetWidth(), m_imagePanel->GetSize().GetHeight());
        DisplayImage();
    }
    DrawHexagon();
    event.Skip();
}

void MyFrame::DisplayImage()
{
    wxBitmap bitmap(m_scaledModifiedImage);
    wxClientDC dc(m_imagePanel);
    dc.DrawBitmap(bitmap, 0, 0, true);
}

void MyFrame::DrawHexagon()
{
    wxBitmap bitmap(m_hexagonImageCopy);
    wxClientDC dc(m_hexImagePanel);
    dc.DrawBitmap(bitmap, 0, 0, true);
    if (m_closestPoint != wxPoint(0,0))
    {
        if (m_brightness <= 128)
        {
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.SetPen(*wxWHITE_PEN);
        }
        else
        {
            dc.SetBrush(*wxBLACK_BRUSH);
            dc.SetPen(*wxBLACK_PEN);
        }
        dc.DrawCircle(m_closestPoint, 5);
    }
}

void MyFrame::GenerateHexagonImage() {
    
    constexpr int width = 200;
    constexpr int height = 200;
    const wxPoint center(width / 2, height / 2);
    constexpr int radius = 100;

    m_hexagonImage.Create(width, height);

    for (int i = 0; i < 6; ++i) {
        const double angle = M_PI / 2 + M_PI / 3 * i;
        const int x = ceil(center.x + radius * cos(angle));
        const int y = ceil(center.y - radius * sin(angle));
        m_hexagonVertices.push_back(wxPoint(x, y));
    }

    unsigned char* data = m_hexagonImage.GetData();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const wxPoint pt(x, y);

            if (!pointInHexagon(pt, m_hexagonVertices)) {
                data[0] = data[1] = data[2] = 255;
                data += 3;
                continue;
            }

            wxColour color;
            double alfa = 0;
            double beta = 0;

            switch (pointInWhichRhombus(pt, m_hexagonVertices, &alfa, &beta))
            {
            case 0:
                color.SetRGB(RGB(255, beta*255, alfa*255));
                break;
            case 1:
                color.SetRGB(RGB(beta*255, alfa*255, 255));
                break;
            case 2:
                color.SetRGB(RGB(alfa*255, 255, beta*255));
                break;
            default:
                color.SetRGB(RGB(128, 128, 128));
                break;
            }
            
            *data++ = color.GetRed();
            *data++ = color.GetGreen();
            *data++ = color.GetBlue();
            
        }
    }
    m_hexagonImage.SetMaskColour(255, 255, 255);
    m_hexagonImageCopy = m_hexagonImage.Copy();
    
}

int pointInWhichRhombus(const wxPoint& pt, const std::vector<wxPoint>& vertices, double* alfa, double* beta)
{
    const wxPoint center(vertices[0].x, (vertices[0].y+vertices[3].y)/2);
    int ret = -1; 
    double angle = atan2(center.y-pt.y, pt.x-center.x);
    
    if (angle < 0)
        angle += 2 * M_PI;
    if (angle >= 0 && angle <= M_PI / 6)
        angle += 2 * M_PI;
    double angle1 = M_PI / 6;
    double angle2 = M_PI / 6 + M_PI / 3 * 2;
    int i;
    for(i = 0; i < 3; ++i)
        if (angle >= angle1 && angle <= angle2)
        {
            ret = i;
            break;
        }
        else
        {
            angle1 += M_PI / 3 * 2;
            angle2 += M_PI / 3 * 2;
        }
    double al, bl, ar, br;
    wxPoint ptl, ptm, ptr;
    ptm = vertices[2 * i];
    ptl = vertices[(2 * i - 1 + 6) % 6];
    ptr = vertices[2 * i + 1];

    al = 1. * (ptl.y - ptm.y) / (ptl.x - ptm.x + 1.e-9);
    bl = ptm.y - al * ptm.x;

    ar = 1. * (ptr.y - ptm.y) / (ptr.x - ptm.x + 1.e-9);
    br = ptm.y - ar * ptm.x;

    double b_beta = pt.y - al * pt.x;
    double b_alfa = pt.y - ar * pt.x;

    double x_beta = (b_beta - br) / (ar - al);
    double y_beta = ar * x_beta + br;

    double x_alfa = (bl - b_alfa) / (ar - al);
    double y_alfa = al * x_alfa +bl;

    *beta = sqrt(pow(y_beta - pt.y, 2) + pow(x_beta - pt.x, 2)) / (center.y - vertices[0].y);
    if (*beta > 1.)
        *beta = 1;
    *alfa = sqrt(pow(y_alfa - pt.y, 2) + pow(x_alfa - pt.x, 2)) / (center.y - vertices[0].y);
    if (*alfa > 1.)
        *alfa = 1;

    return ret;
}

void MyFrame::UpdateHexagonBrightness()
{
    m_hexagonImageCopy = m_hexagonImage.Copy();

    unsigned char* data = m_hexagonImageCopy.GetData();
    const double factor = m_brightness / 255.;

    const int width = m_hexagonImageCopy.GetSize().x;
    const int height = m_hexagonImageCopy.GetSize().y;

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            if (data[0] + data[1] + data[2] == 765)
                data += 3;
            else
            {
                *data++ *= factor;
                *data++ *= factor;
                *data++ *= factor;
            }
    data = &m_hexagonImageCopy.GetData()[height / 2 * 3 * width + 3 * width / 2];
    *data++ *= factor;
    *data++ *= factor;
    *data++ *= factor;
    m_closestPoint = wxPoint(0, 0);
    DrawHexagon();
}

void MyFrame::OnLoadImage(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
        "All files (*.*)|*.*|Image files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString filePath = openFileDialog.GetPath();
    m_image.LoadFile(filePath, wxBITMAP_TYPE_ANY);
    if (!m_image.IsOk()) {
        wxLogError("Cannot load image from file '%s'.", filePath);
        return;
    }
    m_modifiedImage = m_image.Copy().Scale(m_imagePanel->GetSize().GetWidth(), m_imagePanel->GetSize().GetHeight());
    m_scaledModifiedImage = m_modifiedImage;
    DisplayImage();
}

void MyFrame::OnImageClick(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();

    double scaleX = static_cast<double>(m_image.GetWidth()) / m_imagePanel->GetSize().GetWidth();
    double scaleY = static_cast<double>(m_image.GetHeight()) / m_imagePanel->GetSize().GetHeight();

    int imgX = static_cast<int>(pos.x * scaleX);
    int imgY = static_cast<int>(pos.y * scaleY);

    if (imgX < m_image.GetWidth() && imgY < m_image.GetHeight()) {
        unsigned char r = m_image.GetRed(imgX, imgY);
        unsigned char g = m_image.GetGreen(imgX, imgY);
        unsigned char b = m_image.GetBlue(imgX, imgY);
        wxColour color(r, g, b);

        UpdateImageColorPanel(color);

        m_selectedColor = color;

        int max_component_value = r;
        if (g > max_component_value)
            max_component_value = g;
        if (b > max_component_value)
            max_component_value = b;

        m_sliderBrightness->SetValue(max_component_value);
        m_brightness = m_sliderBrightness->GetValue();
        UpdateHexagonBrightness();

        MarkColorOnHexagon(m_selectedColor);
        ApplyColorChange();
    }
}

void MyFrame::OnHexagonClick(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();


    double scaleX = static_cast<double>(m_hexagonImageCopy.GetWidth()) / m_hexImagePanel->GetSize().GetWidth();
    double scaleY = static_cast<double>(m_hexagonImageCopy.GetHeight()) / m_hexImagePanel->GetSize().GetHeight();

    int imgX = static_cast<int>(pos.x * scaleX);
    int imgY = static_cast<int>(pos.y * scaleY);

    if (!pointInHexagon(wxPoint(imgX, imgY), m_hexagonVertices))
        return;

    if (imgX < m_hexagonImageCopy.GetWidth() && imgY < m_hexagonImageCopy.GetHeight()) {
        unsigned char r = m_hexagonImageCopy.GetRed(imgX, imgY);
        unsigned char g = m_hexagonImageCopy.GetGreen(imgX, imgY);
        unsigned char b = m_hexagonImageCopy.GetBlue(imgX, imgY);
        wxColour color(r, g, b);

        m_newColor = color;
        UpdateHexColorPanel(color);
        ApplyColorChange();
    }
}

void MyFrame::UpdateImageColorPanel(const wxColour& color) {
    m_imageColorPanel->SetBackgroundColour(color);
    m_imageColorPanel->Refresh();
}

void MyFrame::UpdateHexColorPanel(const wxColour& color) {
    m_hexColorPanel->SetBackgroundColour(color);
    m_hexColorPanel->Refresh();
}

void MyFrame::MarkColorOnHexagon(const wxColour& color) {
    
    double smallestDistance = DBL_MAX;

    unsigned char color_r = color.GetRed();
    unsigned char color_g = color.GetGreen();
    unsigned char color_b = color.GetBlue();

    unsigned char* data = m_hexagonImageCopy.GetData();

    for (int y = 0; y < m_hexagonImageCopy.GetHeight(); ++y) {
        for (int x = 0; x < m_hexagonImageCopy.GetWidth(); ++x) {
            if(!pointInHexagon(wxPoint(x,y), m_hexagonVertices))
            {
                data += 3;
                continue;
            }

            unsigned char r = *data++;
            unsigned char g = *data++;
            unsigned char b = *data++;

            double distance = sqrt(pow(color_r - r, 2) + pow(color_g - g, 2) + pow(color_b - b, 2));

            if (distance < smallestDistance) 
            {
                smallestDistance = distance;
                m_closestPoint = wxPoint(x, y);
            }
        }
    }
    m_hexImagePanel->ClearBackground();
    DrawHexagon();
}

void MyFrame::ApplyColorChange() {
    if (!m_image.IsOk())
        return;

    m_modifiedImage = m_image.Copy().Scale(m_imagePanel->GetSize().GetWidth(), m_imagePanel->GetSize().GetHeight());
    double change_coefficient = m_strength / 100. * m_change_coefficient;
    double mixing_level = m_mixing_level / 100.;
    double eps = 1e-7;

    unsigned char* data = m_modifiedImage.GetData();
    int width = m_modifiedImage.GetWidth();
    int height = m_modifiedImage.GetHeight();

    unsigned char r_selected = m_selectedColor.Red();
    unsigned char g_selected = m_selectedColor.Green();
    unsigned char b_selected = m_selectedColor.Blue();

    unsigned char r_new = m_newColor.Red();
    unsigned char g_new = m_newColor.Green();
    unsigned char b_new = m_newColor.Blue();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char* r = data++;
            unsigned char* g = data++;
            unsigned char* b = data++;

            double distance = sqrt(pow(*r - r_selected + eps, 2) + pow(*g - g_selected + eps, 2) + pow(*b - b_selected + eps, 2));

            double r_delta = change_coefficient / distance - 50;
            double g_delta = change_coefficient / distance - 50;
            double b_delta = change_coefficient / distance - 50;
            if (r_delta < 0)
                r_delta = 0;
            if (g_delta < 0)
                g_delta = 0;
            if (b_delta < 0)
                b_delta = 0;

            double r_delta_max = r_new - *r;
            double g_delta_max = g_new - *g;
            double b_delta_max = b_new - *b;

            if (r_delta_max < 0)
                r_delta = -r_delta;
            if (g_delta_max < 0)
                g_delta = -g_delta;
            if (b_delta_max < 0)
                b_delta = -b_delta;

            if (fabs(r_delta) > fabs(r_delta_max))
                r_delta = r_delta_max;
            if (fabs(g_delta) > fabs(g_delta_max))
                g_delta = g_delta_max;
            if (fabs(b_delta) > fabs(b_delta_max))
                b_delta = b_delta_max;

            *r += (r_delta * mixing_level);
            *g += (g_delta * mixing_level);
            *b += (b_delta * mixing_level);
        }
    }
    m_scaledModifiedImage = m_modifiedImage.Copy().Scale(m_imagePanel->GetSize().GetWidth(), m_imagePanel->GetSize().GetHeight());
    DisplayImage();
}

void MyFrame::OnSliderUpdate(wxCommandEvent& event) {
    m_mixing_level = m_sliderMixingLevel->GetValue();
    m_strength = m_sliderStrength->GetValue();
    ApplyColorChange();
}

void MyFrame::OnBrightnessSliderUpdate(wxCommandEvent& event) {
    m_brightness = m_sliderBrightness->GetValue();
    UpdateHexagonBrightness();
}

bool pointInHexagon(const wxPoint& point, const std::vector<wxPoint>& vertices) {
    if (point.x < vertices[1].x)
        return false;
    if (point.x > vertices[5].x)
        return false;
    
    if (point.y < vertices[0].y)
        return false;
    if (point.y > vertices[3].y)
        return false;
    

    double a1 = 1. * (vertices[0].y - vertices[1].y) / (vertices[0].x - vertices[1].x);
    double b1 = vertices[0].y - a1 * vertices[0].x;
    if (a1 * point.x + b1 > point.y)
        return false;
    
    a1 = 1. * (vertices[5].y - vertices[0].y) / (vertices[5].x - vertices[0].x);
    b1 = vertices[0].y - a1 * vertices[0].x;
    if (a1 * point.x + b1 > point.y)
        return false;

    a1 = 1. * (vertices[3].y - vertices[2].y) / (vertices[3].x - vertices[2].x);
    b1 = vertices[3].y - a1 * vertices[3].x;
    if (a1 * point.x + b1 < point.y)
        return false;

    a1 = 1. * (vertices[3].y - vertices[4].y) / (vertices[3].x - vertices[4].x);
    b1 = vertices[3].y - a1 * vertices[3].x;
    if (a1 * point.x + b1 < point.y)
        return false;
 
    return true;
}