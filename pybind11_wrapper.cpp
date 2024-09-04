#include <pybind11/pybind11.h>
#include "calculations.cpp"

PYBIND11_MODULE(cpp_module, m) {
    m.doc() = "Cpp module for faster calculations";
    m.def("possible_moves", &PossibleMoves, "Returns a number representation of all possible moves");
    m.def("play_move", &PlayMove, "Plays the move for the given player and returns colors number");
    m.def("check_if_legal", &CheckIfLegal, "Check if the move for the given player is legal");
    m.def("calculate_heuristic_value", &CalculateHeuristicValue, "Calculates how optimal the board is for the given player");
    m.def("count_color", &CountColor, "Counts the given color on the board");
}
