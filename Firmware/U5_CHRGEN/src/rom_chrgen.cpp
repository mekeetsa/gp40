#include <cstdio>
#include <cstring>

char* LS38( int v )
{
    static char G[ 9 ] = { 0 };
    G[0] = ( v >> 0 ) & 1 ? '.' : '#';
    G[1] = ( v >> 1 ) & 1 ? '.' : '#';
    G[2] = ( v >> 2 ) & 1 ? '.' : '#';
    G[3] = ( v >> 3 ) & 1 ? '.' : '#';
    G[4] = ( v >> 4 ) & 1 ? '.' : '#';
    G[5] = ( v >> 5 ) & 1 ? '.' : '#';
    G[6] = ( v >> 6 ) & 1 ? '.' : '#';
    G[7] = ( v >> 7 ) & 1 ? '.' : '*';
    return G;
}

int main( int argc, char* argv[] )
{
        enum { rom_size = 65536 };
    unsigned char rom[ rom_size ];
    memset( rom, 0xFF, rom_size );
    FILE* f = fopen( argc >= 2 ? argv[1] : "GALAXY_CHRGENP.BIN", "rb" );
    fread( &rom, 1, rom_size, f );
    fclose( f );

    /* ROMG = 0, lower 4k, characters  */

    for( int chr = 0; chr < 128; ++chr )
    {
        printf( "Character %02x (%c)\n", chr, chr );
        for( int line = 0; line < 16; ++ line )
        {
            int i = (line << 7) | chr;
            int A00 = ( i >> 0 ) & 1;
            int A01 = ( i >> 1 ) & 1;
            int A02 = ( i >> 2 ) & 1;
            int A03 = ( i >> 3 ) & 1;
            int A04 = ( i >> 4 ) & 1;
            int A05 = ( i >> 5 ) & 1;
            int A07 = ( i >> 6 ) & 1;
            int A08 = ( i >> 7 ) & 1;
            int A09 = ( i >> 8 ) & 1;
            int A10 = ( i >> 9 ) & 1;
            int A11 = ( i >> 10 ) & 1;
            int A06 = ( i >> 11 ) & 1;
            int ROMG = ( i >> 12 ) & 1;
            int addr = (ROMG << 12)
                | (A11 << 11) | (A10 << 10) | (A09 << 9) | (A08 << 8)
                | (A07 <<  7) | (A06 <<  6) | (A05 << 5) | (A04 << 4)
                | (A03 <<  3) | (A02 <<  2) | (A01 << 1) | (A00 << 0);
            printf( "%04x %02x %s\n", i, rom[i], LS38(rom[i]) );
        }
        printf( "\n" );
    }

    /* ROMG = 1, upper 4k, graphics mode  */

    for( int i = 4096; i < 8192; ++i )
    {
        if ( i % 1024 == 0 ) { printf( "Block %dk\n\n", i/1024 ); }
        int A00 = ( i >> 0 ) & 1;
        int A01 = ( i >> 1 ) & 1;
        int A02 = ( i >> 2 ) & 1;
        int A03 = ( i >> 3 ) & 1;
        int A04 = ( i >> 4 ) & 1;
        int A05 = ( i >> 5 ) & 1;
        int A07 = ( i >> 6 ) & 1;
        int A08 = ( i >> 7 ) & 1;
        int A09 = ( i >> 8 ) & 1;
        int A10 = ( i >> 9 ) & 1;
        int A11 = ( i >> 10 ) & 1;
        int A06 = ( i >> 11 ) & 1;
        int ROMG = ( i >> 12 ) & 1;
        int addr = (ROMG << 12)
            | (A11 << 11) | (A10 << 10) | (A09 << 9) | (A08 << 8)
            | (A07 <<  7) | (A06 <<  6) | (A05 << 5) | (A04 << 4)
            | (A03 <<  3) | (A02 <<  2) | (A01 << 1) | (A00 << 0);
        printf( "%04x %02x %s ", i, rom[i], LS38(rom[i]) );
        printf( "| %02x %s", rom[addr], LS38(rom[addr]) );
        printf( "\n" );
        if ( ( i % 8 ) == 7 ) { printf( "\n" ); }
    }
}
