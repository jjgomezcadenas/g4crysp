import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def histoplot(var, xlabel, ylabel, bins=100, figsize=(4,4), title=""):
    fig, ax = plt.subplots(1, 1, figsize=(4,4))
    h = plt.hist(var,bins)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    return h[0],h[1]


def compute_pileup(df, ncrystals, intTime = 2.4, ai=10, af=500, step=5, debug=0):
    evtsTot = np.shape(np.unique(df.event))[0]
    if debug >= 0:
            print(f"events total = {evtsTot}")
    XPA = []
    XA = []
    for activity in range(ai, af, step):
        XA.append(activity)
        if debug >=1:
            print(f"activity= {activity}")
        evtsWindow = int(activity * intTime)
        #loops = int(evtsTot/evtsWindow)
        if debug >=1:
            print(f"events Window = {evtsWindow}")
        XP = []
        if evtsWindow < 10:
            XPA.append(0.0)
            continue
            
        for i in range(0, evtsTot-evtsWindow, evtsWindow):
            if debug >=2:
                print(f"range ->[{i},{evtsWindow+i}]")
            evts = np.unique(df.event[i:evtsWindow+i])
            norm = evts.shape[0]
            b, e = np.histogram(df.sensor_id[i:evtsWindow+i], bins=ncrystals)
            px = b[b>1]
            pl = px.shape[0]
            XP.append(pl/norm)
            if debug >=2:
                print(f"xp = {pl/norm}")
        if debug >=1:
            print(f"mean pileup = {np.mean(np.array(XP))}")
        XPA.append(np.mean(np.array(XP)))
    return  XA, XPA

def plot_pileup(xa, xp, material,figsize=(4,4)):
    fig, ax = plt.subplots(1, 1, figsize=figsize)
    plt.plot(xa,xp)
    ax.set_xlabel("Activity (MBq)")
    ax.set_ylabel("Pileup fraction")
    ax.set_title(f"Pileup, {material}")

