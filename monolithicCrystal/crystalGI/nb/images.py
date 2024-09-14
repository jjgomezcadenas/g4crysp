import numpy as np
import pandas as pd
import glob

def collect_images(df, n= 8):
    events = np.unique(df['event'])
    images = np.zeros((events.shape[0],n,n))
    gevt = df.groupby('event')
    i=0
    charge_matrix = np.zeros((n, n))
    for event_number, group in gevt:
        for _, row in group.iterrows():
            sensor_id = row['sensor_id']
            charge = row['amplitude']
            charge_matrix[sensor_id // n, sensor_id % n] = charge
        images[i]= charge_matrix
        i+=1
    return images


df = pd.read_csv("integrated_sensor_data.csv", header=0)
images = collect_images(df)
fln = 'images2'
np.save(fln, images)
print(f"Saved {images.shape[0]} images to file {fln}")
