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
#include <wx/base64.h>
#include <wx/app.h>
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
#include "Tiger.h"

std::string code(const std::string &in, const std::string &key)
{
	Algorithm_Tiger tiger;
	tiger.process(key + "\n");
	tiger.process(std::to_string(in.size()));
	const Hash *hash = tiger.finish();
	const std::size_t hash_len = hash->len() / 8;

	std::string result(in);
	for (std::size_t i = 0; i < in.size(); ++i)
	{
		const std::size_t hash_idx = i % hash_len;
		if ((i % hash_len) == (hash_len - 1))
		{
			tiger.process(hash->str() + "\n");
			tiger.process(key + "\n");
			tiger.process(std::to_string(in.size()));
			delete hash;
			hash = tiger.finish();
		}
		result[i] ^= (hash->data()[hash_idx]);
	}
	delete hash;
	return result;
}

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

MiniCryptApp* app;

bool MiniCryptApp::OnInit()
{
	app = this;
	wxIcon icon(minicrypt_ico);
	dialog = new wxDialog(NULL, -1, wxGetTranslation(L"MiniCrypt"), wxDefaultPosition, wxSize(180,105),
		wxDEFAULT_DIALOG_STYLE);
	dialog->SetIcon(icon);

	wxBoxSizer* bsTop = new wxBoxSizer(wxVERTICAL);

	wxStaticText* stLabelTop = new wxStaticText(dialog, -1, wxGetTranslation(L"Password"));
	bsTop->Add(stLabelTop, 0, wxALL, 2);

	key = new wxTextCtrl(dialog, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
	bsTop->Add(key, 0, wxALL | wxEXPAND, 2);

	wxBoxSizer* bsButton = new wxBoxSizer(wxHORIZONTAL);

	wxButton* bEncrypt = new wxButton(dialog, -1, wxGetTranslation(L"Encode"));
	bEncrypt->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [=](wxCommandEvent&) { this->ProcessClipboard(MiniCryptApp::Encode); });
	bsButton->Add(bEncrypt);

	wxButton* bDecrypt = new wxButton(dialog, -1, wxGetTranslation(L"Decode"));
	bDecrypt->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [=](wxCommandEvent&) { this->ProcessClipboard(MiniCryptApp::Decode); });
	bsButton->Add(bDecrypt);

	bsTop->Add(bsButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 2);
	wxStaticText* stLabelBottom = new wxStaticText(dialog, -1,
		wxGetTranslation(L"Encode/Decode clipboard content"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	wxFont font(7, wxROMAN, wxNORMAL, wxNORMAL, false);
	stLabelBottom->SetFont(font);
	bsTop->Add(stLabelBottom, 1, wxTOP | wxALIGN_CENTER, 7);

	dialog->SetSizer(bsTop);
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
			const wxString output = processor(data.GetText(), wx2str(app->key->GetValue()));
			// clipboard takes ownership of wxTextDataObject!
			wxTheClipboard->SetData(new wxTextDataObject(output));
		}
		wxTheClipboard->Close();
		wxTheClipboard->Flush();
	}
}

wxString MiniCryptApp::Encode(const wxString &value, const std::string &key)
{
	const std::string data_enc = code(wx2str(value), key);
	return wxBase64Encode(data_enc.data(), data_enc.size());
}

wxString MiniCryptApp::Decode(const wxString &value, const std::string &key)
{
	size_t posErr;
	const wxMemoryBuffer buffer = wxBase64Decode(value, wxBase64DecodeMode_SkipWS, &posErr);
	const std::string data_enc = std::string(static_cast<char*>(buffer.GetData()), buffer.GetDataLen());
	const std::string data_dec = code(data_enc, key);
	return wxString::FromUTF8(data_dec.data(), data_dec.size());
}
