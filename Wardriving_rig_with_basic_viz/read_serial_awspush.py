import json
from awscrt import io, mqtt
import mqtt_connection_builder
import serial
import time
#arudino block

ser = serial.Serial(
    port="COM3",  # Replace with the port name for your Arduino board
    baudrate=9600,
    timeout=1
    )
ser2 = serial.Serial(
    port="COM5",
    baudrate=115200,
    timeout=2
)
while True:
    line = ser2.readline().decode("utf-8").strip()
    line_lat_long = ser.readline().decode("utf-8").strip()

    if line and line_lat_long:
        try:
            json_data1 = json.loads(line)
            json_data2 = json.loads(line_lat_long)

            # merge the two JSON objects
            json_data1.update(json_data2)
            print(json.dumps(json_data1, indent=4))
        except json.JSONDecodeError as e:
            print(f'Error decoding JSON data: {e}')
    elif line:
        try:
            json_data1 = json.loads(line)
            print(json.dumps(json_data1, indent=4))
        except json.JSONDecodeError as e:
            print(f'Error decoding JSON data: {e}')
    elif line_lat_long:
        try:
            json_data2 = json.loads(line_lat_long)
            print(json.dumps(json_data2, indent=4))
        except json.JSONDecodeError as e:
            print(f'Error decoding JSON data: {e}')

    time.sleep(1)
    if line:
        data = (json.dumps(json_data1, indent=4))
        # Define your endpoint, certificate file path and private key file path
        ENDPOINT = "a942cy2hw050q-ats.iot.us-west-2.amazonaws.com"
        CERTIFICATE = "C:/Users/niles/Desktop/New folder (2)/bkb.cert.pem"
        PRIVATE_KEY = "C:/Users/niles/Desktop/New folder (2)/bkb.private.key"
        # Define your topic and message
        TOPIC = "your/details"
        # Initialize the event loop group and create the MQTT connection
        event_loop_group = io.EventLoopGroup(1)
        host_resolver = io.DefaultHostResolver(event_loop_group)
        # Create the client bootstrap
        client_bootstrap = io.ClientBootstrap(
            event_loop_group=event_loop_group,
            host_resolver=host_resolver
        )
        mqtt_connection = mqtt_connection_builder.mtls_from_path(
            endpoint=ENDPOINT,
            cert_filepath=CERTIFICATE,
            pri_key_filepath=PRIVATE_KEY,
            #client_bootstrap=io.ClientBootstrap(event_loop_group),
            ca_filepath="C:/Users/niles/Desktop/New folder (2)/root-CA.crt",
            on_connection_interrupted=None,
            on_connection_resumed=None,
            client_id="test",
            clean_session=False,
            keep_alive_secs=6,
            #mqtt_version=mqtt.MqttVersion.MQTT_5
        )
        # Connect to AWS IoT Core
        print("Connecting to AWS IoT Core. :{}".format(ENDPOINT))
        connect_future = mqtt_connection.connect()
        connect_future.result()
        print("Connected!")
        # Subscribe to the topic
        def on_message_received(topic, payload, **kwargs):
            print(f"Received message from topic '{topic}': {payload}")
        print(f"Subscribing to topic '{TOPIC}'...")
        subscribe_future, packet_id = mqtt_connection.subscribe(
            topic=TOPIC,
            qos=mqtt.QoS.AT_LEAST_ONCE,
            callback=on_message_received
        )
        subscribe_future.result()
        print("Subscribed with qos: 1")
        mqtt_connection.publish(
            topic=TOPIC,
            payload=json.dumps(data),
            qos=mqtt.QoS.AT_LEAST_ONCE
            )
        print("Published!")
        # Disconnect from AWS IoT Core
        print("Disconnecting from AWS IoT Core...")
        disconnect_future = mqtt_connection.disconnect()
        disconnect_future.result()
        print("Disconnected!")




