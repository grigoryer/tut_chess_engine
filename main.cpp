#include <iostream>
#include <stdio.h>
#include <string.h>

using std::cout;
#define U64 unsigned long long


#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "


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
U64 occupancies[3];

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

    printf("Enpassant:   %s\n", (enpassant != no_sq) ? square_to_coordinates[enpassant] : "no");
    
    // print castling rights
    printf("Castling:  %c%c%c%c\n\n", (castle & wk) ? 'K' : '-',
                                           (castle & wq) ? 'Q' : '-',
                                           (castle & bk) ? 'k' : '-',
                                           (castle & bq) ? 'q' : '-');
}



//parse FEN

void parse_fen(const char *fen){

    //reset board position
    memset(bitboards, 0ULL, sizeof(bitboards));

    //reset occupancies
    memset(occupancies, 0ULL, sizeof(occupancies));

    //reset game state variables
    side = 0;
    enpassant = no_sq;
    castle = 0;

    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square =rank * 8 + file;

            if((*fen >= 'a' && *fen <= 'z') || *fen >='A' && *fen <= 'Z'){
                int piece = char_pieces[*fen];

                //set piece on corresponding bitboard
                set_bit(bitboards[piece], square);

                fen++;
            }
            //match empty squares
            if(*fen >= '0' && *fen <= '9'){

                //convert char to int
                int offset = *fen - '0';

                int piece = -1;

                //loop over all piece bitboards

                for(int bb_piece = P; bb_piece <= k; bb_piece++){
                    if(get_bit(bitboards[bb_piece], square)){
                        piece = bb_piece;
                    }
                }
                //if no piece on current square
                if(piece == -1)
                    file--;

                //adjust file counter
                file += offset;

                fen++;
            }

            if(*fen == '/')
                fen++;
        }
    }
    fen++;

    //side
    (*fen == 'w') ? (side = white) : (side = black);

    //casteling rights
    fen += 2;

    while (*fen != ' '){
        switch(*fen){
            case 'K': castle |= wk; break;
            case 'Q': castle |= wq; break;
            case 'k': castle |= bk; break;
            case 'q': castle |= bq; break;
            case '-': break;
        }
        fen++;
    }

    //parse en passant square
    fen++;
    if(*fen != '-'){
        //parse enpassant file & rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        //init enpassant square

        enpassant = rank * 8 + file;
    }else{
        enpassant = no_sq;
    }


    //init occupancies
    //white
    for(int piece = P; piece <= K; piece++){
        occupancies[white] |= bitboards[piece];
    }

    //black
    for(int piece = p; piece <= k; piece++){
        occupancies[black] |= bitboards[piece];
    }

    //both
    occupancies[both] |= occupancies[white] | occupancies[black];
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
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};

//bishop magic numbers
U64 bishop_magic_numbers[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
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

    // mask relevant bishop occupancy bits
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

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

    // mask relevant rook occupancy bits
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

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


static inline U64 get_queen_attacks(int square, U64 occupancy)
{

    //init result attacks

    U64 queen_attacks = 0ULL;

    U64 bishop_occupancy = occupancy;
    U64 rook_occupancy = occupancy;

    //bisop attacks assuming board occupancy 
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magic_numbers[square];
    bishop_occupancy >>= 64 - bishop_relevant_bits[square];

    queen_attacks = bishop_attacks[square][bishop_occupancy];

    // get rook attacks assuming current board occupancy
    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magic_numbers[square];
    rook_occupancy >>= 64 - rook_relevant_bits[square];

    queen_attacks |= rook_attacks[square][rook_occupancy];

    // return queen attacks
    return queen_attacks;
}


//is square attacked?!

static inline int is_square_attacked(int square, int side){

    //squqres attacked by white pawn
    if((side == white) && (pawn_attacks[black][square] & bitboards[P])) return 1;
    //squqres attacked by black pawn
    if((side == black) && (pawn_attacks[white][square] & bitboards[p])) return 1;

    //squares attacked by knights
    if(knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])) return 1;

    //squares attacked by bishop
    if (get_bishop_attacks(square,occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])) return 1;

    //squares attacked by rook
    if (get_rook_attacks(square,occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])) return 1;

    //squares attacked by queen
    if (get_queen_attacks(square,occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])) return 1;

    //squares attacked by king
    if(king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])) return 1;

    //by deafult fasle
    return 0;

}

// print attacked squares

void print_attacked_squares(int side){
    printf("\n");
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file ++){

            int square = rank * 8 + file;
            if(!file)
                printf("   %d ",8 - rank);

            printf(" %d",is_square_attacked(square, side)? 1 : 0);
        }
        printf("\n");
    }
    printf("\n      a b c d e f g h\n\n");
}



static inline void generate_moves(){
    int source_square; int target_square;

    //init bitboard copy
    U64 bitboard, attacks;

    //loop over all the bitboards

    for(int piece = P; piece <= k; piece++){
        //init piece bitboard copy 
        bitboard = bitboards[piece];

        //gen white pawns and white king casteling moves

        if(side == white){

            if(piece == P){
                //loop over white pawns.
                while(bitboard){
                    // init source square
                    source_square = get_lsb_index(bitboard);
                    //init target square
                    target_square = source_square - 8;

                    if(!(target_square < a8) && !get_bit(occupancies[both], target_square)){
                        //pawn promotion
                        if(source_square >= a7 && source_square <= h7){
                            //add move into move list
                            printf("%s%sq: wpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr: wpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sk: wpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb: wpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        else{
                            //one square ahead one move
                            printf("%s%s: wpawn push\n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            //two squares ahead pawn move
                            if((source_square >= a2 && source_square <= h2) && !get_bit(occupancies[both], target_square - 8))
                                printf("%s%s: wpawn double push\n", square_to_coordinates[source_square], square_to_coordinates[target_square - 8]);
                        }
                    }

                    //pawn capture moves
                    attacks = pawn_attacks[side][source_square] & occupancies[black];

                    while(attacks){
                        //init target attacks
                        target_square = get_lsb_index(attacks);
                        if(source_square >= a7 && source_square <= h7){
                            printf("%s%sq: wpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr: wpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sk: wpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb: wpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        else{
                            printf("%s%s: wpawn capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        pop_bit(attacks, target_square);
                    }
                    if(enpassant != no_sq){

                        //look up pawn attacks and bitiwise AND with enpassant square  (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        //make sure enpassant capture avaibalbe
                        if(enpassant_attacks){
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            printf("%s%s: wpawn enpassant capture \n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
                        }
                    }
                    
                    //pop ls1b from bitboard copy
                    pop_bit(bitboard, source_square);
                }

            }
            //casteling moves

            if(piece == K){
                //king side casteling is avaiable
                if(castle & wk){
                    //no pieces between rook and king
                    if(!get_bit(occupancies[both],f1) && !get_bit(occupancies[both],g1)){
                        if(!is_square_attacked(e1, black) && !is_square_attacked(f1, black )&& !is_square_attacked(g1, black)){
                            printf("e1g1: casteling move\n");
                        }
                    }
                }
                if(castle & wq){
                    if(!get_bit(occupancies[both],c1) && !get_bit(occupancies[both],d1) && !get_bit(occupancies[both],b1)){
                        if(!is_square_attacked(e1, black) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)){
                            printf("e1c1: casteling move\n");
                        }
                    }
                }
                //queen side avaiable
            }
            
        }else{//generate black pawns and black king casteling moves
            if(piece == p){
                //loop over white pawns.
                while(bitboard){
                    // init source square
                    source_square = get_lsb_index(bitboard);
                    //init target square
                    target_square = source_square + 8;

                    if(!(target_square > h1) && !get_bit(occupancies[both], target_square)){
                        //pawn promotion
                        if(source_square >= a2 && source_square <= h2){
                            //add move into move list
                            printf("%s%sq: bpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr: bpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sk: bpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb: bpawn promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        else{
                            //one square ahead one move
                            printf("%s%s: bpawn double push\n", square_to_coordinates[source_square], square_to_coordinates[target_square - 8]);                            //two squares ahead pawn move
                            if((source_square >= a7 && source_square <= h7) && !get_bit(occupancies[both], target_square + 8))
                                printf("%s%s: bpawn double push: \n", square_to_coordinates[source_square], square_to_coordinates[target_square + 8]);
                        }
                    }

                    //pawn capture moves
                    attacks = pawn_attacks[side][source_square] & occupancies[white];

                    while(attacks){
                        //init target attacks
                        target_square = get_lsb_index(attacks);
                        if(source_square >= a2 && source_square <= h2){
                            printf("%s%sq: bpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sr: bpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sk: bpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                            printf("%s%sb: bpawn capture promotion: \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        else{
                            printf("%s%s: pawn capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                        }
                        pop_bit(attacks, target_square);
                    }

                    if(enpassant != no_sq){

                        //look up pawn attacks and bitiwise AND with enpassant square  (bit)
                        U64 enpassant_attacks = pawn_attacks[side][source_square] & (1ULL << enpassant);

                        //make sure enpassant capture avaibalbe
                        if(enpassant_attacks){
                            int target_enpassant = get_lsb_index(enpassant_attacks);
                            printf("%s%s: bpawn enpassant capture \n", square_to_coordinates[source_square], square_to_coordinates[target_enpassant]);
                        }
                    }
                
                    //pop ls1b from bitboard copy
                    pop_bit(bitboard, source_square);
                }

            }

            if(piece == k){
                //king side casteling is avaiable
                if(castle & bk){
                    //no pieces between rook and king
                    if(!get_bit(occupancies[both],f8) && !get_bit(occupancies[both],g8)){
                        if(!is_square_attacked(e8, white) && !is_square_attacked(f8, white )&& !is_square_attacked(g8, white)){
                            printf("e8g8: casteling move: \n");
                        }
                    }
                }
                if(castle & bq){
                    if(!get_bit(occupancies[both],c8) && !get_bit(occupancies[both],d8) && !get_bit(occupancies[both],b8)){
                        if(!is_square_attacked(e8, white) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)){
                            printf("e8c8: casteling move: \n");
                        }
                    }
                }
                //queen side avaiable
            }
        }


        //generate knight moves

        if((side == white) ? piece ==N : piece == n){
            while(bitboard){
                source_square = get_lsb_index(bitboard);

                //init piece attacks
                attacks = knight_attacks[source_square] & ((side == white) ?  ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    target_square = get_lsb_index(attacks);

                    if(!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                        printf("%s%s: piece quite move \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    else
                        printf("%s%s: piece capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    pop_bit(attacks, target_square);
                    //pop lsb in current attack set
                }
                pop_bit(bitboard, source_square);
            }
        }

        //generate bishop moves

        if((side == white) ? piece == B : piece == b){
            while(bitboard){
                source_square = get_lsb_index(bitboard);

                //init piece attacks
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ((side == white) ?  ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    target_square = get_lsb_index(attacks);

                    if(!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                        printf("%s%s: piece quite move \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    else
                        printf("%s%s: piece capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    pop_bit(attacks, target_square);
                    //pop lsb in current attack set
                }
                pop_bit(bitboard, source_square);
            }
        }


        //generate rook moves

        if((side == white) ? piece == R : piece == r){
            while(bitboard){
                source_square = get_lsb_index(bitboard);

                //init piece attacks
                attacks = get_rook_attacks(source_square, occupancies[both]) & ((side == white) ?  ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    target_square = get_lsb_index(attacks);

                    if(!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                        printf("%s%s: piece quite move \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    else
                        printf("%s%s: piece capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    pop_bit(attacks, target_square);
                    //pop lsb in current attack set
                }
                pop_bit(bitboard, source_square);
            }
        }


        //generate queen moves

        if((side == white) ? piece == Q : piece == q){
            while(bitboard){
                source_square = get_lsb_index(bitboard);

                //init piece attacks
                attacks = get_queen_attacks(source_square, occupancies[both]) & ((side == white) ?  ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    target_square = get_lsb_index(attacks);

                    if(!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                        printf("%s%s: piece quite move \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    else
                        printf("%s%s: piece capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    pop_bit(attacks, target_square);
                    //pop lsb in current attack set
                }
                pop_bit(bitboard, source_square);
            }
        }
        //generate king moves

        if((side == white) ? piece == K : piece == k){
            while(bitboard){
                source_square = get_lsb_index(bitboard);

                //init piece attacks
                attacks = king_attacks[source_square] & ((side == white) ?  ~occupancies[white] : ~occupancies[black]);

                while (attacks){
                    target_square = get_lsb_index(attacks);

                    if(!get_bit(((side == white) ? occupancies[black] : occupancies[white]), target_square))
                        printf("%s%s: piece quite move \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    else
                        printf("%s%s: piece capture \n", square_to_coordinates[source_square], square_to_coordinates[target_square]);
                    pop_bit(attacks, target_square);
                    //pop lsb in current attack set
                }
                pop_bit(bitboard, source_square);
            }
        }
    }
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
    //parse_fen("r3k2r/p11pqpb1/bn2pnp1/2pPN3/1p2P3/2N2Q1p/PPr2PPP/R3K2R w KQkq - 0 1 ");
    parse_fen(tricky_position);
    print_board();
    generate_moves();
    return 0;
}