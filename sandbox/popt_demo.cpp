/*
 Argument parsing getopts_long demo.
 
 g++ -Wall  -c popt_demo.cpp && g++ popt_demo.o  -lpopt -o popt_demo
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <popt.h>
#include <string.h>

using namespace std;

#define PROGRAM_NAME "popt_demo"

struct goodwinParams {
    double r;
    double c;
    double a;
    double b;
    double w0;
    double Y0;
    int Nsteps;
};

static void parseArguments( int argc, const char **argv, goodwinParams* gparams )
{
    poptContext optCon;
    const struct poptOption optionsTable[] = {
        POPT_AUTOHELP
        { "nsteps  ", 'n', POPT_ARG_INT, &gparams->Nsteps, 0,
            "Set number of time steps." },
        { "r  ", 'r', POPT_ARG_DOUBLE, &gparams->r, 0,
            "Set wage appreciation parameter." },
        { "c  ", 'c', POPT_ARG_DOUBLE, &gparams->c, 0,
            "Set wage growth decay rate parameter." },
        { "a  ", 'a', POPT_ARG_DOUBLE, &gparams->a, 0,
            "Set output growth rate parameter." },
        { "b  ", 'b', POPT_ARG_DOUBLE, &gparams->b, 0,
            "Set output depreciation parameter." },
        { "w0 ", 'w', POPT_ARG_DOUBLE, &gparams->w0, 0,
            "Set initial wage share." },
        { "Y0 ", 'y', POPT_ARG_DOUBLE, &gparams->Y0, 0,
            "Set initial output level." },
        { "Y0 ", 'Y', POPT_ARG_DOUBLE, &gparams->Y0, 0,
            "Set initial output level." },
        {NULL, 0, 0, NULL, 0, }
    };
    int i;
    int err;
    const char *arg = NULL;
    int argcnt = 0;
    
    optCon = poptGetContext(PROGRAM_NAME, argc, argv, optionsTable, 0);
    poptReadDefaultConfig(optCon, 0);
    
    /*
     You'd need a two pass loop here to pretty print a longer program 
     description to wrap it in 80 char width. 
    */
    for( i=0; i < argc; i++) {
        //if ( strncmp( *(argv+i), "--help", 6)==0 || strncmp( *(argv+i), "-h", 2)==0 ) {
        if ( strncmp( *(argv+i), "--help", 6)==0  ) {
            printf("Demo use of opt for argument parsing.\n");
        }
    }
    
    err = poptGetNextOpt(optCon);
    if (err != -1) {
        fprintf(stderr, "\t%s: %s\n",
            poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
            poptStrerror(err));
        exit(-1);
    }
    
    /* Parse arguments that do not begin with '-' (leftovers) */
    arg = poptGetArg(optCon);
    while (arg != NULL) {
        printf("arg %2d   = %s\n", ++argcnt, arg);
        arg = poptGetArg(optCon);
    }
    poptFreeContext(optCon);
}


int main( int argc, const char *argv[] ) {
    
    goodwinParams gparams;
    gparams.r = 1.0;
    gparams.c = 1.0;
    gparams.a = 1.0;
    gparams.b = 1.0;
    gparams.w0 = 1.0;
    gparams.Y0 = 1.0;
    gparams.Nsteps = 100;
    parseArguments( argc, argv, &gparams );
    
    printf ("r = %f, c = %f, a = %f, b = %f, w0 = %f, Y0 = %f\n",
            gparams.r, gparams.c, gparams.a, gparams.b, gparams.w0, gparams.Y0);
    return 0 ;
}
