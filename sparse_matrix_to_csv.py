import pandas as pd
import re as regex


def convert_dictionary_to_csv(file):
    data = pd.read_table(file, header=None)
    s = data[0].str.split(' ')
    for x in s:
        x[0] = regex.sub('^', 'Label:', x[0])
    m = [list(w for w in x if w)for x in s]
    d = [dict(w.split(':') for w in x) for x in m]
    cols = sorted(d, key=len, reverse=True)[0].keys()
    df = pd.DataFrame.from_records(d, index=data.index, columns=cols)
    dataframe_cols = list(df.columns)
    dataframe_cols.remove('Label')
    dataframe_cols = list(map(int, dataframe_cols))
    dataframe_cols.sort()
    dataframe_cols = list(map(str, dataframe_cols))
    df = df[dataframe_cols]
    df = df[dataframe_cols]
    df = df.fillna(0)
    df.to_csv(file+".csv", index=False)

