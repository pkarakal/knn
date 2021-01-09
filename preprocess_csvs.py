import numpy as np
import pandas as pd


def preprocess_csvs(file, rows, separator):
    data = pd.read_csv(file, header=None, skiprows=rows, sep=separator)
    data = data.apply(pd.to_numeric, errors='coerce')
    data = data.select_dtypes(include=['number'])
    data = data.applymap(lambda l: l if not np.isnan(l) else np.random.uniform(-10, 10))
    data.to_csv(file + ".processed.csv", header=None, index=None)
