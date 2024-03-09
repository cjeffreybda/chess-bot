#pragma once
#include "MyWidgets.h"

#include <list>
#include <set>
#include <string>
#include <array>

class ChessFrame : public wxFrame
{
public:


    ChessFrame();


    void initialiseBoard(char board[8][8]);


    void loadFEN(char(&board)[8][8], std::string fen);


    bool checkSquareCandidacy(char board[8][8], char current, int x, int y, int dx, int dy, bool isWhite);


    std::set<std::array<int, 2>> getLegalMoves(char board[8][8], int x, int y);


    std::set<std::array<int, 4>> getAllLegalMoves(char board[8][8]);


    void onMouseDown(wxMouseEvent& event);


private:

    wxGenericStaticBitmap* bg;

    wxGenericStaticBitmap* pieces[8][8];

    wxGenericStaticBitmap* overlay[8][8];

    bool boardCreated = false;
};