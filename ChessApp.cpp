#include "ChessApp.h"


wxIMPLEMENT_APP(ChessApp);


bool ChessApp::OnInit()
{
	ChessFrame* frame = new ChessFrame();
	frame->Show(true);
	return true;
}