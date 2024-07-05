import pandas as pd

def read_parquet_file(file_path):
    # Read the Parquet file into a DataFrame
    df = pd.read_parquet(file_path)
    return df

def main():
    # Specify the path to the Parquet file
    file_path = "events.parquet"

    # Read the Parquet file
    df = read_parquet_file(file_path)

    # Display the DataFrame
    print("Data read from Parquet file:")
    print(df)

if __name__ == "__main__":
    main()
