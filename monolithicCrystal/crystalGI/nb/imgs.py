import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def select_image_from_df2(df, evtsel = 10):
    gevt = df.groupby('event')
    n = 8
    charge_matrix = np.zeros((n, n))
    for event_number, group in gevt:
        if event_number == evtsel:
            for _, row in group.iterrows():
                sensor_id = row['sensor_id']
                charge = row['amplitude']
                charge_matrix[sensor_id // n, sensor_id % n] = charge
            break
    return charge_matrix


def select_image_from_df(df,evtsel, n = 8):
    df2=df[df.event==evtsel]
    
    charge_matrix = np.zeros((n, n))
    sensor_id = df2['sensor_id'].values
    charge    = df2['amplitude'].values

    for id in sensor_id:
        charge_matrix[sensor_id[id] // n, sensor_id[id] % n] = charge[id]
    return charge_matrix


def get_gamma_position(dfg, evtsel, x_spatial, y_spatial):
    df = dfg[dfg.event==evtsel]
    print(f"xg1 = {df.x1.values[0]}, yg1 ={df.y1.values[0]}")
    print(f"xg2 = {df.x1.values[0]}, yg2 ={df.y1.values[0]}")
    
    xt1, yt1 = transform_coordinates(df.x1.values[0], df.y1.values[0], 
                                     x_spatial, y_spatial)

    xt2, yt2 = transform_coordinates(df.x2.values[0], df.y2.values[0], 
                                     x_spatial, y_spatial)
    print(xt1, yt1)
    print(xt2, yt2)

    return xt1, yt1,xt2, yt2
    

def plot_image(dfq, dfg,  evtsel, x_spatial, y_spatial, figsize=(6, 6)):
    
    charge_matrix = select_image_from_df(dfq,evtsel)
    xt1, yt1,xt2, yt2 =get_gamma_position(dfg, evtsel, x_spatial, y_spatial)
    
    # Create the plot
    fig, ax1 = plt.subplots(figsize=figsize)

    # Plot the image with imshow (with pixel axis from 0 to 8)
    img = ax1.imshow(charge_matrix.T, extent=[0, 8, 0, 8], origin='lower', aspect='auto',
                    cmap='viridis', interpolation='none')

    # Add colorbar for the imshow plot
    cbar = fig.colorbar(img, ax=ax1, pad=0.18)
    cbar.set_label('Charge')

    # Create a secondary x-axis (spatial) that matches the pixel axis
    ax2 = ax1.twiny()
    ax2.set_xlim(x_spatial[0], x_spatial[-1])
    ax2.set_xlabel('X')

    # Create a secondary y-axis (spatial) that matches the pixel axis
    ax3 = ax1.twinx()
    ax3.set_ylim(y_spatial[0], y_spatial[-1])
    ax3.set_ylabel('Y')

    # Show the plot

    ax1.scatter(xt1, yt1,  facecolor='red')
    ax1.scatter(xt2, yt2,  facecolor='blue')
    plt.tight_layout()
    plt.show()
    

def select_event(evtsel, dfop, dfg):
    charge_matrix = select_image_from_df(dfop,evtsel)
    plot_image(charge_matrix, evtsel, figsize=(6, 4))
    df = dfg[dfg.event==evtsel]

    print(f"event = {df.event.values[0]}, ntrk = {df.ntrk.values[0]}")
    print(f"x1 = {df.x1.values[0]}, y1 = {df.y1.values[0]}, z1 = {df.z1.values[0]}, e1 = {df.e1.values[0]}")
    print(f"x2 = {df.x2.values[0]}, y2 = {df.y2.values[0]}, z2 = {df.z2.values[0]}, e2 = {df.e2.values[0]}")
    print(f"xb = {df.xb.values[0]:.2f}, y2 = {df.yb.values[0]:.2f}, z2 = {df.zb.values[0]:.2f}")
    print(f"d12 = {df.d12.values[0]:.2f}, d12b = {df.d12b.values[0]:.2f}, d1b = {df.d1b.values[0]:.2f}")



def transform_coordinates(x, y, x_spatial, y_spatial, x_min2=0,   x_max2=8,  y_min2=0,   y_max2 = 8):
                        
    x_min1=x_spatial[0] 
    x_max1=x_spatial[-1] 
    y_min1=y_spatial[0]
    y_max1 = y_spatial[-1]
    
    # Apply the transformation for x and y
    x_new = ((x - x_min1) / (x_max1 - x_min1)) * (x_max2 - x_min2) + x_min2
    y_new = ((y - y_min1) / (y_max1 - y_min1)) * (y_max2 - y_min2) + y_min2
    
    return x_new, y_new



def select_image_from_df3(df, dfs,evtsel = 10,  n = 8):

    Q = np.zeros(n*n)
    xM = np.zeros(n*n)
    yM = np.zeros(n*n)
    
    df2=df[df.event==evtsel]
    sensor_id = df2['sensor_id'].values
    charge    = df2['amplitude'].values

    for id in sensor_id:
        dfs2=dfs[dfs.sensor_id == id]
        xM[id] = dfs2.sensor_x.values[0]
        yM[id] = dfs2.sensor_y.values[0]
        Q[id]  = charge[id]

    # Create scatter plot

    dxy = 40  # Assuming x-axis spans from 0 to 25, and there are 8 bins
    # Calculate the size of markers in points² (note: marker size in scatter is in points²)
    marker_size = (72.0 / plt.gcf().dpi) * dxy  # Convert bin size to points


    plt.scatter(xM, yM, c=Q, s=marker_size**2,cmap='coolwarm', marker='s', edgecolor='black')

    # Add color bar to represent the energy scale
    plt.colorbar(label='Charge')

    # Add labels and title
    plt.xlabel('X coordinates')
    plt.ylabel('Y coordinates')



def xyQ_from_df(df, dfs,evtsel = 10,  n = 8):

    Q = np.zeros(n*n)
    xM = np.zeros(n*n)
    yM = np.zeros(n*n)
    
    df2=df[df.event==evtsel]
    sensor_id = df2['sensor_id'].values
    charge    = df2['amplitude'].values

    for id in sensor_id:
        dfs2=dfs[dfs.sensor_id == id]
        xM[id] = dfs2.sensor_x.values[0]
        yM[id] = dfs2.sensor_y.values[0]
        Q[id]  = charge[id]

    return xM,yM,Q



def plot_event(evtsel, dfop, dfg, dfs, figsize=(6, 4), xmin=-25, xmax=25):
    
    xM,yM,Q = xyQ_from_df(dfop, dfs, evtsel)
    df = dfg[dfg.event==evtsel]
    
    fig, axs = plt.subplots(2, 2, figsize=figsize)
    axs = axs.ravel()
    
    dxy = 20  # Assuming x-axis spans from 0 to 25, and there are 8 bins
    # Calculate the size of markers in points² (note: marker size in scatter is in points²)
    marker_size = (72.0 / plt.gcf().dpi) * dxy  # Convert bin size to points


    axs[0].scatter(xM, yM, c=Q, s=marker_size**2,cmap='coolwarm', marker='s', edgecolor='black')
    fig.colorbar(axs[0].scatter(xM, yM, c=Q, s=marker_size**2,cmap='coolwarm', marker='s', edgecolor='black'), ax=axs[0], label='Charge')
    
    # Add labels and title
    plt.xlabel('X coordinates')
    plt.ylabel('Y coordinates')

    xg, yg = df.xb.values[0], df.yb.values[0]
    axs[0].scatter(xg, yg,  facecolor='black')
    axs[0].set_xlabel('Sensor X')
    axs[0].set_ylabel('Sensor Y')
    
    

    axs[1].scatter(df.x1, df.y1,  facecolor='red')
    axs[1].scatter(df.x2, df.y2,  facecolor='green')
    axs[1].scatter(df.x3, df.y3,  facecolor='blue')
    axs[1].scatter(df.xb, df.yb,  facecolor='black')
    axs[1].set_xlim([xmin, xmax])
    axs[1].set_ylim([xmin, xmax])
    axs[1].set_xlabel('x')
    axs[1].set_ylabel('y')

    axs[2].scatter(df.x1, df.z1,  facecolor='red')
    axs[2].scatter(df.x2, df.z2,  facecolor='green')
    axs[2].scatter(df.x3, df.z3,  facecolor='blue')
    axs[2].scatter(df.xb, df.zb,  facecolor='black')
    axs[2].set_xlim([xmin, xmax])
    axs[2].set_ylim([xmin, xmax])
    axs[2].set_xlabel('x')
    axs[2].set_ylabel('z')

    axs[3].scatter(df.y1, df.z1,  facecolor='red')
    axs[3].scatter(df.y2, df.z2,  facecolor='green')
    axs[3].scatter(df.y3, df.z3,  facecolor='blue')
    axs[3].scatter(df.yb, df.zb,  facecolor='black')
    axs[3].set_xlim([xmin, xmax])
    axs[3].set_ylim([xmin, xmax])
    axs[3].set_xlabel('y')
    axs[3].set_ylabel('z')

    plt.tight_layout()

    #print(f"event = {df.event.values[0]}, ntrk = {df.ntrk.values[0]}")
    #print(f"x1 = {df.x1.values[0]}, y1 = {df.y1.values[0]}, z1 = {df.z1.values[0]}, e1 = {df.e1.values[0]}")
    #print(f"x2 = {df.x2.values[0]}, y2 = {df.y2.values[0]}, z2 = {df.z2.values[0]}, e2 = {df.e2.values[0]}")
    #print(f"x3 = {df.x3.values[0]}, y3 = {df.y3.values[0]}, z3 = {df.z3.values[0]}, e3 = {df.e3.values[0]}")
    #print(f"xb = {df.xb.values[0]:.2f}, yb = {df.yb.values[0]:.2f}, zb = {df.zb.values[0]:.2f}")
    #print(f"d12 = {df.d12.values[0]:.2f}, d1b = {df.d1b.values[0]:.2f}")
    
def plot_event2(evtsel, dfop, dfg, dfs, figsize=(6, 4), xmin=-25, xmax=25):
    
    charge_matrix = select_image_from_df(dfop,evtsel)
    df = dfg[dfg.event==evtsel]
    xg, yg = transform_coordinates(df.x1.values[0], df.y1.values[0])
    
    fig, axs = plt.subplots(2, 2, figsize=figsize)
    axs = axs.ravel()
    #axs[0].imshow(charge_matrix, cmap='viridis', interpolation='none')
    fig.colorbar(axs[0].imshow(charge_matrix, cmap='viridis', interpolation='none'), ax=axs[0], label='Charge')
    axs[0].scatter(xg, yg,  facecolor='red')
    axs[0].set_xlabel('Sensor X')
    axs[0].set_ylabel('Sensor Y')
    
    

    axs[1].scatter(df.x1, df.y1,  facecolor='red')
    axs[1].scatter(df.x2, df.y2,  facecolor='green')
    axs[1].scatter(df.x3, df.y3,  facecolor='blue')
    axs[1].scatter(df.xb, df.yb,  facecolor='black')
    axs[1].set_xlim([xmin, xmax])
    axs[1].set_ylim([xmin, xmax])
    axs[1].set_xlabel('x')
    axs[1].set_ylabel('y')

    axs[2].scatter(df.x1, df.z1,  facecolor='red')
    axs[2].scatter(df.x2, df.z2,  facecolor='green')
    axs[2].scatter(df.x3, df.z3,  facecolor='blue')
    axs[2].scatter(df.xb, df.zb,  facecolor='black')
    axs[2].set_xlim([xmin, xmax])
    axs[2].set_ylim([xmin, xmax])
    axs[2].set_xlabel('x')
    axs[2].set_ylabel('z')

    axs[3].scatter(df.y1, df.z1,  facecolor='red')
    axs[3].scatter(df.y2, df.z2,  facecolor='green')
    axs[3].scatter(df.y3, df.z3,  facecolor='blue')
    axs[3].scatter(df.yb, df.zb,  facecolor='black')
    axs[3].set_xlim([xmin, xmax])
    axs[3].set_ylim([xmin, xmax])
    axs[3].set_xlabel('y')
    axs[3].set_ylabel('z')

    plt.tight_layout()

    #print(f"event = {df.event.values[0]}, ntrk = {df.ntrk.values[0]}")
    #print(f"x1 = {df.x1.values[0]}, y1 = {df.y1.values[0]}, z1 = {df.z1.values[0]}, e1 = {df.e1.values[0]}")
    #print(f"x2 = {df.x2.values[0]}, y2 = {df.y2.values[0]}, z2 = {df.z2.values[0]}, e2 = {df.e2.values[0]}")
    #print(f"x3 = {df.x3.values[0]}, y3 = {df.y3.values[0]}, z3 = {df.z3.values[0]}, e3 = {df.e3.values[0]}")
    #print(f"xb = {df.xb.values[0]:.2f}, yb = {df.yb.values[0]:.2f}, zb = {df.zb.values[0]:.2f}")
    #print(f"d12 = {df.d12.values[0]:.2f}, d1b = {df.d1b.values[0]:.2f}")
    