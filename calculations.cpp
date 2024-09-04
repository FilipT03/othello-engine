#include<iostream>
#include<pybind11/pybind11.h>

// if the color is white(1) then it should not be blacksTurn(0)
#define isFriendly(colors, mask, blacksTurn) ((bool)(colors & mask) != blacksTurn)

using namespace std;
namespace py = pybind11;

const uint64_t MASK[][8] = {
{0x0000000000000001,0x0000000000000002,0x0000000000000004,0x0000000000000008,0x0000000000000010,0x0000000000000020,0x0000000000000040,0x0000000000000080},
{0x0000000000000100,0x0000000000000200,0x0000000000000400,0x0000000000000800,0x0000000000001000,0x0000000000002000,0x0000000000004000,0x0000000000008000},
{0x0000000000010000,0x0000000000020000,0x0000000000040000,0x0000000000080000,0x0000000000100000,0x0000000000200000,0x0000000000400000,0x0000000000800000},
{0x0000000001000000,0x0000000002000000,0x0000000004000000,0x0000000008000000,0x0000000010000000,0x0000000020000000,0x0000000040000000,0x0000000080000000},
{0x0000000100000000,0x0000000200000000,0x0000000400000000,0x0000000800000000,0x0000001000000000,0x0000002000000000,0x0000004000000000,0x0000008000000000},
{0x0000010000000000,0x0000020000000000,0x0000040000000000,0x0000080000000000,0x0000100000000000,0x0000200000000000,0x0000400000000000,0x0000800000000000},
{0x0001000000000000,0x0002000000000000,0x0004000000000000,0x0008000000000000,0x0010000000000000,0x0020000000000000,0x0040000000000000,0x0080000000000000},
{0x0100000000000000,0x0200000000000000,0x0400000000000000,0x0800000000000000,0x1000000000000000,0x2000000000000000,0x4000000000000000,0x8000000000000000}
};

const int dr[] = {-1,-1, 0, 1, 1, 1, 0,-1,-1};
const int dc[] = { 0, 1, 1, 1, 0,-1,-1,-1, 0};
/*  701 
    6x2
    543  */
bool CheckIfLegal(uint64_t occupied, uint64_t colors, bool blacksTurn, int r, int c)
{
    int rNew, cNew;
    uint64_t mask;
    for(int k=0;k<8;k++){
        rNew = r+dr[k];
        cNew = c+dc[k];
        mask = MASK[rNew][cNew];
        // if on the next space there is no piece or the piece is friendly, skip this direction
        if(!(occupied & mask) || isFriendly(colors, mask, blacksTurn))
            continue;
        do{
            rNew = rNew+dr[k];
            cNew = cNew+dc[k];
            if(rNew < 0 || rNew > 7 || cNew < 0 || cNew > 7) // we are out of bounds
                break;
            mask = MASK[rNew][cNew];
            if(!(occupied & mask)) // there is an empty space
                break;
            if(isFriendly(colors, mask, blacksTurn)) // there is a friendly piece
                return true;
        }while(true);
    }
    return false;
}

// White = true,   Black = false
uint64_t PossibleMoves(uint64_t occupied, uint64_t colors, bool blacksTurn)
{
    uint64_t newMoves = 0;
    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
            if((occupied & MASK[i][j]) == 0) // this spot isn't occupied so it qualifies
                if(CheckIfLegal(occupied, colors, blacksTurn, i, j))
                    newMoves |= MASK[i][j]; // mark the space
    return newMoves;
}

// Plays the move for the given player and returns colors number
uint64_t PlayMove(uint64_t occupied, uint64_t colors, bool blacksTurn, int r, int c)
{
    uint64_t newColors = colors;
    int rNew, cNew;
    uint64_t mask;
    bool valid;
    for(int k=0;k<8;k++){
        rNew = r+dr[k];
        cNew = c+dc[k];
        mask = MASK[rNew][cNew];
        // first we check if the given direction is valid
        valid = false;
        // if on the next space there is no piece or the piece is friendly, skip this direction
        if(!(occupied & mask) || isFriendly(colors, mask, blacksTurn))
            continue;
        do{
            rNew = rNew+dr[k];
            cNew = cNew+dc[k];
            if(rNew < 0 || rNew > 7 || cNew < 0 || cNew > 7) // we are out of bounds
                break;
            mask = MASK[rNew][cNew];
            if(!(occupied & mask)) // there is an empty space
                break;
            if(isFriendly(colors, mask, blacksTurn)){ // there is a friendly piece
                valid = true;
                break;
            }
        }while(true);
        if(!valid)
            continue;
        // the move is valid so we fill in the space to that piece with friendly pieces
        rNew = r;
        cNew = c;
        if(!blacksTurn) // if it's white's turn, set the color on the starting space to white(1)
            newColors |= MASK[r][c];
        do{
            rNew = rNew+dr[k];
            cNew = cNew+dc[k];
            mask = MASK[rNew][cNew];
            if(isFriendly(colors, mask, blacksTurn)) // there is a friendly piece
                break;
            newColors ^= mask; // flip the pieces
        }while(true);
    }
    return newColors;
}

int CountLegalMoves(uint64_t occupied, uint64_t colors, bool blacksTurn)
{
    int count = 0;
    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
            if((occupied & MASK[i][j]) == 0) // this spot isn't occupied so it qualifies
                if(CheckIfLegal(occupied, colors, blacksTurn, i, j))
                    count++;
    return count;
}

int cornerR[] = {7, 0, 0, 7};
int cornerC[] = {0, 0, 7, 7};
/*  1x2 
    xxx
    0x3  */

int heuristicMatrix[][8] = {
    {20,-3,11, 8, 8,11,-3,20},
    {-3,-7,-4, 1, 1,-4,-7,-3},
    {11,-4, 2, 2, 2, 2,-4,11},
    { 8, 1, 2,-3,-3, 2, 1, 8},
    { 8, 1, 2,-3,-3, 2, 1, 8},
    {11,-4, 2, 2, 2, 2,-4,11},
    {-3,-7,-4, 1, 1,-4,-7,-3},
    {20,-3,11, 8, 8,11,-3,20}};

// Calculates how optimal the board is for the given player
double CalculateHeuristicValue(uint64_t occupied, uint64_t colors, bool blacksTurn)
{
    uint64_t mask;
    int friendlyTiles = 0, oppTiles = 0, friendlyFrontTiles = 0, oppFrontTiles = 0;
    int rNew, cNew;
    double p = 0, c = 0, l = 0, m = 0, f = 0, d = 0;

    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++){
            mask = MASK[i][j];
            if(occupied & mask){
                if(isFriendly(colors, mask, blacksTurn)){
                    d += heuristicMatrix[i][j];
                    friendlyTiles++;
                }
                else{
                    d -= heuristicMatrix[i][j];
                    oppTiles++;
                }
                for(int k=0; k<8; k++){
                    rNew = i+dr[k];
                    cNew = j+dc[k];
                    if(rNew < 0 || rNew > 7 || cNew < 0 || cNew > 7)
                        continue;
                    if(!(occupied & MASK[rNew][cNew])){
                        if(isFriendly(colors, mask, blacksTurn))
                            friendlyFrontTiles++;
                        else
                            oppFrontTiles++;
                        break;
                    }
                }
            }
        }
    
    if(friendlyTiles > oppTiles)
		p = (100.0 * friendlyTiles) / (friendlyTiles + oppTiles);
	else if(friendlyTiles < oppTiles)
		p = -(100.0 * oppTiles) / (friendlyTiles + oppTiles);
	else p = 0;

	if(friendlyFrontTiles > oppFrontTiles)
		f = -(100.0 * friendlyFrontTiles) / (friendlyFrontTiles + oppFrontTiles);
	else if(friendlyFrontTiles < oppFrontTiles)
		f = (100.0 * oppFrontTiles) / (friendlyFrontTiles + oppFrontTiles);
	else f = 0;

    friendlyTiles = oppTiles = 0;
	for(int k=0; k<4; k++)
        if(occupied && MASK[cornerR[k]][cornerC[k]]) // if occupied increase the piece counter
            isFriendly(colors, MASK[cornerR[k]][cornerC[k]], blacksTurn) ? friendlyTiles++ : oppTiles++;
	c = 25 * (friendlyTiles - oppTiles);


    friendlyTiles = oppTiles = 0;
	for(int k=0; k<4; k++) // for each corner
        if(!(occupied && MASK[cornerR[k]][cornerC[k]])) // if it's not occupied
            for(int k2 = 2*k; k2 <= 2*k+2; k2++) // for each adjacent piece 
                if(occupied && MASK[cornerR[k]+dr[k2]][cornerC[k]+dc[k2]]) // if occupied increase the piece counter
                    isFriendly(colors, MASK[cornerR[k]+dr[k2]][cornerC[k]+dc[k2]], blacksTurn) ? friendlyTiles++ : oppTiles++;
	l = -12.5 * (friendlyTiles - oppTiles);

    friendlyTiles = CountLegalMoves(occupied, colors, blacksTurn);
    oppTiles      = CountLegalMoves(occupied, colors,!blacksTurn);
	if(friendlyTiles > oppTiles)
		m = (100.0 * friendlyTiles) / (friendlyTiles + oppTiles);
	else if(friendlyTiles < oppTiles)
		m = -(100.0 * oppTiles) / (friendlyTiles + oppTiles);
	else m = 0;

	double score = (10 * p) + (801.724 * c) + (382.026 * l) + (78.922 * m) + (74.396 * f) + (10 * d);
	return score;
}

int CountColor(uint64_t occupied, uint64_t colors, bool white){
    int count = 0;
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            if(occupied & MASK[i][j])
                if((bool)(colors & MASK[i][j]) == white) // colors has 1 if it's white
                    count++;
    return count;
}
