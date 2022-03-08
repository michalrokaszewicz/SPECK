
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <locale.h>

#include <string.h>

void start(uint32_t[], uint32_t[], uint32_t);

int ROL(uint32_t, uint32_t);
int ROR(uint32_t, uint32_t);

int R(uint32_t*, uint32_t*, uint32_t);
int RR(uint32_t*, uint32_t*, uint32_t);

void encipher(uint32_t[], uint32_t[], uint64_t);
void decipher(uint32_t[], uint32_t[], uint64_t);

int main()
{
    //word = 32b // block = 64b // key = 4 x 32b = 128b
    uint64_t block, tmp;
    uint32_t blockSize = 8, word, kSize =  128, key[4],
    i = 0, counter = 0, v[2], numRounds = 27, keySchedule[numRounds];
    uint8_t date = 1;
    char bufor;

    key[0] = 0x01234567;
    key[1] = 0x01234567;
    key[2] = 0x01234567;
    key[3] = 0x01234567;

    start(key, keySchedule, numRounds);

    FILE *out, *in;
    in = fopen("input.txt", "r");
    out = fopen("output.txt", "w");

    while(date)
    {
        int j = 0;
        block = 0;

        do
        {
            block <<= 8;
            bufor = fgetc(in);
            if((int)bufor < 0 )
                {
                    bufor = 0;
                    date = 0;
                }
            block += (int)bufor;

            j++;
        }while(j < blockSize);

        tmp = block;
        tmp >>= 32;
        v[0] =  tmp;
        tmp = (block & 0x00000000FFFFFFFF);
        v[1] = tmp;
        tmp = block;

        encipher(v, keySchedule, numRounds);
        decipher(v, keySchedule, numRounds);

        tmp = v[0];
        tmp <<= 32;
        tmp += v[1];

        for(int o = 0; o < 8; o++)
        {
            uint64_t mask= 0xFF00000000000000, t = 0;

            t = tmp & mask;
            t >>= 56;
            bufor = (char)t;
            fputc(bufor, out);

            tmp <<= 8;
        }
    }

    fclose(in);
    fclose(out);

    printf("End of encryption");

    return 0;
}

void start(uint32_t key[], uint32_t keySchedule[], uint32_t rounds)
{
    keySchedule[0] = key[0];

    for(uint32_t i = 0; i < rounds - 1; i++)
    {
        R(&keySchedule[(i % 3) + 1], &keySchedule[0], &i);
        keySchedule[i + 1] = keySchedule[0];
    }
}

int ROR(uint32_t x, uint32_t r)
{
    return ((x >> r) | (x << (32 - r)));
}

int ROL(uint32_t x, uint32_t r)
{
    return ((x << r) | (x >> (32 - r)));
}

int R(uint32_t *x, uint32_t *y, uint32_t k)
{
    *x = ROR(*x, 8);
    *x += *y;
    *x ^= k;
    *y = ROL(*y, 3);
    *y ^= *x;
}

int RR(uint32_t *x, uint32_t *y, uint32_t k)
{
    *y ^= *x;
    *y = ROR(*y, 3);
    *x ^= k;
    *x -= *y;
    *x = ROL(*x, 8);
}

void encipher(uint32_t v[], uint32_t key[], uint64_t rounds)
{
    uint32_t x = v[0], y = v[1], a = key[0], b = key[1];

    for (uint64_t i = 0; i < rounds; i++)
    {
        R(&x, &y, &key[i]);
    }
    v[0] = x;
    v[1] = y;
}

void decipher(uint32_t v[], uint32_t key[], uint64_t rounds)
{
    uint32_t x = v[0], y = v[1], a = key[0], b = key[1];

    for (uint64_t i = rounds; i > 0; i--)
    {
        RR(&x, &y, &key[i-1]);
    }
    v[0] = x;
    v[1] = y;
}
