#pragma once
#include "MyWidgets.h"

#include <list>
#include <set>
#include <string>
#include <array>
#include <limits>
#include <variant>

class ChessFrame : public wxFrame
{
public:


    ChessFrame();


    void initialiseBoard(char board[8][8]);


    void loadFEN(char (&board)[8][8], std::string fen);


    int checkSquareCandidacy(char board[8][8], char current, int x, int y, int dx, int dy, bool isWhite);


    std::set<std::array<int, 2>> getLegalMoves(char board[8][8], int x, int y);


    std::set<std::array<int, 4>> getAllLegalMoves(char board[8][8], bool isWhite);


    std::array<std::array<int, 5>, 2> getPieceCounts(char board[8][8]);


    float evaluatePosition(char board[8][8], std::array<std::array<int, 5>, 2> pieces, bool isWhite); /// ! will likely need to employ a search of its own


    char makeMove(char (&board)[8][8], std::array<int, 4> move, bool isWhite, bool onBoard); // returns captures


    std::array<std::variant<int, float>, 5> getBestMove(char board[8][8], std::array<std::array<int, 5>, 2> pieces, bool isWhite, int depth, bool decrementDepth); // last element is for eval


    void onMouseDown(wxMouseEvent& event);


private:

    wxGenericStaticBitmap* bgBmp;

    wxGenericStaticBitmap* piecesBmp[8][8];

    wxGenericStaticBitmap* overlayBmp[8][8];

    bool boardCreated = false;

    int globalWhitePieces[5] = { 8, 2, 2, 2, 1 };
    
    int globalBlackPieces[5] = { 8, 2, 2, 2, 1 };
};