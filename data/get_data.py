# Script that retrieves the NOAA GSOD data. It requires querying BigQuery
# tables where the table name is:
#       `bigquery-public-data.noaa_gsod.gsodYYYY`
#
# The station WBAN numbers and their corresponding locations are as follows:
#   -   23174: LOS ANGELES INERNATIONAL AIRPORT
#   -   23129: LONG BEACH AIRPORT
#   -   93134: DOWNTOWN L.A./USC CAMPUS
#
# NOTES:
#   -   Need a Google Cloud authentication:
#           https://cloud.google.com/docs/authentication/getting-started#auth-cloud-implicit-python
#   -   The path to the json auth file you get from the google auth needs to be
#       set in your PATH.
#       -   This means you're going to have to set it everytime you have a new
#           shell session.
#   -   bq_helper abstracts retrieving big queries through google cloud for
#       an easier time:
#           https://github.com/SohierDane/BigQuery_Helper
from bq_helper import BigQueryHelper
import pandas as pd

print('Retrieving NOAA GSOD data.')
bq_assist = BigQueryHelper("bigquery-public-data", "noaa_gsod")
print('Successfully retrieved data.')

start_yr = 2010
end_yr = 2019

for year in range(start_yr, end_yr):
    query = "SELECT stn, year, mo AS month, da AS day, temp, slp FROM `bigquery-public-data.noaa_gsod.gsod{}` WHERE wban='23174' OR wban='23129' OR wban='93134' ".format(year)

    df_weather = bq_assist.query_to_pandas_safe(query, max_gb_scanned=5)

    filename = 'socal_weather_{}.csv'.format(year)

    df_weather.to_csv(filename, index=False)

    print("Saved {}".format(filename))
