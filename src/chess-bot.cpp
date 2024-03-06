#include <string>
#include <iostream>
#include <algorithm> // erasing slashes
#include <ctype.h>

void loadFEN(char (&board)[8][8], std::string fen)
{
  fen.erase(std::remove(fen.begin(), fen.end(), '/'), fen.end()); // remove slashes

  int fenIndex = 0;
  // populate board
  for (int j = 0; j < 8; j ++)
  {
    for (int i = 0; i < 8; i ++)
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
      fenIndex ++;
    }
  }
}

void printBoard(char (&board)[8][8])
{
  for (int j = 0; j < 8; j ++)
  {
    for (int i = 0; i < 8; i ++)
    {
      std::cout << "[" << board[j][i] << "]";
    }
    std::cout << std::endl;
  }
}

int main()
{
  std::string startFEN("rnbqkbnr/pppppppp/8/4k3/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  char board[8][8] = {{'0'}, {'0'}};

  loadFEN(board, startFEN);
  printBoard(board);

  return 0;
}