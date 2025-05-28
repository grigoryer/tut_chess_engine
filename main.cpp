#include <iostream>
#include <stdio.h>
#include <string.h>

using std::cout;
#define U64 unsigned long long


/*********************************\ 
 
         Bit manipulations

 ==================================
 \*********************************/       
 

 // set get pop  macros 
 #define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
 #define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
 #define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

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
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
 };

 //sides to move (colors)
 enum{white, black, both};

 //bishop and rook
 enum{rook, bishop};

 //casteling
 enum{wk = 1, wq = 2, bk = 4, bq = 8};

 //encode peice
 enum{P, N, B, R, Q, K, p, n, b, r, q, k};


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

 //ASCII Pieces
 char ascii_pieces[13] = "PNBRQKpnbrqk";

 //character to encode
 int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
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
 
            Board! 

 ==================================
 \*********************************/   

//define piece bitboards
U64 bitboards[12];

 //define occuapny bitboard
U64 occupancy[3];

//side to move
int side;

//en passant square
int enpassant = no_sq;

//castling rights
int castle;


//print board function

void print_board(){
    // loop over ranks and files
    printf("\n");

    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;

            if(!file) 
                printf(" %d", 8 - rank);    

            int piece = -1;

            //loop over all piece bitboards

            for(int bb_piece = P; bb_piece <= k; bb_piece++){
                if(get_bit(bitboards[bb_piece], square)){
                    piece = bb_piece;
                }
            }

            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }
    printf("\n   a b c d e f g h\n\n");
    //side to mvoe
    printf("Side: %s\n", (!side ? "White" : "Black"));
    //print enpassant
    //printf("Enpas: %s\n", (enpassant != no_sq) ? square_to_coordinates[enpassant] : "none");
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

//bishop relevant occupancy bit count for every square on board 
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

//rook relevant occupancy bit count for every square on board 
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// rook magic numbers
U64 rook_magic_numbers[64] = {
    0x2000380201000008ULL,
    0x9002041009000030ULL,
    0x1000430100100082ULL,
    0x808008040002020ULL,
    0x801a0105000424ULL,
    0x12000018200000a0ULL,
    0x2000020800220040ULL,
    0x40801910d4004ULL,
    0x1020210000040180ULL,
    0x1915500080810013ULL,
    0x2800000882600ULL,
    0x28140010110aULL,
    0xc0018c1000262001ULL,
    0xa001000200060ULL,
    0xc00000e00a000500ULL,
    0x4050008210040800ULL,
    0x25c080804200101ULL,
    0x14a0000c00001004ULL,
    0x202008011001182ULL,
    0x88d288c004000ULL,
    0xa520042000120000ULL,
    0x80280304a109080ULL,
    0x841801400120030ULL,
    0x800080000100001ULL,
    0x3200a40580200001ULL,
    0x400020500c00200ULL,
    0x2020020410000010ULL,
    0x1000082400000400ULL,
    0x800402808004000ULL,
    0x40000300a102020ULL,
    0x80042002904000ULL,
    0x181080480000041ULL,
    0x540004900470302ULL,
    0x2019004000c382ULL,
    0x1040010001880800ULL,
    0x9000200289008000ULL,
    0x131042080000150ULL,
    0x220800040c04800ULL,
    0x8c02100001200ULL,
    0x2022188000040120ULL,
    0x20020891201800ULL,
    0x608022088440411ULL,
    0x8061840001108005ULL,
    0x870001a040ULL,
    0x7000000009018ULL,
    0x1001064a2250000ULL,
    0x2000820300180020ULL,
    0x2040c09000060020ULL,
    0x2008000004069210ULL,
    0x8b08001220009ULL,
    0x400032040004ULL,
    0x20006200040280c0ULL,
    0x20004403000010ULL,
    0x8000c001020c0001ULL,
    0x12040810902240ULL,
    0x400c802211420200ULL,
    0x1a10004004004ULL,
    0xa09540042500000ULL,
    0x540008008069ULL,
    0x1080c804200ULL,
    0xa000081241180380ULL,
    0x800880000402445ULL,
    0x8040035200202040ULL,
    0x400080002440020ULL
};

//bishop magic numbers
U64 bishop_magic_numbers[64] = {
    0x8000900140840092ULL,
    0x804840004c900ULL,
    0x4018a4000100200ULL,
    0x2008028800020000ULL,
    0x4003108c18304c34ULL,
    0x2c08401405280000ULL,
    0x100780600010ULL,
    0xc408004a001040ULL,
    0x800420008018010ULL,
    0x1800012898000018ULL,
    0x62040981c0001032ULL,
    0x102120030402100ULL,
    0x10182840048180ULL,
    0x50405800000840ULL,
    0x4300085008ULL,
    0x11008402150a0200ULL,
    0x840011004044280ULL,
    0x704840810202ULL,
    0x8000020e080001ULL,
    0x8808008200808401ULL,
    0x44c20212800000ULL,
    0x18000d090041800ULL,
    0x10e020504211200ULL,
    0x200021018440400ULL,
    0xaa0001210064048ULL,
    0x241102000008124ULL,
    0x100002c12180204ULL,
    0x880200102070ULL,
    0x40200052008011ULL,
    0x430000d0ULL,
    0x8024c0005408060fULL,
    0x400002000c50060ULL,
    0x422008180c1020ULL,
    0x2308102a00414400ULL,
    0x2000082020004206ULL,
    0x8004000c2024090ULL,
    0x6006501801ULL,
    0x88880004685010ULL,
    0x10000802040003c0ULL,
    0x1010430800024000ULL,
    0x410041240190800ULL,
    0x1804000b00810100ULL,
    0x800002808000284ULL,
    0x648041000ULL,
    0x80010802081044ULL,
    0x8080024009001042ULL,
    0x3000dc020500020ULL,
    0x80042000e00004ULL,
    0x220802468032200ULL,
    0x10858020080a0ULL,
    0x2080604201818000ULL,
    0x8002100214000ULL,
    0x8040000821802000ULL,
    0xa0001200000c020ULL,
    0x100090004e800ULL,
    0x28890000282040ULL,
    0x40502920100000ULL,
    0x8020293001200840ULL,
    0x2605400040003000ULL,
    0x2200002002800446ULL,
    0x200806200a008ULL,
    0x801400600042100ULL,
    0x2900290080208400ULL,
    0x5000020058ULL
};

// pawn attacks table [side][square]
U64 pawn_attacks[2][64];

//knights attacks table [square]
U64 knight_attacks[64];

//kings attacks table [square]
U64 king_attacks[64];

//bishop attack masks
U64 bishop_masks[64];

//rook attack masks
U64 rook_masks[64];

//bishop attacks table [squares][occupanices]
U64 bishop_attacks[64][512];

/// rook attacks table [square] [occupancies]
U64 rook_attacks[64][4096];

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
 
         Magics

 ==================================
 \*********************************/ 

 //psuedo random number state
unsigned int random_state = 1804289383;

// gen 32-bit psudo legal numbers
unsigned int get_random_U32number(){
    // get current state
    unsigned int number = random_state;

    //XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    //upadte rnadom number state
    random_state = number;
    
    //return random number

    return number;
}

//generate 64 bit pseudo legal numbers function

U64 get_random_U64number(){
    //define 4 random numbers

    U64 n1, n2, n3, n4;

    n1 = (U64)(get_random_U32number()) & 0xFFFF;
    n2 = (U64)(get_random_U32number()) & 0xFFFF;
    n3 = (U64)(get_random_U32number()) & 0xFFFF;
    n4 = (U64)(get_random_U32number()) & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

//generate magic number candidate
U64 generate_magic_number(){
    return get_random_U64number() & get_random_U64number() & get_random_U64number();
}

 //find apporpriate magic number

 U64 find_magic_number(int square, int relevant_bits, int bishop){
    // init occupancy
    U64 occupancies[4096];

    //init attack tables
    U64 attacks[4096];

    //init used attacks
    U64 used_attacks[4096];

    //init attack mask for a current piece
    U64 attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    int occupancy_indicies = 1 << relevant_bits;

    //loop over occupancy indicies
    for(int index = 0; index < occupancy_indicies; index++){
        //init occupancies
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);

        //innit attacks 
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) 
                                : rook_attacks_on_the_fly(square, occupancies[index]);
    }

    //test magic numbers loop

    for(int random_count = 0; random_count < 1000000000; random_count++){
        //gen magic number candidate
        U64 magic_number = generate_magic_number();

        // skip inappropriate candidates
        if(count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        //init used attacks array
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        //init inde & fail flag
        int index, fail;

        //test magic index loop
        for(index = 0, fail = 0; !fail && index < occupancy_indicies; index++){
            //init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            //if magic index works
            if(used_attacks[magic_index] == 0ULL){
                //init used attacks
                used_attacks[magic_index] = attacks[magic_index];
            }

            //otherwise
            else if(used_attacks[magic_index] != attacks[magic_index]){
                fail = 1;
            }
        }
        //if magic number works return it
        if(!fail)
            return magic_number;
    }
    //if magic number doesnt work
    printf(" Magic number fails");
    return 0ULL;
 }

// init magic numbers
void init_magic_numbers()
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        // init rook magic numbers
        rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], rook);

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        // init bishop magic numbers
        bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], bishop);
}


// init slider piece's attack tables
void init_sliders_attacks(int bishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);
        
        // init current mask
        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];
        
        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);
        
        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);
        
        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                
                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
                
                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }
            
            // rook
            else
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                
                // init magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
                
                // init bishop attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            
            }
        }
    }
}

// get bishop attacks
static inline U64 get_bishop_attacks(int square, U64 occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get rook attacks
static inline U64 get_rook_attacks(int square, U64 occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];

    // return rook attacks
    return rook_attacks[square][occupancy];
}
/*********************************\ 
 
        init all

 ==================================
 \*********************************/

 //init all variables

 void init_all(){

    //init magic nubmers
    //init_magic_numbers();

    init_leapers_attacks();

    // init slider pieces attacks
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
 }


/*********************************\ 
 
        Main Driver

 ==================================
 \*********************************/ 

int main(){
    //init all
    init_all();


    set_bit(bitboards[P], a2);
    set_bit(bitboards[P], b2);
    set_bit(bitboards[P], c2);
    set_bit(bitboards[P], d2);
    set_bit(bitboards[P], e2);
    set_bit(bitboards[P], f2);
    set_bit(bitboards[P], g2);
    set_bit(bitboards[P], h2);


    set_bit(bitboards[N], b1);
    set_bit(bitboards[N], g1);

    set_bit(bitboards[B], c1);
    set_bit(bitboards[B], f1);

    set_bit(bitboards[R], a1);
    set_bit(bitboards[R], h1);

    set_bit(bitboards[K], e1);
    set_bit(bitboards[Q], d1);



//set black
    set_bit(bitboards[p], a7);
    set_bit(bitboards[p], b7);
    set_bit(bitboards[p], c7);
    set_bit(bitboards[p], d7);
    set_bit(bitboards[p], e7);
    set_bit(bitboards[p], f7);
    set_bit(bitboards[p], g7);
    set_bit(bitboards[p], h7);


    set_bit(bitboards[n], b8);
    set_bit(bitboards[n], g8);

    set_bit(bitboards[b], c8);
    set_bit(bitboards[b], f8);

    set_bit(bitboards[r], a8);
    set_bit(bitboards[r], h8);

    set_bit(bitboards[k], e8);
    set_bit(bitboards[q], d8);
    //print_bitboard(bitboards[P]);

    print_board();


    for(int piece = P; piece <= k; piece++){
        print_bitboard(bitboards[piece]);
    }
    return 0;

}