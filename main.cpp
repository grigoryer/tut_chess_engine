#include <iostream>
#include <stdio.h>

using std::cout;
using U64 = unsigned long long;


/*********************************\ 
 
         Bit manipulations

 ==================================
 \*********************************/       
 

 // set get pop  macros 
 #define get_bit(bitboard, square) (bitboard & (1ULL << square))
 #define set_bit(bitboard, square) (bitboard |= (1ULL << square))
 #define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

 // board squares
 enum{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
 };

 //sides to move (colors)
 enum{white, black};

 /*
 "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
 "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
 "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
 "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
 "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
 "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
 "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
 "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
 
 */

 //print bitboard
 void print_bitboard(U64 bitboard){
    printf("\n");

    //loop over board ranks
    for( int rank = 0; rank < 8; rank++){
        //loop over board files
        for(int file = 0; file < 8; file++){
            //convert file & rank into sqaure index
            int square = rank * 8 + file;

            //print ranks
            if(!file){
                printf("  %d ", 8 - rank);
            }

            //print bit state euther 1 or 0
            printf(" %d ", get_bit(bitboard, square) ? 1 : 0);
        }
        //print new line every rank
        printf("\n");
    }
    //print board files
    printf("\n     a  b  c  d  e  f  g  h\n\n");

    //print bitboard as unsigned decimal number
    printf("     Bitboard: %llud\n\n", bitboard);
 }


 /*********************************\ 
 
            Attacks 

 ==================================
 \*********************************/   

 /*
        not A File 

  8  0  1  1  1  1  1  1  1 
  7  0  1  1  1  1  1  1  1 
  6  0  1  1  1  1  1  1  1 
  5  0  1  1  1  1  1  1  1 
  4  0  1  1  1  1  1  1  1 
  3  0  1  1  1  1  1  1  1 
  2  0  1  1  1  1  1  1  1 
  1  0  1  1  1  1  1  1  1 

     a  b  c  d  e  f  g  h

     not AB File

  8  0  0  1  1  1  1  1  1 
  7  0  0  1  1  1  1  1  1 
  6  0  0  1  1  1  1  1  1 
  5  0  0  1  1  1  1  1  1 
  4  0  0  1  1  1  1  1  1 
  3  0  0  1  1  1  1  1  1 
  2  0  0  1  1  1  1  1  1 
  1  0  0  1  1  1  1  1  1 

     a  b  c  d  e  f  g  h

     not H File

  8  1  1  1  1  1  1  1  0 
  7  1  1  1  1  1  1  1  0 
  6  1  1  1  1  1  1  1  0 
  5  1  1  1  1  1  1  1  0 
  4  1  1  1  1  1  1  1  0 
  3  1  1  1  1  1  1  1  0 
  2  1  1  1  1  1  1  1  0 
  1  1  1  1  1  1  1  1  0 

     a  b  c  d  e  f  g  h

     not HG File 

  8  1  1  1  1  1  1  0  0 
  7  1  1  1  1  1  1  0  0 
  6  1  1  1  1  1  1  0  0 
  5  1  1  1  1  1  1  0  0 
  4  1  1  1  1  1  1  0  0 
  3  1  1  1  1  1  1  0  0 
  2  1  1  1  1  1  1  0  0 
  1  1  1  1  1  1  1  0  0 

     a  b  c  d  e  f  g  h

 */

//not A file constant
const U64 not_a_file = 18374403900871474942ULL;

//not H file constant
const U64 not_h_file = 9187201950435737471ULL;

//not HG file constant
const U64 not_hg_file= 4557430888798830399ULL;

//not AB file constant
U64 not_ab_file = 18229723555195321596ULL;

// pawn attacks table [side][square]
U64 pawn_attacks[2][64];

//knights attacks table [square]
U64 knight_attacks[64];

//kings attacks table [square]
U64 king_attacks[64];


// generate pawn attacks
U64 mask_pawn_attacks(int side, int square){
    
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // piece bitboard
    U64 bitboard = 0ULL;

    //set piece on board
    set_bit(bitboard, square);

    if(!side){ //white pawns
        attacks |= (bitboard & not_h_file) >> 7;
        attacks |= (bitboard & not_a_file) >> 9;
    }
    else{ //black pawns
        attacks |= (bitboard & not_a_file) << 7;
        attacks |= (bitboard & not_h_file) << 9;
    }

    //return attack map
    return attacks;
}

//generate knight attacks
U64 mask_knight_attacks(int square){
    
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // piece bitboard
    U64 bitboard = 0ULL;

    //set piece on board
    set_bit(bitboard, square);

    //generate attacks
    attacks |= (bitboard & not_h_file) >> 15;
    attacks |= (bitboard & not_a_file) >> 17;
    attacks |= (bitboard & not_hg_file) >> 6;
    attacks |= (bitboard & not_ab_file) >> 10;

    attacks |= (bitboard & not_h_file) << 17;
    attacks |= (bitboard & not_a_file) << 15;
    attacks |= (bitboard & not_hg_file) << 10;
    attacks |= (bitboard & not_ab_file) << 6;

    //return attack map
    return attacks;
}

//generate king attacks
U64 mask_king_attacks(int square){
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // piece bitboard
    U64 bitboard = 0ULL;

    //set piece on board
    set_bit(bitboard, square);

    //generate attacks
    attacks |= (bitboard & not_a_file) >> 1;
    attacks |= (bitboard & not_a_file) >> 9;
    attacks |= (bitboard & not_h_file) >> 7;
    attacks |= (bitboard >> 8);

    attacks |= (bitboard & not_h_file) << 1;
    attacks |= (bitboard & not_h_file) << 9;
    attacks |= (bitboard & not_a_file) << 7;
    attacks |= (bitboard << 8);

    return attacks;
}

//init leaper pieces attacks
void init_leapers_attacks(){
    //loop over 64 boards squares
    for( int square = 0; square < 64; square++){
        //init pawn attacks
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);

        //init kniht attacks
        knight_attacks[square] = mask_knight_attacks(square);

        //init king attacks
        king_attacks[square] = mask_king_attacks(square);
    }
}





/*********************************\ 
 
        Main Driver

 ==================================
 \*********************************/ 

int main(){

    //print_bitboard(mask_king_attacks(a4));

    init_leapers_attacks();

    for (int square=0; square < 64; square++){
        print_bitboard(king_attacks[square]);
    }

    return 0;
}