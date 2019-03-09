/*
 Example GNU-GSL ODE solver for Goodwin wage--output model
 
 g++ -Wall -I/usr/include/ -c goodwin_to_csv.cpp &&
 g++ -L/usr/local/lib goodwin_to_csv.o -lgsl -lgslcblas -lpopt -o goodwin_to_csv

 Version 2.0  has cmdl parsing options, and file output
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#include <popt.h>
#include <string.h>

using namespace std;

#define PROGRAM_NAME "goodwin"
#define VERSION 1

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
        { "nsteps  ", 'n', POPT_ARG_DOUBLE, &gparams->Nsteps, 0,
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


int func (double t, const double y[], double f[],
      void *params)
{
    (void)(t); /* avoid unused parameter warning */ 
    struct goodwinParams *p = (struct goodwinParams*)(params); 
    double r = p->r; 
    double c = p->c; 
    double a = p->a; 
    double b = p->b;
    f[0] = -c*y[0] + r*y[0]*y[1]; // wages y[0], f[0]=dy[0]/dt
    f[1] = a*y[1] - b*y[0]*y[1]; // output y[1], f[1]=dy[1]/dt
    return GSL_SUCCESS;
}

int
jac (double t, const double y[], double *dfdy,
     double dfdt[], void *params)
{
    (void)(t); /* avoid unused parameter warning */
    struct goodwinParams *p = (struct goodwinParams*)(params); 
    double r = p->r; 
    double c = p->c; 
    double a = p->a; 
    double b = p->b;
    gsl_matrix_view dfdy_mat
    = gsl_matrix_view_array (dfdy, 2, 2);
    gsl_matrix * m = &dfdy_mat.matrix;
    gsl_matrix_set (m, 0, 0, -c+r*y[1] );
    gsl_matrix_set (m, 0, 1, -b*y[1] );
    gsl_matrix_set (m, 1, 0, r*y[0] );
    gsl_matrix_set (m, 1, 1, a-b*y[0] );
    dfdt[0] = 0.0;  // no explicit time dependencies
    dfdt[1] = 0.0;
    return GSL_SUCCESS;
}

int main ( int argc, const char *argv[] )
{
    goodwinParams params;
    params.r = 1.0;
    params.c = 1.0;
    params.a = 1.0;
    params.b = 1.0;
    params.w0 = 3.0;
    params.Y0 = 4.0;
    params.Nsteps = 100.;
    parseArguments( argc, argv, &params );
    
    gsl_odeiv2_system sys = {func, jac, 2, &params };

    gsl_odeiv2_driver * d =
    gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk8pd,
                                    1e-6, 1e-6, 0.0);
    int i;
    double t = 0.0, t1 = 100.0;
    double y[2] = {  params.w0,  params.Y0 }; // initial conditions: { wages, output }
    
    ofstream pdout;
    char csvfile[80];
    sprintf(csvfile,"%s_v%d_N%d.pd",PROGRAM_NAME,VERSION,params.Nsteps);
    pdout.open(csvfile);
    pdout << "# Goodwin model data output." << endl;
    pdout << "# r="<< params.r << " , c=" << params.r 
       << " , a=" << params.a << " , b=" << params.b 
       << " , w0="<< y[0] << " , Y0=" << y[1] << endl;
    // NB: no whitespace in the column names if we want Pandas dataframe format
    pdout << "time,wages,output" << endl; 
    for (i = 1; i <= params.Nsteps; i++)
    {
        double ti =  i * t1 / 1000.0;
        int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

        if (status != GSL_SUCCESS)
        {
            printf ("error, return value = %d\n", status);
            break;
        }
        pdout << setw(12) << fixed << t << "," << setw(12)
           <<  y[0] << "," << setw(12) << y[1] << endl;
    }
    pdout.close();
    gsl_odeiv2_driver_free (d);
    return 0;
}
