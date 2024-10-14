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


def scatter_xy(df, figsize=(4, 4)):
    fig, axs = plt.subplots(1, 3, figsize=figsize)
    
    # Scatter plot for x1 vs x2
    axs[0].scatter(df.x, df.y, alpha=0.7, edgecolor='k')
    axs[0].set_title('x vs y')
    axs[0].set_xlabel('x')
    axs[0].set_ylabel('y')

    axs[1].scatter(df.x, df.z, alpha=0.7, edgecolor='k')
    axs[1].set_title('x vs z')
    axs[1].set_xlabel('x')
    axs[1].set_ylabel('z')

    axs[2].scatter(df.y, df.z, alpha=0.7, edgecolor='k')
    axs[2].set_title('y vs z')
    axs[2].set_xlabel('y')
    axs[2].set_ylabel('z')


def scatter_xyze(df, figsize=(18,9)):
    fig, axs = plt.subplots(2, 2, figsize=figsize)
    
    # Scatter plot for x1 vs x2
    axs[0,0].scatter(df['x1'], df['x2'], alpha=0.7, edgecolor='k')
    axs[0,0].set_title('Scatter plot: x1 vs x2')
    axs[0,0].set_xlabel('x1')
    axs[0,0].set_ylabel('x2')
    
    # Scatter plot for y1 vs y2
    axs[0,1].scatter(df['y1'], df['y2'], alpha=0.7, edgecolor='k')
    axs[0,1].set_title('Scatter plot: y1 vs y2')
    axs[0,1].set_xlabel('y1')
    axs[0,1].set_ylabel('y2')
    
    # Scatter plot for z1 vs z2
    axs[1,0].scatter(df['z1'], df['z2'], alpha=0.7, edgecolor='k')
    axs[1,0].set_title('Scatter plot: z1 vs z2')
    axs[1,0].set_xlabel('z1')
    axs[1,0].set_ylabel('z2')
    
    # Scatter plot for e1 vs e2
    axs[1,1].scatter(df['e1'], df['e2'], alpha=0.7, edgecolor='k')
    axs[1,1].set_title('Scatter plot: e1 vs e2')
    axs[1,1].set_xlabel('e1')
    axs[1,1].set_ylabel('e2')
    # Adjust layout to prevent overlap
    plt.tight_layout()
    
    # Show the plots
    plt.show()


def plot_time(df, num_bins = 20, xmin=0.0, xmax=5e+3, timebin=200, figsize=(6, 4), title=""):
    
    fig, ax0 = plt.subplots(1, 1, figsize=figsize)
    _, _, _ = ax0.hist(df.time * timebin, num_bins, (xmin, xmax), weights=df.charge)
    ax0.set_xlabel("Event Time (ns)")
    ax0.set_ylabel('Events/bin')
    ax0.set_title(title)
    
    fig.tight_layout()
    plt.show()


def plot_amplitude(df, num_bins = 20, xmin=2e+4, xmax=4e+4, figsize=(6, 4), title=""):
    energies = df.groupby("event").sum().amplitude.values
    
    fig, ax0 = plt.subplots(1, 1, figsize=figsize)
    h = ax0.hist(energies, num_bins, (xmin, xmax))
    ax0.set_xlabel("Event energy")
    ax0.set_ylabel('Events/bin')
    ax0.set_title(title)
    
    fig.tight_layout()
    return h[0],h[1]
    
def event_size(df):
    return len(np.unique(df.event))


def event_edep(df):
    """
    Input: Data frame (df) ordered by event number with a column edep holding
    the energies deposited by each particle. 
    Return: the total energy deposit of the event
    """
    grdf = df.groupby('event')
    ekin = grdf['edep'].sum()
    return ekin


def event_mult(df):
    """
    Input: Data frame (df) ordered by event number. 
    Return: the multiplicity of the event
    """
    grdf = df.groupby('event')
    ekin = grdf['event'].count()
    return ekin


def event_class(df):
    """
    Input: Data frame (df) ordered by event number with a field `proc` describing creating process. 
    Return: fraction of Compton and fraction of Photo.
    """
    phot = df[df['ntrk'] == "phot"]
    compt = df[df['proc'] == "compt"]
    return phot.size/df.size, compt.size/df.size


def fiducial_select(df, d=48.2, z=37.2):
    df2 = df[np.abs(df['x'])<d/2]
    df3 = df2[np.abs(df2['y'])<d/2]
    df4 = df3[np.abs(df3['z'])<z/2]
    return df4


def fiducial_df(df, d=48.2, z=37.2):
    gdf = fiducial_select(df, d, z)
    grdf = gdf.groupby('event')
    gdfa = grdf.agg(
                etot = ('edep', 'sum'),
                ntrk = ('edep', 'count')).reset_index()
    
    return pd.merge(gdf, gdfa, on='event')


def twocluster_df(df):
    """
    Creates a df with the position and energy of the two clusters of maximum energy
    as well as their baricenter
    """
    grouped = df.groupby('event')

    # Initialize lists to store results
    results = []
    
    # Iterate over each event group
    for event_id, group in grouped:
        # Sort particles by energy in descending order
        sorted_group = group.sort_values(by='time', ascending=True).reset_index(drop=True)
        
        # Extract etot, ntrk from the first particle (they are the same for all particles in the event)
        event = sorted_group.loc[0, 'event']
        etot = sorted_group.loc[0, 'etot']
        ntrk = sorted_group.loc[0, 'ntrk']
        
        # Get particle with early time
        t1, x1, y1, z1, e1 = sorted_group.loc[0, ['time','x', 'y', 'z', 'edep']]
        
        # Get particle with second early time
        if len(sorted_group) > 1:
            t2, x2, y2, z2, e2 = sorted_group.loc[1, ['time','x', 'y', 'z', 'edep']]
        else:
            t2, x2, y2, z2, e2 = t1, x1, y1, z1, e1  # If no second particle, set to first particle
        
        # Calculate the baricenter of x1, y1, z1, e1 and x2, y2, z2, e2 (norm to tot energy)
        if e2 != e1:
            x12 = (x1 * e1 + x2 * e2) / etot
            y12 = (y1 * e1 + y2 * e2) / etot
            z12 = (z1 * e1 + z2 * e2) / etot
        else:
            x12, y12, z12 = x1, y1, z1  # If only one particle, baricenter is that particle's coordinates
        
        # Calculate the baricenter of all particles in the event
        xb = (group['x'] * group['edep']).sum() / etot
        yb = (group['y'] * group['edep']).sum() / etot
        zb = (group['z'] * group['edep']).sum() / etot
        
        # Append the results
        results.append({'event':event, 'etot':etot,
            'ntrk': ntrk,
            't1': t1, 'x1': x1, 'y1': y1, 'z1': z1, 'e1': e1,
            't2': t2, 'x2': x2, 'y2': y2, 'z2': z2, 'e2': e2,
            'x12': x12, 'y12': y12, 'z12': z12,
            'xb': xb, 'yb': yb, 'zb': zb
        })
    
    # Create the final DataFrame
    return pd.DataFrame(results).reset_index()


def three_cluster_df(df):
    """
    Creates a df with the position and energy of the three clusters of maximum energy
    as well as their baricenter
    """
    grouped = df.groupby('event')

    # Initialize lists to store results
    results = []
    
    # Iterate over each event group
    for event_id, group in grouped:
        # Sort particles by energy in descending order
        sorted_group = group.sort_values(by='edep', ascending=False).reset_index(drop=True)
        
        # Extract etot, ntrk and proc from the first particle (they are the same for all particles in the event)
        event = sorted_group.loc[0, 'event']
        etot = sorted_group.loc[0, 'etot']
        ntrk = sorted_group.loc[0, 'ntrk']
        proc = sorted_group.loc[0, 'proc']
        
        # Get particle with maximum energy
        x1, y1, z1, e1 = sorted_group.loc[0, ['x', 'y', 'z', 'edep']]
        
        # Get particle with second maximum energy (if available)
        if len(sorted_group) > 1:
            x2, y2, z2, e2 = sorted_group.loc[1, ['x', 'y', 'z', 'edep']]
        else:
            x2, y2, z2, e2 = x1, y1, z1, e1  # If no second particle, set to first particle


        # Get particle with third maximum energy (if available)
        if len(sorted_group) > 2:
            x3, y3, z3, e3 = sorted_group.loc[2, ['x', 'y', 'z', 'edep']]
        else:
            x3, y3, z3, e3 = x2, y2, z2, e2  # If no second particle, set to second particle
        
        # Calculate the baricenter of all particles in the event
        total_energy = group['edep'].sum()
        xb = (group['x'] * group['edep']).sum() / etot
        yb = (group['y'] * group['edep']).sum() / etot
        zb = (group['z'] * group['edep']).sum() / etot


        # Calculate d12: Distance between (x1, y1, z1) and (x2, y2, z2)
        d12 = np.sqrt((x2 - x1)**2 + (y2 - y1)**2 + (z2 - z1)**2)

    # Calculate d1b: Distance between (x1, y1, z1) and (xb, yb, zb) 
        d1b = np.sqrt((xb - x1)**2 + (yb - y1)**2 + (zb - z1)**2)

        # Append the results
        results.append({'event':event, 'etot':etot,
            'ntrk': ntrk,
            'proc': proc,
            'x1': x1, 'y1': y1, 'z1': z1, 'e1': e1,
            'x2': x2, 'y2': y2, 'z2': z2, 'e2': e2,
            'x3': x3, 'y3': y3, 'z3': z3,'e3': e3,
            'xb': xb, 'yb': yb, 'zb': zb, 'd12':d12, 'd1b':d1b
        })
    
    # Create the final DataFrame
    return pd.DataFrame(results).reset_index()


def add_distances(df):
    # Calculate d12: Distance between (x1, y1, z1) and (x2, y2, z2)
    df['d12'] = np.sqrt((df['x2'] - df['x1'])**2 + (df['y2'] - df['y1'])**2 + (df['z2'] - df['z1'])**2)
    
    # Calculate d12b: Distance between (x12, y12, z12) and (xb, yb, zb)
    df['d12b'] = np.sqrt((df['xb'] - df['x12'])**2 + (df['yb'] - df['y12'])**2 + (df['zb'] - df['z12'])**2)

    # Calculate d1b: Distance between (x1, y1, z1) and (x12, y12, z12) 
    df['d1b'] = np.sqrt((df['x12'] - df['x1'])**2 + (df['y12'] - df['y1'])**2 + (df['z12'] - df['z1'])**2)


