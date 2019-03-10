/*
 g++ -Wall -I/usr/include/ -c dir_exists.cpp &&
 g++ dir_exists.o  -lstdc++fs -o dir_exists

Example:
 $ ls -R
.:
sim_data

./sim_data:
test.json

 $ ./dir_exists ./sim_data/test.json
 pathname = ./sim_data/test.json 
 dname = "./sim_data"
 thedir = ./sim_data
 directory './sim_data' is present.

$ ./dir_exists foobar
dname was empty, reassigning to "foobar"
 pathname = foobar 
 dname = "foobar"
 thedir = foobar
 path 'foobar' does not exist.
 we will now create this directory for you.

*/
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using namespace std;

int main ( int argc, const char *argv[] )
{
    struct stat info;
    const char *pathname = argv[1];
    
    fs::path emptypath = "";
    fs::path  fpath = pathname;
    fs::path dname = fpath.parent_path();
    if ( dname.compare(emptypath)==0 ) {
        dname = fpath ;
        cout << "dname was empty, reassigning to " << dname << endl;
    }
    cout << " pathname = " << pathname << "\n dname = " << dname << endl;
    const char * thedir = dname.c_str();
    cout << " thedir = " << thedir << endl;
    
    if( stat( thedir, &info ) == 0 ) {
        if (  ( (info.st_mode) & S_IFDIR ) == S_IFDIR )  {
            printf(" directory '%s' is present.\n",thedir);
        } else {
            printf(" '%s' exists but is not a directory.\n",thedir);
            cout<< "Nothing to do."<< endl;
        }
    } else {
        printf(" path '%s' does not exist, so\n",thedir);
        printf(" we will now create this directory for you.\n");
        int status = mkdir(thedir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if ( status == -1 ) printf("Could not mkdir for %s\n",thedir);
    }
    return 0;
}
