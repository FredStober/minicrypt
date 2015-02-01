#ifndef I18N_H
#define I18N_H

#include <wx/intl.h>

enum struct I18N_String
{
	TITLE, PASSWORD, ENCODE, DECODE, DESCRIPTION, ERR_TOO_SHORT, ERR_TAMPERED
};

const wxString myT(const I18N_String entry);

#endif
