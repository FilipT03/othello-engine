from utils import parse
from cpp_module import *
import math
import time

players_turn = True
players_color = "Black"
occupied = 0
colors = 0
last_played_row = -1
last_played_col = -1

table_values = {} # keeps track of the heuristic values for different tables
depth_table_values = {} # keeps track of the best heuristic value for different tables and certain depth

start_time = 0
computer_move_number = 0 # which move is it

def init():
    matrix = [[0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,2,1,0,0,0],
              [0,0,0,1,2,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0],
              [0,0,0,0,0,0,0,0]]
    
    global occupied, colors
    occupied, colors = parse.to_numbers(matrix)

# Tries to play the selected move. If not possible or it's not player's turn, returns None, None
def try_play_move(row, col) -> tuple:
    global players_turn, players_color, occupied, colors, last_played_row, last_played_col
    if not players_turn:
        return None, None
    if occupied & parse.MASK[row][col]:
        return None, None
    if not check_if_legal(occupied, colors, players_color == "Black", row, col):
        return None, None
    colors = play_move(occupied, colors, players_color == "Black", row, col)
    occupied |= parse.MASK[row][col]
    last_played_row = row
    last_played_col = col
    players_turn = False
    return occupied, colors

def play_computer_move():
    global occupied, colors, players_turn, computer_move_number
    global start_time
    computer_move_number += 1
    start_time = time.time()
    if computer_move_number <= 3:
        depth = 16
    elif computer_move_number <=6:
        depth = 12
    elif computer_move_number <=9:
        depth = 10
    else:
        depth = 8
    occupied, colors = minimax(occupied, colors, depth, -math.inf, math.inf, True, True)

    global bail
    bail = False
    it_took = time.time() - start_time
    print('It took me {:.10f}s'.format(it_took))

    players_turn = True
    return occupied, colors

def is_blacks_turn():
    if players_color == "White":
        return True
    else:
        return False

def get_heuristic_value(occupied, colors, blacks_turn):
    global table_values
    key = str(blacks_turn)+str(occupied)+str(colors)
    if key in table_values:
        return table_values[key]
    else:
        value = calculate_heuristic_value(occupied, colors, blacks_turn)
        table_values[key] = value
        return value

bail = False
def minimax(occupied, colors, depth, alpha, beta, maximizingPlayer, first_call = False):
    global bail, last_played_row, last_played_col, depth_table_values

    if not bail and time.time() - start_time > 2.9:
        #print("Bailed!")
        bail = True
    blacks_turn = is_blacks_turn()

    if (str(depth) + str(occupied) + str(colors)) in depth_table_values:
        return depth_table_values[str(depth) + str(occupied) + str(colors)]

    if depth == 0 or bail:
        return get_heuristic_value(occupied, colors, blacks_turn)
        
    pruned = False
    if maximizingPlayer:
        if first_call:
            best_occupied = -1
        max_eval = -math.inf
        new_moves = possible_moves(occupied, colors, blacks_turn)
        for i in range(8):
            for j in range(8):
                if new_moves & parse.MASK[i][j]:
                    new_colors = play_move(occupied, colors, blacks_turn, i, j)
                    new_occupied = occupied | parse.MASK[i][j]
                    eval = minimax(new_occupied, new_colors, depth-1, alpha, beta, False)
                    if first_call and eval > max_eval:
                        last_played_row = i
                        last_played_col = j
                        best_occupied = new_occupied
                        best_colors = new_colors
                    max_eval = max(max_eval, eval)
                    alpha = max(alpha, eval)
                    if beta <= alpha:
                        pruned = True
                        break
            if pruned:
                break
        if first_call and best_occupied != -1:
            return best_occupied, best_colors
        elif math.isfinite(max_eval):
            depth_table_values[str(depth) + str(occupied) + str(colors)] = max_eval
            return max_eval
        else:
            return get_heuristic_value(occupied, colors, blacks_turn)
    else:
        min_eval = math.inf
        new_moves = possible_moves(occupied, colors, blacks_turn)
        for i in range(8):
            for j in range(8):
                if new_moves & parse.MASK[i][j]:
                    new_colors = play_move(occupied, colors, blacks_turn, i, j)
                    new_occupied = occupied | parse.MASK[i][j]
                    eval = minimax(new_occupied, new_colors, depth-1, alpha, beta, True)
                    min_eval = min(min_eval, eval)
                    beta = min(beta, eval)
                    if beta <= alpha:
                        pruned = True
                        break
            if pruned:
                break
        if math.isfinite(min_eval):
            depth_table_values[str(depth) + str(occupied) + str(colors)] = min_eval
            return min_eval
        else:
            return get_heuristic_value(occupied, colors, blacks_turn)