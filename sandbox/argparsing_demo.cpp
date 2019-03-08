/*
 Argument parsing example.
 
 g++ -Wall -I/usr/include/ -c argparsin-demo.cpp && g++ -L/usr/local/lib argparsing_demo.o  -o argparsing_demo
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>>

using namespace std;

struct goodwin_params {
    double r;
    double c;
    double a;
    double b;
    double w0;
    double Y0;
};

void getopts( int argc, char **argv,  goodwin_params* gparams ) {
    int index;
    int opt;

    opterr = 0;

    while (( opt = getopt (argc, argv, "r:c:a:b:w:y:Y:")) != -1)
    switch ( opt ) {
        case 'r':
        gparams->r = atof(optarg);
        break;
        case 'c':
            gparams->c = atof(optarg);
            break;
        case 'a':
            gparams->a = atof(optarg);
            break;
        case 'b':
            gparams->b = atof(optarg);
            break;
        case 'w':
            gparams->w0 = atof(optarg);
            break;
        case 'y':
            gparams->Y0 = atof(optarg);
            break;
        case 'Y':
            gparams->Y0 = atof(optarg);
            break;
        case '?':
        if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
            fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
        return;
        default:
        abort ();
    }

    printf ("r = %f, c = %f, a = %f, b = %f, w0 = %f, Y0 = %f\n",
            gparams->r, gparams->c, gparams->a, gparams->b, gparams->w0, gparams->Y0);

    for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
    
}


int  main (int argc, char **argv)
{
    // defaults
    goodwin_params gparams;
    gparams.r = 1.0;
    gparams.c = 1.0;
    gparams.a = 1.0;
    gparams.b = 1.0;
    gparams.w0 = 1.0;
    gparams.Y0 = 1.0;
    getopts( argc, argv, &gparams );
    return 0;
}
