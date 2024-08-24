import numpy as np
import pandas as pd
import glob
import os
import sys

def collect_images(df, n= 8):
    """
    Create images (numpy arrays) from a df with fields
    event	sensor_id	amplitude
    One image per event is created. The dimension of the images
    is (n,n), with a default of 8 (8x8 SiPm array).

    """

    events = np.unique(df['event'])
    images = np.zeros((events.shape[0],n,n))
    gevt = df.groupby('event')
    i=0
    charge_matrix = np.zeros((n, n))
    for _, group in gevt:
        for _, row in group.iterrows():
            sensor_id = row['sensor_id']
            charge = row['amplitude']
            charge_matrix[sensor_id // n, sensor_id % n] = charge
        images[i]= charge_matrix
        i+=1
    return images


def prepare_images_and_metadata(csvdir, imgsufx="Img"):
    """
    Input: csvdir of the form: '/home/path/csvdir/, containing files with names:
    isensor_data_nn.csv and gamma_interactions_nn.csv

    Actions:
    1. create a directory of images: '/home/path/imgdir/ where imgdir = csvidr+suffix
    2. glob the directory, getting a list of files:
    /home/path/imgdir/isensor_data_nn.csv and /home/path/imgdir/gamma_interactions_nn.csv

    3. each file gamma_interactions_nn.csv must be copied in imgfile directory as 
    metadata_nn.csv
    4. each file /home/path/imgdir/isensor_data_nn.csv is processed to produce an image 
    that is written in image directory as images_nn.npy
    
    """

    def create_directory(directory_name):
        """
        Create directory if if doesn't exist
        
        """
        
        if not os.path.exists(directory_name):
            os.makedirs(directory_name)
            print(f"Directory '{directory_name}' created.")
        else:
            print(f"Directory '{directory_name}' already exists.")
            
        
    def imgdir_from_csvdir():
        """
        1. Split the full path using / as delimiter (sn)
        2. Get the root (e.g, the path minus the actual directory) (pn)
        3. Get the name of the csvdir (fa)
        4. Build the image dir by appending imgsufx to fa and return full path
        
        """
        sn = csvdir.split("/")
        fpl = sn[0:-1]
        pn  = '/'.join(fpl)
        fn  = sn[-1]
        fa  = f"{pn}" 
        return f"{pn}/{fn}{imgsufx}"
        
        
    def get_file_prefix_and_number(path):
        """
        1. Split the full path (/) and get last element: this is the file name (fn)
        2. Split again (_): the list (fns) contains all atoms of the name
        3. Split again the last atom (.csv) using "." and get the first element (the number nn)
        4. return the prefix and the number 
        
        """
        
        fn = path.split("/")[-1]
        fns = fn.split("_")
        nn = fns[-1].split(".")[0]
        return fns[0], nn
        

    def create_image_file(dfg, imgf):
        """
        1. get the image
        2. save as npy in images dir
        """
        
        images = collect_images(dfg)
        np.save(imgf, images)

    def create_metadata_file(dfg, metaf):
        """
        1. rename columns
        2. save as metadata in images dir.
        """
        
        dfg.rename(columns={'event': 'event_id',
                    'x': 'initial_x',
                    ' y': 'initial_y',
                    ' z': 'initial_z'}, 
                    inplace=True)
        dfg.to_csv(metaf, index=False)


    imgdir = imgdir_from_csvdir()
    create_directory(imgdir)
    csvfiles = glob.glob(f"{csvdir}/*.csv")

    # Read all the datafiles to check that nothing is corrupt before conversion
    for n, csvf in enumerate(csvfiles):
            print(f"Reading file {n+1}, file name = {csvf}")
            df = pd.read_csv(csvf, header=0).sort_values(by='event').reset_index(drop=True)

    # Now convert
    for n, csvf in enumerate(csvfiles):
        print(f"Reading file {n+1}, file name = {csvf}")
        df = pd.read_csv(csvf, header=0).sort_values(by='event').reset_index(drop=True)
        pfx, nn = get_file_prefix_and_number(csvf)

        if pfx == "gamma":
            metaf = f"{imgdir}/metadata_{nn}.csv"
            print(f" creating metadata file = {metaf}")
            create_metadata_file(df, metaf)
        elif pfx == "isensor":
            imgf = f"{imgdir}/images_{nn}.npy"
            print(f" creating image = {imgf}")
            create_image_file(df, imgf)
        else:
            print("not implemented")
            break

   
if __name__ == "__main__":
    csvdir = "/Users/jjgomezcadenas/Data/G4Prods/crystalMT/csiGammasZX0"
    prepare_images_and_metadata(csvdir, imgsufx="Img")