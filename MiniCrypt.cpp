#include <math.h>
#include <wx/wx.h>
#include <wx/clipbrd.h>
#include "atlenc.h"

class MiniCryptApp : public wxApp
{
	bool OnInit();
	void OnEncode(wxCommandEvent& event);
	void OnDecode(wxCommandEvent& event);
private:
	wxDialog *dialog;
	wxTextCtrl *key;
};

IMPLEMENT_APP(MiniCryptApp)

MiniCryptApp* app;

bool MiniCryptApp::OnInit()
{
	app = this;
	wxIcon icon("IDI_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
	dialog = new wxDialog(NULL, -1, "MiniCrypt", wxDefaultPosition, wxSize(180,105), wxCAPTION | wxSYSTEM_MENU);
	dialog->SetIcon(icon);
	wxBoxSizer* bsTop = new wxBoxSizer(wxVERTICAL);
	wxStaticText* stLabelTop = new wxStaticText(dialog, -1, "Schlüssel:");
	bsTop->Add(stLabelTop, 0, wxALL, 0);
	key = new wxTextCtrl(dialog, -1);
	bsTop->Add(key, 0, wxALL | wxEXPAND, 2);
	wxBoxSizer* bsButton = new wxBoxSizer(wxHORIZONTAL);
	wxButton* bEncrypt = new wxButton(dialog, -1, "Encode");
	bEncrypt->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MiniCryptApp::OnEncode);
	bsButton->Add(bEncrypt);
	wxButton* bDecrypt = new wxButton(dialog, -1, "Decode");
	bDecrypt->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MiniCryptApp::OnDecode);
	bsButton->Add(bDecrypt);
	bsTop->Add(bsButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 2);
	wxStaticText* stLabelBottom = new wxStaticText(dialog, -1, "Verschlüsselt Inhalt der Zwischenablage.", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	wxFont font(7, wxROMAN, wxNORMAL, wxNORMAL, false);
	stLabelBottom->SetFont(font);
	bsTop->Add(stLabelBottom, 1, wxALL | wxEXPAND, 0);
	dialog->SetSizer(bsTop);
	dialog->Layout();
	dialog->ShowModal();
	this->ExitMainLoop();
	delete dialog;
	return true;
}

void code(char* inp, int len, wxString key)
{
	char prev = 0;
	char table[256];
	long long akku = 0;
	for (int i = 0; i < 256; i++)
	{
		const char akey = key.GetChar(i % key.Length());
		const char sinp = (char)(sin((double)akey + prev) * 255);
		table[i] = sinp ^ ((akku >> 3) % 256);
		akku += (unsigned char)(table[i]);
	}
	for (int i = 0; i < len; i++)
		inp[i] = inp[i] ^ table[i % 256];
}

void MiniCryptApp::OnEncode(wxCommandEvent& event)
{
	wxOpenClipboard();
	char* input = (char*)wxGetClipboardData(wxDataFormat(wxDF_TEXT));
	wxCloseClipboard();
	int ilen = Strlen(input);
	code(input, ilen, app->key->GetValue());
	int olen = UUEncodeGetRequiredLength(ilen);
	char* output = new char[olen + 1];
	UUEncode((unsigned char *)input, ilen, output, &olen);
	output[olen] = 0;
	wxOpenClipboard();
	wxEmptyClipboard();
	wxSetClipboardData(wxDataFormat(wxDF_TEXT), output);
	wxCloseClipboard();
	delete output;
	delete input;
}

void MiniCryptApp::OnDecode(wxCommandEvent& event)
{
	wxOpenClipboard();
	char* input = (char*)wxGetClipboardData(wxDataFormat(wxDF_TEXT));
	wxCloseClipboard();
	int ilen = Strlen(input);
	int olen = UUDecodeGetRequiredLength(ilen);
	char* output = new char[olen + 1];
	UUDecode((unsigned char *)input, ilen, (unsigned char *)output, &olen);
	code(output, olen, app->key->GetValue());
	output[olen] = 0;
	wxOpenClipboard();
	wxEmptyClipboard();
	wxSetClipboardData(wxDataFormat(wxDF_TEXT), output);
	wxCloseClipboard();
	delete output;
	delete input;
}
