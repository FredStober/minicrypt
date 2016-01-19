#include "I18N.h"
#include <vector>
#include <map>
#include <functional>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

// Using embedded translations to have everything in a single executable

struct I18N_Translation
{
	I18N_Translation(std::vector<wxLanguage> l) : languages(l) {}
	std::vector<wxLanguage> languages;
	std::map<I18N_String, wxString> translation;
};

const wxString myT(const I18N_String str)
{
	static I18N_Translation i18n_en({ wxLANGUAGE_DEFAULT });
	i18n_en.translation[I18N_String::TITLE] = wxString::FromUTF8("MiniCrypt");
	i18n_en.translation[I18N_String::PASSWORD] = wxString::FromUTF8("Password");
	i18n_en.translation[I18N_String::ENCODE] = wxString::FromUTF8("Encode");
	i18n_en.translation[I18N_String::DECODE] = wxString::FromUTF8("Decode");
	i18n_en.translation[I18N_String::DESCRIPTION] = wxString::FromUTF8("Encode or decode clipboard content with password");
	i18n_en.translation[I18N_String::ERR_TOO_SHORT] = wxString::FromUTF8("The clipboard content is incomplete!");
	i18n_en.translation[I18N_String::ERR_TAMPERED] = wxString::FromUTF8("The clipboard content has been tampered with!");

	static I18N_Translation i18n_de({ wxLANGUAGE_GERMAN, wxLANGUAGE_GERMAN_AUSTRIAN, wxLANGUAGE_GERMAN_BELGIUM,
		wxLANGUAGE_GERMAN_LIECHTENSTEIN, wxLANGUAGE_GERMAN_LUXEMBOURG, wxLANGUAGE_GERMAN_SWISS });
	i18n_de.translation[I18N_String::TITLE] = wxString::FromUTF8("MiniCrypt");
	i18n_de.translation[I18N_String::PASSWORD] = wxString::FromUTF8("Passwort");
	i18n_de.translation[I18N_String::ENCODE] = wxString::FromUTF8("Verschlüsseln");
	i18n_de.translation[I18N_String::DECODE] = wxString::FromUTF8("Entschlüsseln");
	i18n_de.translation[I18N_String::DESCRIPTION] = wxString::FromUTF8("Ver- oder Entschlüsselt den Inhalt der Zwischenablage");
	i18n_de.translation[I18N_String::ERR_TOO_SHORT] = wxString::FromUTF8("Der Inhalt der Zwischenablage ist unvollständig!");
	i18n_de.translation[I18N_String::ERR_TAMPERED] = wxString::FromUTF8("Der Inhalt der Zwischenablage ist ungültig oder wurde verändert!");

	static std::vector<I18N_Translation> i18n_table = { i18n_en, i18n_de };

	wxString result = i18n_en.translation.at(str);
	const wxLanguage lang_sys = static_cast<wxLanguage>(wxLocale::GetSystemLanguage());
	for (const I18N_Translation &i18n_entry : i18n_table)
		for (const wxLanguage lang_entry : i18n_entry.languages)
			if (lang_sys == lang_entry)
				for (const std::pair<const I18N_String, const wxString> i18n_trans : i18n_entry.translation)
					if (i18n_trans.first == str)
						result = i18n_trans.second;
	return result;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
