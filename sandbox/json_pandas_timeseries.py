#!/usr/bin/env python3
"""
Using JSON data to pandas DataFrame to plot twin time series.
For Tutorial Problem 2.9.(c)
"""
import subprocess
import simplejson as json
import pandas as pd
import matplotlib.pyplot as plt
from numpy import ceil
import re

def rup_even(f):
    """ Round up to next even number."""
    return ceil(f / 2.) * 2
"""
Data generation using our C++ engine.
  The reason we do this is to control the output filename, so 
  that this script is fairly self-contained. The only thing external 
  we rely upon here is a working 
  'goodwin_prob2_9a' executable.
"""
ofile="goodwin_prob2_9a.json"
goodwin_exec='./goodwin_prob2_9a'
a,b,r,c,w0,Y0,nsteps=2,1,1,1,3.5,4.4,200
term_out = subprocess.run( [goodwin_exec,
                            '-r','%s'%(r),
                            '-c','%s'%(c),
                            '-a','%s'%(a),
                            '-b','%s'%(b),
                            '-w','%s'%(w0),
                            '-y','%s'%(Y0),
                            '-n','%s'%(nsteps),
                            '-o',ofile],
                            stdout=subprocess.PIPE )
#term_out.stdout.decode('utf-8')
#print("term_out = ",term_out)
#print("term_out.stdout = ",term_out.stdout)
#print( "term_out.returncode = ",term_out.returncode)
if term_out.returncode==0:
    print("{0} executed ok.\nOutput in file: {1}"\
        .format(goodwin_exec,ofile))
else:
    print("Alert! Problem executing program {0}\n" \
        "{0} returned non-zero exit status.\n" \
        "Have to quit, sorry.".format(goodwin_exec))
    quit()
"""
Load the data back in...
"""
goodwin_data={}
f=open(ofile,'r')
goodwin_data = json.load(f)
f.close()

params_dict = goodwin_data["params"]
data_dict = goodwin_data["data"]
wages_df = pd.DataFrame(data_dict["wages"],
                        index=data_dict["times"],
                        columns=["wages"])
outputs_df = pd.DataFrame(data_dict["outputs"],
                          index=data_dict["times"],
                          columns=["outputs"])

fig = plt.figure(figsize=(10,5))
ax1 = fig.add_subplot(111)
plt.grid(True,axis='both')
ax1.set_title("Goodwin model: wage and output times series")

ax1.plot( wages_df, 'b-')
ax1.set_xlabel('time (years)')
# Make the y-axis label, ticks and tick labels match the line color.
ax1.set_ylabel('Wage share', color='b')
ax1.tick_params('y', colors='b')
ax1.set_ylim(bottom=0,top=rup_even(max(data_dict["wages"])) )
# Second y-axis
ax2 = ax1.twinx()
ax2.plot( outputs_df, 'tab:red' )
ax2.set_ylabel('Output', color='tab:red')
ax2.tick_params('y', colors='tab:red')
ax2.set_ylim(bottom=0,top=rup_even(max(data_dict["outputs"])) )
#fig.tight_layout()
image = re.sub(r'.json$','.png',ofile)
plt.savefig(image, dpi=600 )
plt.show()

