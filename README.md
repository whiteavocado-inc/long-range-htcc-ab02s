# long-range-htcc-ab02s
LoRa communication project (1-10km range)


<h2>Buy list</h2>

* Integrated LoRa microcontroller, includes GPS ([heltech](https://heltec.org/project/htcc-ab02s/) | [tinytronics](https://www.tinytronics.nl/en/development-boards/microcontroller-boards/met-gps/heltec-cubecell-lora-development-board-gnss-868mhz-met-0.96-inch-oled-display)) ~€25 x 2 (for receiving and sending)

<code>Est price: ~€50 for two devices</code>

<h2>Schematic of LoRa module + integrated microcontroller</h2>
<img width="1024" height="507" alt="lo-ra" src="https://github.com/user-attachments/assets/2ace8b85-6a6b-4dd7-9994-480567d969ed" />

<h2>Controller</h2>

<p>Use the [Controller](https://github.com/whiteavocado-inc/long-range-htcc-ab02s/tree/main/code/controller) to connect with the LoRa module by USB-cable</p>

<p>To setup the Controller, you can execute the following commands in the folder with the main.py and .env file</p>

* <code>python3 -m venv env</code>
* For Windows: <code>call env/Scripts/activate</code> For Linux: <code>source env/bin/activate</code>
* <code>python3 -m pip install pyserial pycryptodome python-dotenv</code>
* Connect the LoRa module by USB.
* Rename <code>.env_example</code> to <code>.env</code> and edit it with valid data.
* <code>python3 main.py</code>
