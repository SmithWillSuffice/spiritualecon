/*
 Example GNU-GSL ODE solver for Goodwin wage--output model
 
 g++ -Wall -I/usr/include/jsoncpp/ -c goodwin_prob2_9a.cpp &&
 g++ -L/usr/local/lib goodwin_prob2_9a.o -lgsl -lgslcblas -lpopt -ljsoncpp -o goodwin_prob2_9a
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <string>
#include <cstdarg> // suplies va_end()
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#include <popt.h>
#include <string.h>
#include <memory>
#include "json/json.h"

using namespace std;

#define PROGRAM_NAME "goodwin"
#define VERSION 1
#define NMAX 1000000000

struct goodwinParams {
    double r;
    double c;
    double a;
    double b;
    double w0;
    double Y0;
    int Nsteps;
};

std::string strformat( const string fmt_str,...)
{
    va_list ap;
    char *fp = NULL;
    va_start(ap, fmt_str);
    vasprintf(&fp, fmt_str.c_str(), ap);
    va_end(ap);
    std::unique_ptr<char[]> formatted(fp);
    return std::string(formatted.get());
}

static void parseArguments( int argc, const char **argv, 
                            goodwinParams* gparams , char ** outfile )
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
        { "output", 'o', POPT_ARG_STRING, outfile, 0,
            "Pandas format CSV file output pathname (max 180 chars)." },
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
    params.Nsteps = 100;
    string outfile = "goodwin_prob2_9a.json";
    char * pathname;
    pathname = (char*)malloc(sizeof(char)*180);
    //printf("Before parseArgs , pathname = '%s'\n",pathname);
    parseArguments( argc, argv, &params, &pathname );
    //printf("After parseArgs , pathname = '%s'\n",pathname);
    //cout << " strcmp(pathname,'') = " << strcmp(pathname,"") <<endl;
    if ( strcmp(pathname,"")!=0 )  outfile= strformat("%s",pathname);
    if ( params.Nsteps>NMAX ) {
        cout << "Nsteps exceeded maximum.  Resetting Nsteps to  NMAX ="<<NMAX<<endl;
        params.Nsteps = NMAX;
    } else if (params.Nsteps<1 ) {
        cout << "Nsteps = "<<params.Nsteps << " less than minimum."
        << "\nResetting Nsteps to default = 100" << endl;
        params.Nsteps = 100;
    }
    
    gsl_odeiv2_system sys = {func, jac, 2, &params };

    gsl_odeiv2_driver * d =
    gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk8pd,
                                    1e-6, 1e-6, 0.0);
    int i;
    double t = 0.0, t1 = 100.0;
    double y[2] = {  params.w0,  params.Y0 }; // initial conditions: { wages, output }
    
    Json::Value AllData(Json::arrayValue);
    Json::Value param_obj;
    param_obj["r"] = params.r;
    param_obj["c"] = params.c;
    param_obj["a"] = params.a;
    param_obj["b"] = params.b;
    param_obj["w0"] = params.w0;
    param_obj["Y0"] = params.Y0;
    param_obj["Nsteps"] = params.Nsteps;
    Json::Value t_obj;
    Json::Value w_obj;
    Json::Value Y_obj;
    Json::Value times(Json::arrayValue);
    Json::Value wages(Json::arrayValue);
    Json::Value outputs(Json::arrayValue);
   
    for (i = 1; i <= params.Nsteps; i++)
    {
        double ti =  i * t1 / 1000.0;
        int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

        if (status != GSL_SUCCESS)
        {
            printf ("error, return value = %d\n", status);
            break;
        }
        times.append(Json::Value( t ));
        wages.append(Json::Value( y[0] ));
        outputs.append(Json::Value( y[1] ));
    }
    
    t_obj["time (s)"] = times;
    w_obj["wage"] = wages;
    Y_obj["output"] = outputs;
    AllData.append(param_obj);
    AllData.append(t_obj);
    AllData.append(w_obj);
    AllData.append(Y_obj);
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream( outfile );
    writer -> write( AllData, &outputFileStream );
    
    gsl_odeiv2_driver_free (d);
    return 0;
}
