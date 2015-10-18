void kernel_main()
{
    char *vidmem = (char*) 0xb8000;
    vidmem[0] = 'H';
    vidmem[2] = 'e';
    vidmem[4] = 'l';
    vidmem[6] = 'l';
    vidmem[8] = 'o';
    vidmem[10] = ' ';
    vidmem[12] = 'W';
    vidmem[14] = 'o';
    vidmem[16] = 'r';
    vidmem[18] = 'l';
    vidmem[20] = 'd';

    while (1) {
    }
}
