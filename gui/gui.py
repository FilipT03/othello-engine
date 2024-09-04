import tkinter as tk
from utils import parse
from game import game
from cpp_module import count_color, possible_moves
 
def draw_start_screen():
    global window, bt1, bt2, bt3
    canvas.create_text(200, 40, text = "Reversi AI", font=('Helvetica','30'))
    canvas.create_text(200, 70, text = "By Filip Tot", font=('Helvetica','13'))
    bt1 = tk.Button(master = window, text = "White", command = pick_white, font=('Helvetica','24'))
    bt1.place(x=30,y=120)
    bt2 = tk.Button(master = window, text = "Black", command = pick_black, font=('Helvetica','24'))
    bt2.place(x=270,y=120)
    canvas.create_oval(170, 210, 230, 270, fill=game.players_color.lower())
    bt3 = tk.Button(master = window, text = "START", command = start, font=('Helvetica','30'))
    bt3.place(x=120,y=370)

def pick_black():
    global bt1, bt2, bt3
    game.players_color = "Black"
    bt1.destroy()
    bt2.destroy()
    bt3.destroy()
    draw_start_screen()
def pick_white():
    global bt1, bt2, bt3
    game.players_color = "White"
    bt1.destroy()
    bt2.destroy()
    bt3.destroy()
    draw_start_screen()

def print_test():
    print("test")

def draw_board(occupied, colors):
    global canvas
    canvas.delete("all")
    for row in range(8):
        for col in range(8):
            x1 = col * 50
            y1 = row * 50
            x2 = x1 + 50
            y2 = y1 + 50
 
            if not (occupied & parse.MASK[row][col]):
                canvas.create_rectangle(x1, y1, x2, y2, fill="green")
            elif (colors & parse.MASK[row][col]) == 0:
                canvas.create_oval(x1, y1, x2, y2, fill="black")
            else:
                canvas.create_oval(x1, y1, x2, y2, fill="white")
            if row == game.last_played_row and col == game.last_played_col:
                canvas.create_oval(x1+20, y1+20, x2-20, y2-20, fill="red")
    
    white = count_color(occupied, colors, True)
    black = count_color(occupied, colors, False)
    if game.players_color == "White":
        canvas.create_text(70, 430, text = f"Player (White): {white}")
        canvas.create_text(320, 430, text = f"Computer (Black): {black}")
    else:
        canvas.create_text(70, 430, text = f"Player (Black): {black}")
        canvas.create_text(320, 430, text = f"Computer (White): {white}")
    if (game.players_turn and game.players_color == "White") or (not game.players_turn and game.players_color == "Black"):
        canvas.create_oval(175, 405, 225, 455, fill="white")
    else:
        canvas.create_oval(175, 405, 225, 455, fill="black")
 
def draw_possible_moves(occupied, colors):
    global canvas
    moves = possible_moves(occupied, colors, game.players_color == "Black")
    for row in range(8):
        for col in range(8):
            if(moves & parse.MASK[row][col]):
                x1 = col * 50
                y1 = row * 50
                x2 = x1 + 50
                y2 = y1 + 50
                canvas.create_oval(x1+2, y1+2, x2-2, y2-2, outline="black", width=1.6)

def place_piece(row, col):
    global window
    if game.players_turn:
        occupied, colors = game.try_play_move(row, col)
        if occupied is not None:
            draw_board(occupied, colors)
            window.update()
            # is the game over? can the computer move?
            if possible_moves(occupied, colors, game.players_color != "Black") == 0:
                player_count = count_color(occupied, colors, game.players_color == "White")
                computer_count = count_color(occupied, colors, game.players_color == "Black")
                create_popup(player_count, computer_count)
                return
            # it can, then play the computers move
            occupied, colors = game.play_computer_move()
            draw_board(occupied, colors)
            draw_possible_moves(occupied, colors) # draw the empty circles
            # is the game over? can the player move?
            if possible_moves(occupied, colors, game.players_color == "Black") == 0:
                player_count = count_color(occupied, colors, game.players_color == "White")
                computer_count = count_color(occupied, colors, game.players_color == "Black")
                canvas.bind("<Button-1>", None) # the game is over so we should unbind the left click
                create_popup(player_count, computer_count)

def create_popup(player_count, computer_count):
    popup = tk.Tk()
    popup.wm_title("!")
    if(player_count > computer_count):
        label = tk.Label(popup, text="Player won!!!", font=('Helvetica', 10))
    elif(player_count < computer_count):
        label = tk.Label(popup, text="Computer won!!!", font=('Helvetica', 10))
    else:
        label = tk.Label(popup, text="It's a draw!!!", font=('Helvetica', 10))
    label.pack(side="top", fill="x", pady=10)
    btn = tk.Button(popup, text="Close", command=popup.destroy)
    btn.pack()
    popup.mainloop()
 
def handle_click(event):
    col = event.x // 50
    row = event.y // 50
    if row < 0 or row > 7 or col < 0 or col > 7:
        return
    place_piece(row, col)
 
def start():
    global bt1, bt2, bt3
    bt1.destroy()
    bt2.destroy()
    bt3.destroy()
    global canvas
    canvas.bind("<Button-1>", handle_click)
    draw_board(game.occupied, game.colors)
    if(game.players_color == "White"):
        game.players_turn = False
        game.play_computer_move()
        draw_board(game.occupied, game.colors)
    draw_possible_moves(game.occupied, game.colors)

 
def init():
    global window
    window = tk.Tk()
    window.title("AI Reversi")
    
    global canvas
    canvas = tk.Canvas(window, width=400, height=460)
    canvas.pack()

    draw_start_screen()
    window.mainloop()