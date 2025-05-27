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

 //count bits
 #define count_bits(bitboard) __builtin_popcountll(bitboard)
 //get least signigicant 1st bit index
 #define get_lsb_index(bitboard) __builtin_ctzll(bitboard)

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

 const char *square_to_coordinates[] ={
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
 };

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

//biship attack magic map
U64 mask_bishop_attacks(int square){
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks and files 
    int r, f;

    //init target ranks and files;
    int tr = square / 8;
    int tf = square % 8;

    //mask bishiop occupancy bits

    for(r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for(r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for(r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for(r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

//rook attacks magic bit map
U64 mask_rook_attacks(int square){
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    //init ranks and files 
    int r, f;

    //init target ranks and files;
    int tr = square / 8;
    int tf = square % 8;

    //mask rook occupancy bits
    for(r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for(r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for(f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for(f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

// generate bishop attacks on the fly
U64 bishop_attacks_on_the_fly(int square, U64 block){
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks and files 
    int r, f;

    //init target ranks and files;
    int tr = square / 8;
    int tf = square % 8;


    //generate bishop attacks

    //south east attack 
    for(r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++){
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
    }

    //north east attacks
    for(r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++){
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
    }

    //south west attacks
    for(r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--){
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
    }

    //north west attacks
    for(r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--){
        attacks |= (1ULL << (r * 8 + f));
        if((1ULL << (r * 8 + f)) & block) break;
    }

    //return attack maps
    return attacks;
}

// generate rook attacks on the fly
U64 rook_attacks_on_the_fly(int square, U64 block){
    //result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks and files 
    int r, f;

    //init target ranks and files;
    int tr = square / 8;
    int tf = square % 8;


    //generate rook attacks

    //south attack 
    for(r = tr + 1; r <= 7; r++){
        attacks |= (1ULL << (r * 8 + tf));
        if((1ULL << (r * 8 + tf)) & block) break;
    }

    //north attacks
    for(r = tr - 1; r >= 0; r--){
        attacks |= (1ULL << (r * 8 + tf));
        if((1ULL << (r * 8 + tf)) & block) break;
    }

    //east attacks
    for(f = tf + 1; f <= 6; f++){
        attacks |= (1ULL << (tr * 8 + f));
        if((1ULL << (tr * 8 + f)) & block) break;
    }

    //west attacks
    for(f = tf - 1; f >= 0; f--){
        attacks |= (1ULL << (tr * 8 + f));
        if((1ULL << (tr * 8 + f)) & block) break;
    }

    //return attack maps
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

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){
    //occupancy map
    U64 occupancy = 0ULL;

    //loop over the range of bits within attack mask
    for(int count = 0; count < bits_in_mask; count++){
        // get LS1B index of attack mask
        int square = get_lsb_index(attack_mask);

        //pop_bit in attack map
        pop_bit(attack_mask, square);

        //make sure occupancy is on board
        if(index & (1 << count))
            //pop occupancy map
            occupancy |= (1ULL << square);

    }
    //return occ upancy map
    return occupancy;
}



/*********************************\ 
 
        Main Driver

 ==================================
 \*********************************/ 

int main(){

    //initialize leaper pieces attacks
    init_leapers_attacks();

    //mask piece attacks at given square
    U64 attack_mask = mask_bishop_attacks(d4);

    //loop over occupancy indicies
    for(int index = 0; index < 100; index++){

        //init occupancy bitboard
        print_bitboard(set_occupancy(index, count_bits(attack_mask), attack_mask));
        getchar();
    }
    


    return 0;
}