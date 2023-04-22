import serial
import time
import requests
import sys
import glob
import serial.tools.list_ports as COMs
from serial import SerialException
import math

def send_command(ser, command):
    ser.write(command.encode())


def port_search():
    if sys.platform.startswith('win'): # Windows
        ports = ['COM{0:1.0f}'.format(ii) for ii in range(1,256)]
    else:
        raise EnvironmentError('Machine Not pyserial Compatible')
    arduinos = []
    for port in ports: # loop through to determine if accessible
        if len(port.split('Bluetooth'))>1:
            continue
        try:
            ser = serial.Serial(port)
            ser.close()
            arduinos.append(port) # if we can open it, consider it an arduino
        except (OSError, serial.SerialException):
            pass
    return arduinos
    
def traverse_json(url):
    # Get first JSON blob and find 'x' key
    headers = {
        'User-Agent': 'my-weather-app/1.0 (myemail@example.com)',
        'Accept': 'application/geo+json'
    }     
    response = requests.get(url, headers=headers)
    json1 = response.json()
    next_url= json1['properties']['observationStations']
    print("Student Name : Nilesh J" )
    print("Coordinates " + url.rsplit('/', 1)[-1])
    print('Forecast Office: '+ next_url)
    
    response = requests.get(next_url)
    data = response.json()
    next_url = data['observationStations'][0]
    wwe = next_url.rsplit('/', 1)[-1]
    print("Observation Station: "+ wwe)

    final_url = next_url + '/observations/latest'
    response = requests.get(final_url)
    final_data = response.json()
    temperature = final_data['properties']['temperature']['value']
    relative_humidity = final_data['properties']['relativeHumidity']['value']
    wind_speed = final_data['properties']['windSpeed']['value']    
    return temperature,relative_humidity,wind_speed

def heat_index_wind_chill(temperature_celsius, relative_humidity, wind_speed_kmh):
    temperature_fahrenheit = temperature_celsius * 9/5 + 32
    wind_speed_mph = wind_speed_kmh / 1.60934
    if temperature_fahrenheit <= 50 and wind_speed_mph > 3:
        wind_chill_fahrenheit = 35.74 + 0.6215 * temperature_fahrenheit - 35.75 * math.pow(wind_speed_mph, 0.16) + 0.4275 * temperature_fahrenheit * math.pow(wind_speed_mph, 0.16)
        wind_chill_celsius = (wind_chill_fahrenheit - 32) * 5/9
        wind_chill = "Wind Chill Index: {:.2f}°C | {:.2f}°F".format(wind_chill_celsius, wind_chill_fahrenheit)
        steadman_hi = None
        rothfusz_hi = None
    else:
        if temperature_fahrenheit < 80:
            heat_index_fahrenheit = 0.5 * (temperature_fahrenheit + 61 + (temperature_fahrenheit - 68) * 1.2 + relative_humidity * 0.094)
            heat_index_celsius = (heat_index_fahrenheit -32)*5/9
            steadman_hi = "Steadman HI: {:.2f}°C".format(heat_index_celsius)
            rothfusz_hi = None
            wind_chill = None
        else:
            heat_index_fahrenheit = -42.379 + 2.04901523 * temperature_fahrenheit + 10.14333127 * relative_humidity - 0.22475541 * temperature_fahrenheit * relative_humidity - 0.00683783 * math.pow(temperature_fahrenheit, 2) - 0.05481717 * math.pow(relative_humidity, 2) + 0.00122874 * math.pow(temperature_fahrenheit, 2) * relative_humidity + 0.00085282 * temperature_fahrenheit * math.pow(relative_humidity, 2) - 0.00000199 * math.pow(temperature_fahrenheit, 2) * math.pow(relative_humidity, 2)
            if relative_humidity <13 and (80 <= temperature_fahrenheit <=112):
                heat_index_fahrenheit -= ((13-relative_humidity)/4)*math.sqrt((17-abs(temperature_fahrenheit-95))/17)
            elif relative_humidity >85 and (80 <= temperature_fahrenheit <=87):
                heat_index_fahrenheit += ((relative_humidity-85)/10)*((87-temperature_fahrenheit)/5)
            heat_index_celsius = (heat_index_fahrenheit -32)*5/9
            steadman_hi = None
            #temp = "Normal Heat Index: {:.2f}°C".format(heat_index_celsius)
            rothfusz_hi = "Rothfusz HI: {:.2f}°C ".format(heat_index_celsius)
            wind_chill = None
    return  wind_chill, steadman_hi, rothfusz_hi


def check_severity_level(celsius: float, index: str) -> tuple:
    """
    :param celsius: apparent temperature in Celsius
    :param index: either 'heat' or 'wind chill'
    :return: tuple containing the severity level and servo angle
    """
    if index == 'heat':
        if 26 <= celsius < 32:
            return 'Caution', 79
        elif 32 <= celsius < 41:
            return 'Extreme Caution', 56
        elif 41 <= celsius < 54:
            return 'Danger', 34
        elif celsius >= 54:
            return 'Extreme Danger', 11
    elif index == 'wind chill':
        if celsius > -25:
            return 'Cold', 101
        elif -35 < celsius <= -25:
            return 'Very Cold', 124
        elif -60 < celsius <= -35:
            return 'Danger', 146
        elif celsius <= -60:
            return 'Great Danger', 169
    return 'Normal', 90    


def main():
    t,rh,ws = traverse_json('https://api.weather.gov/points/43.08588176303043,-77.67114945412571')
    if ((t or rh or ws) == None):
        if t is None:
            t = 0
        if rh is None:
            rh = 0
        if ws is None:
            ws = 0
    #returns in this order [temperature,relative_humidity,wind_speed]
    print("Temperature: ",t,"C")
    print("RH: ",rh,"%")
    print("Wind Speed: ",ws,"Km/H")
    #wee = wind_chill_or_heat_index(t,rh,ws)
    #apparent_temperature = heat_index_wind_chill(t,rh, ws)
    index=''
    #print(wee)
    waw = heat_index_wind_chill(t,rh,ws)
    for i in range(len(waw)):
        if waw[i] != None:
            print(waw[i])
            if 'Wind' in waw[i]:
                index = 'wind chill'
            elif 'heat' in waw[i]:
                index = 'heat'
            else:
                print('bambo')
    
    teste = check_severity_level(t,index)
    print(teste[0])
    arduino_ports = port_search()
    ser = serial.Serial(arduino_ports[0],baudrate=9600) # match baud on Arduino
    ser.flush() # clear the port
    time.sleep(2)
    send_command(ser, str(teste[1]))
    print(teste[1])
        
main()
#This function takes in the temperature in degrees Celsius, the relative humidity and the wind speed in km/h and returns the Wind Chill Index, Steadman Heat Index and Rothfusz Heat Index as strings in the format you specified. If the Wind Chill Index is applicable (i.e., the temperature is below or equal to 50°F and the wind speed is above or equal to 3 mph), only the Wind Chill Index value is returned and the Steadman and Rothfusz Heat Index values are None. If the Wind Chill Index is not applicable and the temperature is below or equal to 80°F, only the Steadman Heat Index value is returned and the Wind Chill Index and Rothfusz Heat Index values are None. If the Wind Chill Index is not applicable and the temperature is above or equal to 80°F, only the Rothfusz Heat Index value is returned and the Wind Chill Index and Steadman Heat Index values are None.