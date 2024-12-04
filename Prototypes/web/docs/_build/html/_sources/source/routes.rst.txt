Ruter og Blueprints
--------------------

Flask-appen implementerer `Blueprints`, dette sørger for at applikasjonen kan utvikles
på et modulært vis. Rutene er lagt opp slik:



.. list-table:: Ruter
   :header-rows: 1
   :widths: 20 20 40 20

   * - URL
     - HTTP-metode
     - Beskrivelse
     - Modul
   * - `/`
     - GET
     - Sender brukeren videre til `/front`.
     - `app.landing`
   * - `/set_cookie`
     - GET
     - Oppretter `cookie` med `uuid` og en tilfeldig generert farge.
     - `app.landing`
   * - `/front/`
     - POST, GET
     - Mottar brukerens valg av styrke, og laster inn nettsiden.
     - `app.front`
   * - `/mqtt`
     - POST
     - Mottar melding og videresender til nettleser.
     - `app.mqtt`
   * - `/mqtt/location`
     - POST
     - Videresender lokasjonsdata til nettleser.
     - `app.mqtt`  
   * - `/mqtt/returndata`
     - POST
     - Håndterer returdata basert på lokasjon.
     - `app.mqtt`
   
Blueprints hjelper oss å definere kategorierer og underkategorier enkelt. 
Følgende kode demonstrerer denne funksjonen.

.. code-block:: python

    from flask import Blueprint, request, jsonify
    bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')

    @bp.route('/', methods=['POST'])
    # gir ruten /mqtt/
    def getmqtt():
        return jsonify({"status": "success"}), 200

    @bp.route('/location', methods=['POST'])
    # gir ruten /mqtt/location
    def handle_locations():
        return jsonify({"status": "success"}), 200

Som vi kan se fra eksempelet, hjelper Blueprint oss å holde koden modulær og kompakt.

Moduler
-------
Flask-appens moduler er organisert med følgende Blueprints:
- **app.api**: Håndterer API-forespørsler fra frontend.
- **app.mqtt**: Kommuniserer med MQTT-megleren.