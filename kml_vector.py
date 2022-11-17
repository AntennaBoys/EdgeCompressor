import pandas as pd
import simplekml
import os
from dotenv import load_dotenv

load_dotenv()
blarn = pd.read_csv("out.csv", parse_dates=True)

kml = simplekml.Kml()

vectors = pd.read_csv("vector_plot.csv")
for i, row in enumerate(vectors.iterrows()):
    kml.newpoint(name=i, coords=[(row[1][1], row[1][0])])

kml.newlinestring(name="Ship Trajectory", coords=list(blarn[['Longitude', 'Latitude']].to_records(index=False)))
kml.save(path="data.kml")