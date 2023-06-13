import dash
from dash import dcc
from dash import html
import pandas as pd
import json
import plotly.express as px

app = dash.Dash(__name__)

# Sample data from aws dashboard.
data = {
  "networks": [
    {
      "bssid": "54:AF:97:05:EB:BC",
      "ssid": "TP-Link_EBBC",
      "ssid_short": "TP-Link_EBBC",
      "rssi": -58,
      "encryption": "[WPA2-PSK-CCMP+TKIP][ESS]",
      "channel": 2,
      "wep_detected": 'false'
    },
    {
      "bssid": "10:F0:68:65:8B:10",
      "ssid": "The Province",
      "ssid_short": "The Province",
      "rssi": -54,
      "encryption": "[ESS]",
      "channel": 6,
      "wep_detected": 'false'
    },
    {
      "bssid": "10:F0:68:65:97:80",
      "ssid": "The Province",
      "ssid_short": "The Province",
      "rssi": -48,
      "encryption": "[ESS]",
      "channel": 6,
      "wep_detected": 'false'
    },
    {
      "bssid": "08:40:F3:6C:0C:81",
      "ssid": "crack home",
      "ssid_short": "crack home",
      "rssi": -72,
      "encryption": "[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]",
      "channel": 8,
      "wep_detected": 'false'
    },
    {
      "bssid": "6C:CD:D6:BF:00:54",
      "ssid": "Netgear Thing",
      "ssid_short": "Netgear Thing",
      "rssi": -74,
      "encryption": "[WPA2-PSK-CCMP+TKIP][ESS]",
      "channel": 9,
      "wep_detected": 'false'
    }
  ],
  "latitude": 43.08418,
  "longitude": -77.65626
}
ssid_test = []
ssid_tes = []
# Create a DataFrame with the network information
networks_df = pd.DataFrame(data)
print(networks_df.head())
#ssids = [[net['ssid'] for net in networks_df['networks'] if (net['latitude'], net['longitude']) == (networks_df[
# 'latitude'], networks_df['longitude'])]]
#print(ssids)
#ssid_tes = [str(ssid) for ssid in networks_df['networks']['ssid'].get('ssid')]
#print(ssid_tes)
# Create a Plotly scatter map with markers for each network
for i in range(len(networks_df['networks'])):
  for j in networks_df['networks'][i]:
    if j == 'ssid':
      ssid_test.append([(str(networks_df['networks'][i].get(j)))])
print(ssid_test)
fig = px.scatter_mapbox(networks_df, lat="latitude", lon="longitude", hover_data=[ssid_test], zoom=13)

# Set the map style and margins
fig.update_layout(mapbox_style="carto-positron")
fig.update_layout(margin={"r":0,"t":0,"l":0,"b":0})

# Create a Dash app and layout
app = dash.Dash(__name__)
app.layout = html.Div([
    dcc.Graph(id='networks-map', figure=fig)
])

# Run the app
if __name__ == '__main__':
    app.run_server(debug=True)