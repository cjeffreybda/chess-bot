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

    std::string startFEN = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1";
    std::string tookFEN = "rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1";
    std::string notTookFEN = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPP11PPP/RNBQKBNR w KQkq - 0 1";
    std::string castleString = "KQkq";

    char board[8][8] = { {'0'}, {'0'} };
    

    ChessFrame::loadFEN(board, startFEN);
    ChessFrame::initialiseBoard(board);
    bool startAsWhite = false;

    std::array<std::array<int, 5>, 2> startPieces;
    startPieces[0] = { 8, 2, 2, 2, 1 };
    startPieces[1] = { 8, 2, 2, 2, 1 };

    std::array<std::array<int, 5>, 2> tookPieces;
    tookPieces[0] = { 8, 2, 2, 2, 1 };
    tookPieces[1] = { 8, 2, 2, 2, 1 };

    char tookBoard[8][8] = { {'0'}, {'0'} };
    char notTookBoard[8][8] = { {'0'},{'0'} };

    ChessFrame::loadFEN(tookBoard, tookFEN);
    ChessFrame::loadFEN(notTookBoard, notTookFEN);

    float tookEval = evaluatePosition(tookBoard, tookPieces, true);
    float notTookEval = evaluatePosition(notTookBoard, startPieces, true);

    wxLogDebug(wxFormatString("took = " + std::to_string(tookEval)));
    wxLogDebug(wxFormatString("not took = " + std::to_string(notTookEval)));

    this->Bind(wxEVT_LEFT_DOWN, &ChessFrame::onMouseDown, this);

    std::set<std::array<int, 4>> allMoves = ChessFrame::getAllLegalMoves(board, startAsWhite);
    std::array<std::variant<int, float>, 5> bestMove = ChessFrame::getBestMove(board, startPieces, startAsWhite, 1, false);

    for (auto arr : allMoves) {
        // Iterate over each element of the array
        std::string out = "";
        for (auto element : arr) {
            // Output each element followed by a space 
            out += std::to_string(element);
            
        }
        wxLogDebug(wxFormatString(out));
    }

    wxLogDebug(wxFormatString("Best move " + std::to_string(std::get<int>(bestMove[0])) + std::to_string(std::get<int>(bestMove[1])) + std::to_string(std::get<int>(bestMove[2])) + std::to_string(std::get<int>(bestMove[3])) + ": " + std::to_string(std::get<float>(bestMove[4]))));

    ChessFrame::makeMove(board, { std::get<int>(bestMove[0]), std::get<int>(bestMove[1]), std::get<int>(bestMove[2]), std::get<int>(bestMove[3])}, startAsWhite, true);
    
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

    if (current == 'p') /// ! NEED TO ADD EN PASSANT
    {
        int dir = isWhite ? -1 : 1;
        for (int j = 1; j <= 2; j++)
        {
            if (ChessFrame::checkSquareCandidacy(board, current, x, y, 0, j * dir, isWhite) == 0)
            {
                legalMoves.insert({ y + j * dir, x});
            }

            int start = isWhite ? 6 : 1;

            if (y != start) // only allow double moves for pawns on their start square
            {
                j++;
            }
        }
        for (int i = -1; i <= 1; i++) // captures
        {
            if (i == 0)
            {
                continue;
            }
            if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, dir, isWhite) == 1)
            {
                legalMoves.insert({ y + dir, x + i });
            }
        }
    }
    else if (current == 'n')
    {
        for (int j = -2; j <= 2; j++)
        {
            for (int i = -2; i <= 2; i++)
            {
                if (abs(i) + abs(j) != 3)
                {
                    continue;
                }
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite) < 2)
                {
                    legalMoves.insert({ y + j, x + i });
                }
            }
        }
    }
    else if (current == 'b' || current == 'r' || current == 'q')
    {
        if (current != 'r') // bishop
        {

            for (int k = -1; k < 2; k++) // vertical flip
            {
                if (k == 0)
                {
                    continue;
                }
                for (int j = -1; j < 2; j++) // horizontal flip
                {
                    if (j == 0)
                    {
                        continue;
                    }
                    for (int i = 1; i < 8; i++) // diagonals
                    {
                        int targetCandidacy = ChessFrame::checkSquareCandidacy(board, current, x, y, i * j, i * k, isWhite);
                        if (targetCandidacy < 2)
                        {
                            legalMoves.insert({ y + i * k, x + i * j });
                        }

                        if (targetCandidacy > 0)
                        {
                            i += 8;
                        }
                    }
                }
            }
        }
        if (current != 'b') // rook
        {
            for (int k = -1; k < 2; k++) // flip
            {
                if (k == 0)
                {
                    continue;
                }
                for (int i = 1; i < 8; i++) // horizontal
                {
                    int targetCandidacy = ChessFrame::checkSquareCandidacy(board, current, x, y, i * k, 0, isWhite);
                    if (targetCandidacy < 2)
                    {
                        legalMoves.insert({ y, x + i * k });
                    }

                    if (targetCandidacy > 0)
                    {
                        i += 8;
                    }
                }
            }

            for (int k = -1; k < 2; k++) // flip
            {
                if (k == 0)
                {
                    continue;
                }
                for (int j = 1; j < 8; j++) // vertical
                {
                    int targetCandidacy = ChessFrame::checkSquareCandidacy(board, current, x, y, 0, j * k, isWhite);
                    if (targetCandidacy < 2)
                    {
                        legalMoves.insert({ y + j * k, x });
                    }

                    if (targetCandidacy > 0)
                    {
                        j += 8;
                    }
                }
            }
        }
    }
    else if (current == 'k') /// ! NEED TO IMPLEMENT CASTLING AND CHECKS
    {
        for (int j = -1; j <= 1; j++)
        {
            for (int i = -1; i <= 1; i++)
            {
                if (j == 0 && i == 0)
                {
                    continue;
                }
                if (ChessFrame::checkSquareCandidacy(board, current, x, y, i, j, isWhite) < 2)
                {
                    legalMoves.insert({ y + j, x + i });
                }
            }
        }
    }

    return legalMoves;
}


std::set<std::array<int, 4>> ChessFrame::getAllLegalMoves(char board[8][8], bool isWhite)
{
    std::set<std::array<int, 4>> allMoves;

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            char current = board[j][i];

            if (current == NULL || isWhite == (current == tolower(current))) // passes blanks and enemy pieces
            {
                continue;
            }

            std::set<std::array<int, 2>> currMoves = getLegalMoves(board, i, j);

            for (auto itr : currMoves)
            {
                allMoves.insert({ j, i, itr[0], itr[1]});
            }
        }
    }

    return allMoves;
}

// 0: free 1: capture 2: blocked
int ChessFrame::checkSquareCandidacy(char board[8][8], char current, int x, int y, int dx, int dy, bool isWhite)
{
    if (y + dy < 0 || y + dy > 7 || x + dx < 0 || x + dx > 7) // remove out-of-bounds spaces
    {
        return 2;
    }

    char target = board[y + dy][x + dx];
    bool targetIsWhite = (target != tolower(target));
    bool targetIsOpponent = (isWhite != targetIsWhite); // exclusive or

    if (target == NULL)
    {
        return 0;
    }
    else if (targetIsOpponent)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}


std::array<std::array<int, 5>, 2> ChessFrame::getPieceCounts(char board[8][8])
{
    std::array<std::array<int, 5>, 2> pieces = {};

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (board[j][i] == NULL)
            {
                continue;
            }

            char current = board[j][i];
            bool color = (current == tolower(current));

            switch (tolower(current))
            {
            case 'p':
                pieces[color][0]++;
                break;
            case 'n':
                pieces[color][1]++;
                break;
            case 'b':
                pieces[color][2]++;
                break;
            case 'r':
                pieces[color][3]++;
                break;
            case 'q':
                pieces[color][4]++;
            }
        }
    }

    return pieces;
}


float ChessFrame::evaluatePosition(char board[8][8], std::array<std::array<int, 5>, 2> pieces, bool isWhite)
{
    float whiteMaterial = pieces[0][0] + 3 * (pieces[0][1] + pieces[0][2]) + 5 * pieces[0][3] + 9 * pieces[0][4];
    float blackMaterial = pieces[1][0] + 3 * (pieces[1][1] + pieces[1][2]) + 5 * pieces[1][3] + 9 * pieces[1][4];

    float materialWeight = 50;

    float whiteScore = materialWeight * whiteMaterial;
    float blackScore = materialWeight * blackMaterial;

    if (blackMaterial < 39)
    {
        wxLogDebug(wxFormatString("HERE"));
    }

    for (int r = 0; r < 8; r++)
    {
        for (int f = 0; f < 8; f++)
        {
            char current = tolower(board[r][f]);
            bool currentIsWhite = (current == tolower(board[r][f]));

            if (current == 'p')
            {
                float fileScore = 0;
                float rankScore = 0;

                float fileWeight = 1.2; // P.O.O.M.A.
                float rankWeight = 1.26;
                float overallWeight = 0.1;

                fileScore = -f * (f - 7) / 12.25;

                if (currentIsWhite)
                {
                    rankScore = 6 - r;
                    whiteScore += overallWeight * (pow(fileScore, fileWeight) * pow(rankScore, rankWeight));
                }
                else
                {
                    rankScore = r - 1;
                    blackScore += overallWeight * (pow(fileScore, fileWeight) * pow(rankScore, rankWeight));
                }
            }
            else if (current == 'n')
            {
                float fileScore = -f * (f - 7) / 12.25;
                float rankScore = -r * (r - 7) / 12.25;

                float fileWeight = 1.8;
                float rankWeight = 1;
                float overallWeight = 1;

                float overallScore = overallWeight * (pow(fileScore, fileWeight) + pow(rankScore, rankWeight));

                if (currentIsWhite)
                {
                    whiteScore += overallScore;
                }
                else
                {
                    blackScore += overallScore;
                }
            }
        }
    }

    float returnVal = (whiteScore - blackScore) * (isWhite ? 1 : -1);

    if (board[3][3] == 'P')
    {
        wxLogDebug(wxFormatString("white took"));
        wxLogDebug(wxFormatString("Hello"));
        
    }
    else if (board[3][4] == 'p')
    {
        wxLogDebug(wxFormatString("4"));
        wxLogDebug(wxFormatString("Hello"));
    }

    return returnVal;
}


char ChessFrame::makeMove(char (&board)[8][8], std::array<int, 4> move, bool isWhite, bool onBoard)
{
    char captured = board[move[2]][move[3]];

    board[move[2]][move[3]] = board[move[0]][move[1]];
    board[move[0]][move[1]] = NULL;

    if (onBoard)
    {
        std::string prefix = isWhite ? "w" : "b";

        std::string currStr(1, tolower(board[move[2]][move[3]]));
        std::string path = "E:/repos/chess-bot/images/" + prefix + currStr + ".png";

        piecesBmp[move[0]][move[1]]->SetBitmap(wxNullBitmap);
        piecesBmp[move[2]][move[3]] = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap(path, wxBITMAP_TYPE_PNG), wxPoint(100 * move[3], 100 * move[2]), wxSize(100, 100));
    }

    return captured;
}


std::array<std::variant<int, float>, 5> ChessFrame::getBestMove(char board[8][8], std::array<std::array<int, 5>, 2> pieces, bool isWhite, int depth, bool decrementDepth)
{
    std::set<std::array<int, 4>> moves = getAllLegalMoves(board, isWhite);

    float maxEval = NULL;
    std::array<int, 4> bestMove = {};

    for (auto move : moves)
    {
        char newBoard[8][8];
        std::copy(&board[0][0], &board[0][0] + 8 * 8, &newBoard[0][0]);

        char captured = ChessFrame::makeMove(newBoard, move, isWhite, false);
        float eval = 0;

        std::array<std::array<int, 5>, 2> newPieces;

        for (int i = 0; i < 5; i++)
        {
            newPieces[0][i] = pieces[0][i];
            newPieces[1][i] = pieces[1][i];
        }

        if (captured != NULL)
        {
            bool capturedIsWhite = (captured != tolower(captured));

            switch (tolower(captured))
            {
            case 'p':
                (newPieces[!capturedIsWhite])[0]--;
                break;
            case 'n':
                (newPieces[!capturedIsWhite])[1]--;
                break;
            case 'b':
                (newPieces[!capturedIsWhite])[2]--;
                break;
            case 'r':
                (newPieces[!capturedIsWhite])[3]--;
                break;
            case 'q':
                (newPieces[!capturedIsWhite])[4]--;
            }
        }
        
        if (depth == 1 && decrementDepth)
        {
            eval = ChessFrame::evaluatePosition(newBoard, newPieces, isWhite);
        }
        else
        {
            auto opponentsMove = ChessFrame::getBestMove(newBoard, newPieces, !isWhite, depth - decrementDepth, !decrementDepth);
            eval = -std::get<float>(opponentsMove[4]);
        }

        if (maxEval == NULL || eval > maxEval)
        {
            maxEval = eval;
            bestMove = move;
        }
        wxLogDebug(wxFormatString("move " + std::to_string(move[0]) + std::to_string(move[1]) + std::to_string(move[2]) + std::to_string(move[3]) + " at depth " + std::to_string(depth)));
    }

    return { bestMove[0], bestMove[1], bestMove[2], bestMove[3], maxEval };
}


void ChessFrame::initialiseBoard(char board[8][8])
{
    wxPNGHandler* handler = new wxPNGHandler;
    wxImage::AddHandler(handler);

    bgBmp = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap("E:/repos/chess-bot/images/board.png", wxBITMAP_TYPE_PNG), wxPoint(0, 0), wxSize(800, 800));

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

            piecesBmp[j][i] = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap(path, wxBITMAP_TYPE_PNG), wxPoint(100 * i, 100 * j), wxSize(100, 100));
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
                overlayBmp[j][i] = new wxGenericStaticBitmap(this, wxID_ANY, wxBitmap("E:/repos/chess-bot/images/select.png", wxBITMAP_TYPE_PNG), wxPoint(100 * i, 100 * j), wxSize(100, 100));
            }
        }
    }
}