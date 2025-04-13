#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/slider.h>
#include <wx/dcclient.h>
#include <vector>

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    void OnLoadImage(wxCommandEvent& event);
    void OnImageClick(wxMouseEvent& event);
    void OnHexagonClick(wxMouseEvent& event);
    void OnSliderUpdate(wxCommandEvent& event);
    void OnBrightnessSliderUpdate(wxCommandEvent& event);
    void UpdateImageColorPanel(const wxColour& color);
    void UpdateHexColorPanel(const wxColour& color);
    void GenerateHexagonImage();
    void MarkColorOnHexagon(const wxColour& color);
    void DisplayImage();
    void ApplyColorChange();
    void UpdateHexagonBrightness();
    void OnResize(wxSizeEvent& event);
    void DrawHexagon();

    wxPanel* m_imagePanel;
    wxPanel* m_hexImagePanel;
    wxPanel* m_imageColorPanel;
    wxPanel* m_hexColorPanel;
    wxSlider* m_sliderMixingLevel;
    wxSlider* m_sliderStrength;
    wxSlider* m_sliderBrightness;
    wxButton* m_loadImageButton;
    wxImage m_image;
    wxImage m_modifiedImage;
    wxImage m_scaledModifiedImage;
    wxImage m_hexagonImage;
    wxImage m_hexagonImageCopy;
    std::vector<wxPoint> m_hexagonVertices;
    wxColour m_selectedColor;
    wxColour m_newColor;
    wxPoint m_closestPoint = wxPoint(0, 0);
    double m_mixing_level;
    double m_strength;
    int m_brightness;
    double m_change_coefficient = 255 * 20;

    wxDECLARE_EVENT_TABLE();
};

bool pointInHexagon(const wxPoint& point, const std::vector<wxPoint>& vertices);
int pointInWhichRhombus(const wxPoint& point, const std::vector<wxPoint>& vertices, double* alfa, double* beta);

enum {
    ID_LoadImage = 1,
    ID_SliderTolerance = 2,
    ID_SliderStrength = 3,
    ID_SliderBrightness = 4
};

#endif