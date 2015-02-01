#include "I18N.h"
#include <wx/intl.h>
#include <vector>
#include <map>

// Embedded translations to have everything in a single executable

struct I18N_Translation
{
	std::vector<wxLanguage> languages;
	std::map<I18N_String, wxString> translation;
};

static std::vector<I18N_Translation> i18n_table =
{
	{
		{wxLANGUAGE_DEFAULT},
		{
			{I18N_String::TITLE, L"MiniCrypt"},
			{I18N_String::PASSWORD, L"Password"},
			{I18N_String::ENCODE, L"Encode"},
			{I18N_String::DECODE, L"Decode"},
			{I18N_String::DESCRIPTION, L"Encode or decode clipboard content with password"},
			{I18N_String::ERR_TOO_SHORT, L"The clipboard content is incomplete!"},
			{I18N_String::ERR_TAMPERED, L"The clipboard content has been tampered with!"},
		}
	},
	{
		{wxLANGUAGE_GERMAN, wxLANGUAGE_GERMAN_AUSTRIAN, wxLANGUAGE_GERMAN_BELGIUM,
			wxLANGUAGE_GERMAN_LIECHTENSTEIN, wxLANGUAGE_GERMAN_LUXEMBOURG, wxLANGUAGE_GERMAN_SWISS},
		{
			{I18N_String::TITLE, L"MiniCrypt"},
			{I18N_String::PASSWORD, L"Passwort"},
			{I18N_String::ENCODE, L"Verschlüsseln"},
			{I18N_String::DECODE, L"Entschlüsseln"},
			{I18N_String::DESCRIPTION, L"Ver- oder Entschlüsselt den Inhalt der Zwischenablage"},
			{I18N_String::ERR_TOO_SHORT, L"Der Inhalt der Zwischenablage ist unvollständig!"},
			{I18N_String::ERR_TAMPERED, L"Der Inhalt der Zwischenablage ist ungültig oder wurde verändert!"},
		}
	}
};

const wxString myT(const I18N_String str)
{
	wxString result = i18n_table[0].translation.at(str);
	const wxLanguage lang_sys = static_cast<wxLanguage>(wxLocale::GetSystemLanguage());
	for (const I18N_Translation &i18n_entry : i18n_table)
		for (const wxLanguage lang_entry : i18n_entry.languages)
			if (lang_sys == lang_entry)
				for (const std::pair<const I18N_String, const wxString> i18n_t : i18n_entry.translation)
					if (i18n_t.first == str)
						result = i18n_t.second;
	return result;
}
