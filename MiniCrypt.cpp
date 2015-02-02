/*
 * Copyright (C) 2004-2015 by Fred Stober
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "minicrypt.xpm"
#include <wx/setup.h>
#include <wx/app.h>
#include <wx/base64.h>
#include <wx/buffer.h>
#include <wx/dataobj.h>
#include <wx/button.h>
#include <wx/clipbrd.h>
#include <wx/dialog.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <functional>
#include "Crypt.h"
#include "I18N.h"

std::string wx2str(const wxString &value)
{
	const wxScopedCharBuffer data_utf8 = value.utf8_str();
	return std::string(data_utf8.data(), data_utf8.length());
}

class MiniCryptApp : public wxApp
{
	bool OnInit() override;
	void ProcessClipboard(std::function<wxString(wxString, std::string)> processor);
	static wxString Encode(const wxString &value, const std::string &key);
	static wxString Decode(const wxString &value, const std::string &key);
private:
	wxDialog *dialog;
	wxTextCtrl *key;
};

IMPLEMENT_APP(MiniCryptApp)

bool MiniCryptApp::OnInit()
{
	wxLocale loc;
	loc.Init();

	wxIcon icon(minicrypt_ico);
	dialog = new wxDialog(NULL, -1, myT(I18N_String::TITLE), wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE);
	dialog->SetIcon(icon);

	wxBoxSizer* bsMain = new wxBoxSizer(wxVERTICAL);

	wxStaticText* stLabelTop = new wxStaticText(dialog, -1, myT(I18N_String::PASSWORD));
	bsMain->Add(stLabelTop, 0, wxALL, 2);

	key = new wxTextCtrl(dialog, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	bsMain->Add(key, 0, wxALL | wxEXPAND, 2);

	wxBoxSizer* bsButton = new wxBoxSizer(wxHORIZONTAL);

	wxButton* bEncrypt = new wxButton(dialog, -1, myT(I18N_String::ENCODE));
	bEncrypt->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [=](wxCommandEvent&) { this->ProcessClipboard(MiniCryptApp::Encode); });
	bsButton->Add(bEncrypt);

	wxButton* bDecrypt = new wxButton(dialog, -1, myT(I18N_String::DECODE));
	bDecrypt->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [=](wxCommandEvent&) { this->ProcessClipboard(MiniCryptApp::Decode); });
	bsButton->Add(bDecrypt);

	bsMain->Add(bsButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 2);
	wxStaticText* stLabelBottom = new wxStaticText(dialog, -1, myT(I18N_String::DESCRIPTION),
		wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	stLabelBottom->Wrap(180);
	wxFont font = dialog->GetFont();
	font.MakeSmaller();
	stLabelBottom->SetFont(font);
	bsMain->Add(stLabelBottom, 1, wxTOP | wxBOTTOM | wxALIGN_CENTER, 7);

	dialog->SetSizer(bsMain);
	dialog->Fit();
	dialog->Layout();
	dialog->ShowModal();
	this->ExitMainLoop();
	delete dialog;
	return false;
}

void MiniCryptApp::ProcessClipboard(std::function<wxString(wxString, std::string)> processor)
{
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_TEXT))
		{
			wxTextDataObject data;
			wxTheClipboard->GetData(data);
			const wxString output = processor(data.GetText(), wx2str(key->GetValue()));
			// clipboard takes ownership of wxTextDataObject!
			wxTheClipboard->SetData(new wxTextDataObject(output));
		}
		wxTheClipboard->Close();
		wxTheClipboard->Flush();
	}
}

wxString MiniCryptApp::Encode(const wxString &value, const std::string &key)
{
	const std::string data_enc = encode(wx2str(value), key);
	return wxBase64Encode(data_enc.data(), data_enc.size());
}

wxString MiniCryptApp::Decode(const wxString &value, const std::string &key)
{
	size_t posErr;
	const wxMemoryBuffer buffer = wxBase64Decode(value, wxBase64DecodeMode_SkipWS, &posErr);
	const std::string data_enc = std::string(static_cast<char*>(buffer.GetData()), buffer.GetDataLen());
	DecodeError err = DecodeError::NONE;
	const std::string data_dec = decode(data_enc, key, &err);
	if (err == DecodeError::TOO_SHORT)
		wxLogError(myT(I18N_String::ERR_TOO_SHORT));
	else if (err == DecodeError::TAMPERED)
		wxLogError(myT(I18N_String::ERR_TAMPERED));
	return wxString::FromUTF8(data_dec.data(), data_dec.size());
}
