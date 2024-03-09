#include "ChessFrame.h"


void ChessFrame::loadFEN(char(&board)[8][8], std::string fen)
{
    //fen.erase(std::remove(fen.begin(), fen.end(), '/'), fen.end()); // remove slashes

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
            }
            else
            {
                board[j][i] = current;
            }
            fenIndex++;
        }
        fenIndex++;
    }
}


ChessFrame::ChessFrame() : wxFrame(NULL, wxID_ANY, "Stonk Fitch")
{
    ChessFrame::SetMinSize(wxSize(816, 838)); // header is 38 pixels, side is 16, 
    ChessFrame::SetMaxSize(wxSize(816, 838));

    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    char board[8][8] = { {'0'}, {'0'} };

    ChessFrame::loadFEN(board, startFEN);
    ChessFrame::initialiseBoard(board);

    this->Bind(wxEVT_LEFT_DOWN, &ChessFrame::onMouseDown, this);

    std::set<std::array<int, 4>> allMoves = ChessFrame::getAllLegalMoves(board);

    std::set<std::array<int, 4>>::iterator itr1;
    std::array<int, 4>::iterator itr2;

    for (auto& arr : allMoves) {
        // Iterate over each element of the array
        std::string out = "";
        for (auto& element : arr) {
            // Output each element followed by a space 
            out += std::to_string(element);
            
        }
        wxLogDebug(wxFormatString(out));
    }
}


std::set<std::array<int, 2>> ChessFrame::getLegalMoves(char board[8][8], int x, int y)
{
    if (board[y][x] == NULL) // hopefully saves time ?
    {
        return {};
    }

    char current = tolower(board[y][x]);
    bool isWhite = (!(board[y][x] == current));
    std::set<std::array<int, 2>> legalMoves;

    // pawns
    if (current == 'p') // ! NEED TO ADD EN PASSANT
    {
        for (int j = 1; j <= 2; j++)
        {
            int dir = isWhite ? -1 : 1;
            int target = std::max(std::min(y + dir * j, 8), 0);
            char targetVal = board[target][x];
            std::array<int, 2> newEntry = { target, x };

            if (targetVal == NULL)
            {
                legalMoves.insert(newEntry);
            }

            /*if (targetVal == NULL && legalMoves.find(newEntry) != legalMoves.end()) // not needed bacause set??
            {
                legalMoves.insert(newEntry);
            }*/

            int start = isWhite ? 6 : 1;

            if (y != start) // only allow double moves for pawns on their start square
            {
                j++;
            }

            for (int i = -1; i <= 1 && i != 0; i++) // captures
            {
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, dir, isWhite))
                {
                    legalMoves.insert({ y + dir, x + i });
                }
            }
        }
    }
    else if (current == 'n')
    {
        for (int j = -2; j <= 2 && j != 0; j++)
        {
            for (int i = -2; i <= 2 && abs(i) + abs(j) == 3; i++)
            {
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite))
                {
                    legalMoves.insert({ y + j, x + i });
                }
            }
        }
    }
    else if (current == 'b' || current == 'r' || current == 'q')
    {
        if (current == 'b' || current == 'q')
        {
            for (int j = -1; j >= -7; j--) // north west
            {
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, j, j, isWhite)) //! UPDATE CHECK TO END RIGHT AFTER CAPTURE
                {
                    legalMoves.insert({ y + j, x + j });
                }
                else
                {
                    j -= 8;
                }
            }

            for (int j = -7; j <= 7; j++)
            {
                for (int i = -7; i <= 7 && abs(i) == abs(j) && i != 0; i++)
                {
                    if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite))
                    {
                        legalMoves.insert({ y + j, x + i });
                    }
                }
            }
        }
        if (current == 'r' || current == 'q')
        {
            for (int j = -7; j <= 7; j++)
            {
                for (int i = -7; i <= 7 && (j != 0 != i != 0); i++)
                {
                    if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite))
                    {
                        legalMoves.insert({ y + j, x + i });
                    }
                }
            }
        }
    }
    else if (current == 'k')
    {
        for (int j = -1; j < 1; j++)
        {
            for (int i = -1; i < 1 && (j != 0 != i != 0); i++)
            {
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite))
                {
                    legalMoves.insert({ y + j, x + i });
                }
            }
        }
    }

    return legalMoves;
}


std::set<std::array<int, 4>> ChessFrame::getAllLegalMoves(char board[8][8])
{
    std::set<std::array<int, 4>> allMoves;

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (board[j][i] == NULL)
            {
                continue;
            }

            std::set<std::array<int, 2>> currMoves = getLegalMoves(board, i, j);

            std::set<std::array<int, 2>>::iterator itr;

            for (itr = currMoves.begin(); itr != currMoves.end(); itr++)
            {
                allMoves.insert({ j, i, (*itr)[0], (*itr)[1]});
            }
        }
    }

    return allMoves;
}


bool ChessFrame::checkSquareCandidacy(char board[8][8], char current, int x, int y, int dx, int dy, bool isWhite)
{
    if (y + dy < 0 || y + dy > 7 || x + dx < 0 || x + dx > 7) // remove out-of-bounds spaces
    {
        return false;
    }

    char target = board[y + dy][x + dx];
    bool targetIsWhite = (!(target == tolower(target)));
    bool targetIsOpponent = (isWhite && !targetIsWhite || !isWhite && targetIsWhite);

    if (target != NULL && !targetIsOpponent)
    {
        return false;
    }

    return true;
}


void ChessFrame::initialiseBoard(char board[8][8])
{
    wxPNGHandler* handler = new wxPNGHandler;
    wxImage::AddHandler(handler);

    bg = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap("E:/repos/chess-bot/images/board.png", wxBITMAP_TYPE_PNG), wxPoint(0, 0), wxSize(800, 800));

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            char current = board[j][i];

            if (current == NULL)
            {
                continue;
            }

            bool isWhite = (current == tolower(current)) ? false : true;
            std::string prefix = isWhite ? "w" : "b";

            std::string currStr(1, tolower(current));
            std::string path = "E:/repos/chess-bot/images/" + prefix + currStr + ".png";

            pieces[j][i] = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap(path, wxBITMAP_TYPE_PNG), wxPoint(100 * i, 100 * j), wxSize(100, 100));
        }
    }

    boardCreated = true;
}


void ChessFrame::onMouseDown(wxMouseEvent& event)
{
    if (!boardCreated)
    {
        return;
    }

    wxPoint clickPos = event.GetPosition();

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            bool inX = (clickPos.x >= i * 100 && clickPos.x < (i + 1) * 100);
            bool inY = (clickPos.y >= j * 100 && clickPos.y < (j + 1) * 100);

            if (inX && inY)
            {
                overlay[j][i] = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap("E:/repos/chess-bot/images/select.png", wxBITMAP_TYPE_PNG), wxPoint(100 * i, 100 * j), wxSize(100, 100));
            }
        }
    }
}