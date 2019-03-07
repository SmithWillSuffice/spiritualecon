/*
 Example GNU-GSL ODE solver for Goodwin wage--output model
 
 g++ -Wall -I/usr/include/ -c goodwin.cpp &&
 g++ -L/usr/local/lib goodwin.o -lgsl -lgslcblas -o goodwin

*/

#include <iostream>
#include <iomanip>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

using namespace std;

struct parameters {
    double r;
    double c;
    double a;
    double b;
};

int func (double t, const double y[], double f[],
      void *params)
{
    (void)(t); /* avoid unused parameter warning */ 
    struct parameters *p = (struct parameters*)(params); 
    double r = p->r; 
    double c = p->c; 
    double a = p->a; 
    double b = p->b;
    /*
    cout << "In jac():\n" << "y[0]=" << y[0]<< "y[0]=" << y[1]<< ", y[1]=" << y[1]
    << ", r=" << r << ", c=" << c << ", a=" << a<< "yb=" << b << endl;
    */
    f[0] = -c*y[0] + r*y[0]*y[1]; // wages y[0], f[0]=dy[0]/dt
    f[1] = a*y[1] - b*y[0]*y[1]; // output y[1], f[1]=dy[1]/dt
    return GSL_SUCCESS;
}

int
jac (double t, const double y[], double *dfdy,
     double dfdt[], void *params)
{
    (void)(t); /* avoid unused parameter warning */
    struct parameters *p = (struct parameters*)(params); 
    double r = p->r; 
    double c = p->c; 
    double a = p->a; 
    double b = p->b;
    /*cout << "In func():\n" << "y[0]=" << y[0]<< "y[0]=" << y[1]<< ", y[1]=" << y[1]
    << ", r=" << r << ", c=" << c << ", a=" << a<< ", b=" << b << endl;
    */
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

int main (void)
{
    parameters p;
    p.r = 1.0;
    p.c = 1.0;
    p.a = 1.0;
    p.b = 1.0;
    
    gsl_odeiv2_system sys = {func, jac, 2, &p };

    gsl_odeiv2_driver * d =
    gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk8pd,
                                    1e-6, 1e-6, 0.0);
    int i;
    double t = 0.0, t1 = 100.0;
    double y[2] = { 2.0, 1.0 }; /// initial conditions: { wages, output }
    /*
    cout << setw(12) << fixed << setw(12) << "time," << setw(12) <<  "wage share," << setw(12) << "output" << endl;*/
        
    for (i = 1; i <= 100; i++)
    {
        double ti =  i * t1 / 1000.0;
        int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

        if (status != GSL_SUCCESS)
        {
            printf ("error, return value = %d\n", status);
            break;
        }
        
        cout << setw(12) << fixed << setw(12) << t << setw(12) <<  y[0] << setw(12) << y[1] << endl;
    }

    gsl_odeiv2_driver_free (d);
    return 0;
}
