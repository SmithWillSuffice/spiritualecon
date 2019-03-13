#!/usr/bin/env python3
"""
Animated trajectories for Goodwin model.
For Tutorial Problem 2.11.(a)
"""
import simplejson as json
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.collections import LineCollection
from matplotlib.colors import ListedColormap
from matplotlib.widgets import Slider
import matplotlib.animation as animation
import numpy as np
import re

def closest_index(a,val,tol=1e-6,i_default=0):
    i_array = np.where(np.isclose(a, val, tol))
    return i_default if (len(i_array[0])==0) else int(i_array[0])

"""
 Colormap  customized definitions
"""
Nc=256
#----- white to green cmap
wg_vals = np.ones((Nc, 4))
wg_vals[:, 0] = np.linspace(1, 0, Nc)
wg_vals[:, 1] = np.linspace(1, 1, Nc)
wg_vals[:, 2] = np.linspace(1, 0, Nc)
wg_cm = ListedColormap(wg_vals)
#----- green to blue cmap
gb_vals = np.ones((Nc, 4))
gb_vals[:, 0] = np.linspace(0, 0, Nc)
gb_vals[:, 1] = np.linspace(1, 0, Nc)
gb_vals[:, 2] = np.linspace(0, 1, Nc)
gb_cm = ListedColormap(gb_vals)
#----- blue to purple cmap
bp_vals = np.ones((Nc, 4))
bp_vals[:, 0] = np.linspace(0, .5, Nc)
bp_vals[:, 1] = np.linspace(0, 0, Nc)
bp_vals[:, 2] = np.linspace(1, .5, Nc)
bp_cm = ListedColormap(bp_vals)
#----- purple to red cmap
pr_vals = np.ones((Nc, 4))
pr_vals[:, 0] = np.linspace(.5, 1, Nc)
pr_vals[:, 1] = np.linspace(0, 0, Nc)
pr_vals[:, 2] = np.linspace(.5, 0, Nc)
pr_cm = ListedColormap(pr_vals)
#----- concatenated white-green-blue-purple-red cmap
top = cm.get_cmap(wg_cm,128)
bottom = cm.get_cmap(gb_cm,128)
wgb_stack = np.vstack( (top(np.linspace(0, 1, 128)),bottom(np.linspace(0, 1, 128)) ) )
wgb_cm = ListedColormap( wgb_stack, name='WGB')

top = cm.get_cmap(bp_cm,128)
bottom = cm.get_cmap(pr_cm,128)
bpr_stack = np.vstack( (top(np.linspace(0, 1, 128)),bottom(np.linspace(0, 1, 128)) ) )
bpr_cm = ListedColormap( bpr_stack, name='BPR')

top = cm.get_cmap(wgb_cm,128)
bottom = cm.get_cmap(bpr_cm,128)
wgbpr_stack = np.vstack( (top(np.linspace(0, 1, 128)),bottom(np.linspace(0, 1, 128)) ) )
hot2cold = ListedColormap( wgbpr_stack, name='hot2cold')

"""  Data read in from a previously saved file. """
ofile="goodwin_prob2_9a.json"

goodwin_data={}
f=open(ofile,'r')
goodwin_data = json.load(f)
f.close()

params_dict = goodwin_data["params"]
data_dict = goodwin_data["data"]
w = np.array( data_dict["wages"] )
Y = np.array( data_dict["outputs"] )
t = np.array( data_dict["times"] )
wmin,wmax = 0.01,np.ceil(max(w))
Ymin,Ymax = 0.01,np.ceil(max(Y))

fig,ax = plt.subplots()
#fig = plt.figure(figsize=(10,5))
#ax = fig.add_subplot(111)
plt.grid(True,axis='both')
ax.set_title("Goodwin model: wage and output trajectory")
ax.set_xlabel('wage share, $w$')
ax.set_ylabel('Output, $Y$')
ax.set_aspect('equal')
ax.set_xlim(0, wmax)
ax.set_ylim(0, Ymax)
#fig.tight_layout()

xwd=0.7
"""  Coordinates here are in 'figure units' i.e., fractions of the canvas extent
NB: because of the colobar, the figure sizings get stuffed up 
plt.subplots_adjust( left=-.9, right=1.15, bottom=0.2, top=0.9 ) # also: top=, wspace=, hspace=
"""
plt.subplots_adjust( left=0.1, right=0.1+xwd, bottom=0.2)

""" pyplot.axes ref:  [left, bottom, width, height] 
 The colorbar stuffs up the coordinates, so I have to do a double transform to 
 screen then to figure coords to position the slider roughly.  It is still not 
 left aligned with  data coordinate X=wmin, and I do not know why?
"""
sx0, sy0 = ax.transData.transform((wmin,Ymin)) # data-to-screen coords
sx1, sy1 = ax.transData.transform((wmax,Ymin))
fx0, fy0 = fig.transFigure.inverted().transform((sx0,sy0)) # screen-to-figure cocords
fx1, fy1 = fig.transFigure.inverted().transform((sx1,sy1))

ax_t  = plt.axes([ fx0, 0.05, fx1-fx0, 0.03])

""" Slider reference:
 class matplotlib.widgets.Slider(ax, label, valmin, valmax, valinit=0.5, valfmt='%1.2f', closedmin=True, closedmax=True, slidermin=None, slidermax=None, dragging=True, valstep=None, **kwargs)[
"""
tmin,tmax = min(t), max(t) 
s_t = Slider( ax_t, r'$t=$', tmin, tmax, tmin, valfmt='%1.1f' )

def update(tval):
    global anim_running
    anim_running ^= True
    ax.clear()
    # update t array index
    i = closest_index(t,tval,tol=(tmin-tmax)/1000.)
    # plot only tmin to t_[i] when  i<tbuf
    wpts=w[0:i]
    Ypts=Y[0:i]
    tpts = t[0:1]
    if i >= tbuff:
        # plot  from  t[i-tbuff] to t_cur = t[i]
        wpts=w[i-tbuff:i]
        Ypts=Y[i-tbuff:i]
        tpts=t[i-tbuff:i]
    points = np.array([wpts, Ypts]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    
    lc = LineCollection(segments, cmap=hot2cold )# , norm=plt.Normalize(0, 1) )
    lc.set_array(tpts)
    lc.set_linewidth(3)
    ax.add_collection(lc)
    ax.scatter(wpts,Ypts,s=3,c='r')
    ax.set_title("Goodwin model: wage and output trajectory")
    ax.set_xlabel('wage share, $w$')
    ax.set_ylabel('Output, $Y$')
    ax.set_xlim(0, wmax)
    ax.set_ylim(0, Ymax)
    #plt.draw()  
    fig.canvas.draw_idle()
    
s_t.on_changed(update)

"""
Animation set up and implementations
"""
anim_running=True
nframes=len(t)
tbuff=59  # customize this to something like your data's cycle period.

# Initialization function: plot the background of each frame
def init_anim():
    wpts = w[0:1]
    Ypts = Y[0:1]
    tpts = t[0:1]
    points = np.array([wpts, Ypts]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    lc = LineCollection(segments, cmap=hot2cold ) 
    lc.set_array(tpts)
    lc.set_linewidth(3)
    ax.add_collection(lc)
    ax.scatter(wpts,Ypts,s=3,c='r')
    ax.set_title("Goodwin model: wage and output trajectory")
    ax.set_xlabel('wage share, $w$')
    ax.set_ylabel('Output, $Y$')
    ax.set_xlim(0, wmax)
    ax.set_ylim(0, Ymax)
    return ax,

def playpause(event):
    global anim
    global anim_running
    if anim_running:
        anim.event_source.stop()
        anim_running = False
    else:
        anim.event_source.start()
        anim_running = True

def on_press(event):
    if event.key.isspace():
        playpause(event)
            
# Animation function.  This is called sequentially by plt.show()
def animate(i):
    global anim_running
    if not anim_running:
        return ax,
    ax.clear()
    s_t.set_val(t[i])
    anim_running=True
    # plot only tmin to t[i] when t < tbuf
    wpts=w[0:i]
    Ypts=Y[0:i]
    tpts=t[0:i]
    if i >= tbuff:
        # plot  from  t[i-tbuff] to t = t[i]
        wpts=w[i-tbuff:i]
        Ypts=Y[i-tbuff:i]
        tpts=t[i-tbuff:i]

    points = np.array([wpts, Ypts]).T.reshape(-1, 1, 2)
    segments = np.concatenate([points[:-1], points[1:]], axis=1)
    lc = LineCollection(segments, cmap=hot2cold )
    lc.set_array(tpts)
    lc.set_linewidth(3)
    ax.add_collection(lc)
    ax.scatter(wpts,Ypts,s=3,c='r')
    ax.set_title("Goodwin model: wage and output trajectory")
    ax.set_xlabel('wage share, $w$')
    ax.set_ylabel('Output, $Y$')
    ax.set_xlim(0, wmax)
    ax.set_ylim(0, Ymax)
    return ax,

try:
    
    fig.canvas.mpl_connect('key_press_event', on_press )
    anim = animation.FuncAnimation( fig, animate, init_func=init_anim,
                                frames=nframes, interval=10, blit=False )
    #anim.save('anim_trajectories.webm', codec='libvpx-vp9', bitrate=400, fps=30 )

    plt.show()
except AttributeError:
    print("User exit?  Bye.")
