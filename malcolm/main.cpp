#include <iostream>
#include "Board.h"
#include "BoardSolver.h"
#include "BoardValidator.h"

int main()
{
	using value_t = int;

	malcolm::Board_t<value_t> board(9u, 9u);

	for (auto i{ 0u }; i < 9u; i++)
		for (auto j{ 0u }; j < 9u; j++)
			std::cin >> board.getRawBoardRef()[j][i];

	auto solved{ malcolm::BoardSolver_t<value_t>(board).getSolved() };

	std::cout << board << "\n\n\n\n";
	std::cout << solved << "\n\n\n";
	std::cout << malcolm::BoardValidator_t<value_t>::getInstance().validateBoard(solved);
}