#include <string>
#include <iostream>
#include <stdio.h>
#include <algorithm> // erasing slashes
#include <ctype.h>
#include <list>

#include <wx/wx.h>

// std::list<char[2]> getLegalMoves(char (&board)[8][8], int pos[2])
// {
//   std::list<char[2]> retList;
//   char current = board[pos[0]][pos[1]];
//   bool isWhite = (tolower(current) != current);

//   switch (tolower(current))
//   {
//     case 'p':

//       if (board[pos[0]][pos[1] + (isWhite ? -1 : 1)] == '0')
//       {

//       }
//     case 'n':

//     case 'b':

//     case 'r':

//     case 'q':

//     case 'k':

//   }
// }

void printBoard(char (&board)[8][8])
{
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      std::cout << "[" << board[j][i] << "]";
    }
    std::cout << std::endl;
  }
}

// int main()
// {
//   std::string startFEN = "rnbqkbnr/pppppppp/8/4k3/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

//   char board[8][8] = {{'0'}, {'0'}};

//   loadFEN(board, startFEN);
//   printBoard(board);

//   return 0;
// }

class ChessApp : public wxApp
{
  public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(ChessApp);

class ChessFrame : public wxFrame
{
public:
  ChessFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

  void drawBoard(char board[8][8]);

  void loadFEN(char (&board)[8][8], std::string fen);

private:
  wxStaticBitmap *bg;
  wxStaticBitmap *pieces;
  wxStaticBitmap *overlay;
};

void ChessFrame::loadFEN(char (&board)[8][8], std::string fen)
{
  fen.erase(std::remove(fen.begin(), fen.end(), '/'), fen.end()); // remove slashes

  int fenIndex = 0;
  // populate board
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      char current = fen.at(fenIndex);

      if (isdigit(current))
      {
        i += current - '0' - 1; // weird hack to convert char to int
        board[j][i] = '0';
      }
      else
      {
        board[j][i] = current;
      }
      fenIndex++;
    }
  }
}

bool ChessApp::OnInit()
{
  ChessFrame *frame = new ChessFrame("Stonk Fitch", wxDefaultPosition, wxDefaultSize);
  frame->Show(true);
  return true;
}

ChessFrame::ChessFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
  ChessFrame::SetMinSize(wxSize(800, 838)); // header is 38 pixels
  ChessFrame::SetMaxSize(wxSize(800, 838));

  std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  char board[8][8] = {{'0'}, {'0'}};

  ChessFrame::loadFEN(board, startFEN);
  ChessFrame::drawBoard(board);
  pieces->SetBitmap(wxNullBitmap);
}

void ChessFrame::drawBoard(char board[8][8])
{
  wxPNGHandler *handler = new wxPNGHandler;
  wxImage::AddHandler(handler);

  bg = new wxStaticBitmap(this, wxID_ANY, wxBitmap("images/board.png", wxBITMAP_TYPE_PNG), wxPoint(0, 0), wxSize(800, 800));

  for (int j = 0; j < 8; j ++)
  {
    for (int i = 0; i < 8; i ++)
    {
      char current = board[j][i];

      if (isdigit(current))
      {
        continue;
      }

      bool isWhite = (current == tolower(current)) ? false : true;

      std::string prefix = isWhite ? "w" : "b";
      std::string currStr(1, tolower(current));
      std::string path = "images/" + prefix + currStr + ".png";

      pieces = new wxStaticBitmap(this, 100001, wxBitmap(path, wxBITMAP_TYPE_PNG), wxPoint(100*i, 100*j), wxSize(100, 100));
    }
  }
}